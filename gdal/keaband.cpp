/*
 *  keaband.cpp
 *
 *  Created by Pete Bunting on 01/08/2012.
 *  Copyright 2012 LibKEA. All rights reserved.
 *
 *  This file is part of LibKEA.
 *
 *  Permission is hereby granted, free of charge, to any person 
 *  obtaining a copy of this software and associated documentation 
 *  files (the "Software"), to deal in the Software without restriction, 
 *  including without limitation the rights to use, copy, modify, 
 *  merge, publish, distribute, sublicense, and/or sell copies of the 
 *  Software, and to permit persons to whom the Software is furnished 
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be 
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR 
 *  ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
 *  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#include "keaband.h"
#include "keaoverview.h"
#include "keamaskband.h"
#include "kearat.h"

#include "gdal_rat.h"
#include "libkea/KEAAttributeTable.h"

#include <map>
#include <vector>
#include <limits>

// constructor
KEARasterBand::KEARasterBand( KEADataset *pDataset, int nSrcBand, GDALAccess eAccess, kealib::KEAImageIO *pImageIO, LockedRefCount *pRefCount )
{
    this->m_hMutex = CPLCreateMutex();
    CPLReleaseMutex( this->m_hMutex );
    
    this->poDS = pDataset; // our pointer onto the dataset
    this->nBand = nSrcBand; // this is the band we are
    this->m_eKEADataType = pImageIO->getImageBandDataType(nSrcBand); // get the data type as KEA enum
    this->eDataType = KEA_to_GDAL_Type( m_eKEADataType );       // convert to GDAL enum
    this->nBlockXSize = pImageIO->getImageBlockSize(nSrcBand);  // get the native blocksize
    this->nBlockYSize = pImageIO->getImageBlockSize(nSrcBand);
    this->nRasterXSize = this->poDS->GetRasterXSize();          // ask the dataset for the total image size
    this->nRasterYSize = this->poDS->GetRasterYSize();
    this->eAccess = eAccess;

    if( pImageIO->attributeTablePresent(nSrcBand) )
    {
        this->m_nAttributeChunkSize = pImageIO->getAttributeTableChunkSize(nSrcBand);
    }
    else
    {
        this->m_nAttributeChunkSize = -1; // don't report
    }

    // grab the imageio class and its refcount
    this->m_pImageIO = pImageIO;
    this->m_pRefCount = pRefCount;
    // increment the refcount as we now have a reference to imageio
    this->m_pRefCount->IncRef();

    // initialise overview variables
    m_nOverviews = 0;
    m_panOverviewBands = NULL;

    // mask band
    m_pMaskBand = NULL;
    m_bMaskBandOwned = false;

    // grab the description here
    this->sDescription = pImageIO->getImageBandDescription(nSrcBand);

    this->m_pAttributeTable = NULL;  // no RAT yet
    this->m_pColorTable = NULL;     // no color table yet

    // initialise the metadata as a CPLStringList
    m_papszMetadataList = NULL;
    this->UpdateMetadataList();
    m_pszHistoBinValues = NULL;
}

// destructor
KEARasterBand::~KEARasterBand()
{
    {
        CPLMutexHolderD( &m_hMutex );
        // destroy RAT if any
        delete this->m_pAttributeTable;
        // destroy color table if any
        delete this->m_pColorTable;
        // destroy the metadata
        CSLDestroy(this->m_papszMetadataList);
        if( this->m_pszHistoBinValues != NULL )
        {
            // histgram bin values as a string
            CPLFree(this->m_pszHistoBinValues);
        }
        // delete any overview bands
        this->deleteOverviewObjects();

        // if GDAL created the mask it will delete it
        if( m_bMaskBandOwned )
        {
            delete m_pMaskBand;
        }
    }
    // according to the docs, this is required
    this->FlushCache();

    // decrement the recount and delete if needed
    if( m_pRefCount->DecRef() )
    {
        try
        {
            m_pImageIO->close();
        }
        catch (const kealib::KEAIOException &e)
        {
        }
        delete m_pImageIO;
        delete m_pRefCount;
    }

    CPLDestroyMutex( m_hMutex );
    m_hMutex = NULL;
}

// internal method that updates the metadata into m_papszMetadataList
void KEARasterBand::UpdateMetadataList()
{
    CPLMutexHolderD( &m_hMutex );
    std::vector< std::pair<std::string, std::string> > data;

    // get all the metadata and iterate through
    data = this->m_pImageIO->getImageBandMetaData(this->nBand);
    for(auto iterMetaData = data.begin(); iterMetaData != data.end(); ++iterMetaData)
    {
        // add to our list
        m_papszMetadataList = CSLSetNameValue(m_papszMetadataList, iterMetaData->first.c_str(), iterMetaData->second.c_str());
    }
    // we have a pseudo metadata item that tells if we are thematic 
    // or continuous like the HFA driver
    if( this->m_pImageIO->getImageBandLayerType(this->nBand) == kealib::kea_continuous )
    {
        m_papszMetadataList = CSLSetNameValue(m_papszMetadataList, "LAYER_TYPE", "athematic" );
    }
    else
    {
        m_papszMetadataList = CSLSetNameValue(m_papszMetadataList, "LAYER_TYPE", "thematic" );
    }

    // STATISTICS_HISTONUMBINS
    const GDALRasterAttributeTable *pTable = this->GetDefaultRAT();
    if( pTable != NULL )
    {
        CPLString osWorkingResult;
        osWorkingResult.Printf( "%lu", (unsigned long)pTable->GetRowCount());
        m_papszMetadataList = CSLSetNameValue(m_papszMetadataList, "STATISTICS_HISTONUMBINS", osWorkingResult);

        // attribute table chunksize
        if( this->m_nAttributeChunkSize != -1 )
        {
            osWorkingResult.Printf( "%d", this->m_nAttributeChunkSize );
            m_papszMetadataList = CSLSetNameValue(m_papszMetadataList, "ATTRIBUTETABLE_CHUNKSIZE", osWorkingResult );
        }
    }
}

// internal method to set the histogram column from a string (for metadata)
CPLErr KEARasterBand::SetHistogramFromString(const char *pszString)
{
    // copy it so we can change it (put nulls in etc)
    char *pszBinValues = CPLStrdup(pszString);
    if( pszBinValues == NULL )
        return CE_Failure;

    // find the number of | chars
    int nRows = 0, i = 0;
    while( pszBinValues[i] != '\0' )
    {
        if( pszBinValues[i] == '|' )
            nRows++;
        i++;
    }

#ifdef HAVE_RFC40
    GDALRasterAttributeTable *pTable = this->GetDefaultRAT();
    if( pTable == NULL )
    {
        CPLFree(pszBinValues);
        return CE_Failure;
    }
    
    // find histogram column if it exists
    int nCol = pTable->GetColOfUsage(GFU_PixelCount);
    if( nCol == -1 )
    {
        if( pTable->CreateColumn("Histogram", GFT_Real, GFU_PixelCount) != CE_None )
            return CE_Failure;
        nCol = pTable->GetColumnCount() - 1;
    }
    if( nRows > pTable->GetRowCount() )
        pTable->SetRowCount(nRows);
#else
    // just create a blank rat then populate it
    GDALRasterAttributeTable *pTable = new GDALRasterAttributeTable();
    if( pTable->CreateColumn("Histogram", GFT_Real, GFU_PixelCount) != CE_None )
        return CE_Failure;
    int nCol = 0;
    pTable->SetRowCount(nRows);
#endif

    char * pszWork = pszBinValues;
    for( int nBin = 0; nBin < nRows; ++nBin )
    {
        char * pszEnd = strchr( pszWork, '|' );
        if ( pszEnd != NULL )
        {
            *pszEnd = 0;
            double dValue = CPLAtof( pszWork );
            pTable->SetValue(nBin, nCol, dValue);
            pszWork = pszEnd + 1;
        }
    }
    CPLFree(pszBinValues);

    // note: #ifndef
#ifndef HAVE_RFC40
    // need to set it back if we don't have RFC40
    this->SetDefaultRAT(pTable);
    delete pTable;
#endif

    return CE_None;
}

// get histogram as string with values separated by '|'
char *KEARasterBand::GetHistogramAsString()
{
    const GDALRasterAttributeTable *pTable = this->GetDefaultRAT();
    if( pTable == NULL )
        return NULL;
    int nRows = pTable->GetRowCount();
    // find histogram column if it exists
    int nCol = pTable->GetColOfUsage(GFU_PixelCount);
    if( nCol == -1 )
        return NULL;

    unsigned int nBufSize = 1024;
    char * pszBinValues = (char *)CPLMalloc( nBufSize );
    int    nBinValuesLen = 0;
    pszBinValues[0] = 0;

    for ( int nBin = 0; nBin < nRows; ++nBin )
    {
        char szBuf[32];
        // RAT's don't handle GUIntBig - use double instead. Cast back
#ifndef CPL_FRMT_GUIB
        snprintf( szBuf, 31, "%ull", (GUIntBig)pTable->GetValueAsDouble(nBin, nCol) );
#else        
        snprintf( szBuf, 31, CPL_FRMT_GUIB, (GUIntBig)pTable->GetValueAsDouble(nBin, nCol) );
#endif
        if ( ( nBinValuesLen + strlen( szBuf ) + 2 ) > nBufSize )
        {
            nBufSize *= 2;
            char* pszNewBinValues = (char *)VSIRealloc( pszBinValues, nBufSize );
            if (pszNewBinValues == NULL)
            {
                break;
            }
            pszBinValues = pszNewBinValues;
        }
        strcat( pszBinValues+nBinValuesLen, szBuf );
        strcat( pszBinValues+nBinValuesLen, "|" );
        nBinValuesLen += static_cast<int>(strlen(pszBinValues+nBinValuesLen));
    }

    return pszBinValues;
}

// internal method to create the overviews
void KEARasterBand::CreateOverviews(int nOverviews, int *panOverviewList)
{
    CPLMutexHolderD( &m_hMutex );
    // delete any existing overview bands
    this->deleteOverviewObjects();

    // allocate space
    m_panOverviewBands = (KEAOverview**)CPLMalloc(sizeof(KEAOverview*) * nOverviews);
    m_nOverviews = nOverviews;

    // loop through and create the overviews
    int nFactor, nXSize, nYSize;
    for( int nCount = 0; nCount < m_nOverviews; nCount++ )
    {
        nFactor = panOverviewList[nCount];
        // divide by the factor to get the new size
        nXSize = this->nRasterXSize / nFactor;
        nYSize = this->nRasterYSize / nFactor;

        // tell image io to create a new overview
        this->m_pImageIO->createOverview(this->nBand, nCount + 1, nXSize, nYSize);

        // create one of our objects to represent it
        m_panOverviewBands[nCount] = new KEAOverview((KEADataset*)this->poDS, this->nBand, GA_Update,
                                        this->m_pImageIO, this->m_pRefCount, nCount + 1, nXSize, nYSize);
    }
}

// virtual method to read a block
CPLErr KEARasterBand::IReadBlock( int nBlockXOff, int nBlockYOff, void * pImage )
{
    try
    {
        // GDAL deals in blocks - if we are at the end of a row
        // we need to adjust the amount read so we don't go over the edge
        int nxsize = this->nBlockXSize;
        int nxtotalsize = this->nBlockXSize * (nBlockXOff + 1);
        if( nxtotalsize > this->nRasterXSize )
        {
            nxsize -= (nxtotalsize - this->nRasterXSize);
        }
        int nysize = this->nBlockYSize;
        int nytotalsize = this->nBlockYSize * (nBlockYOff + 1);
        if( nytotalsize > this->nRasterYSize )
        {
            nysize -= (nytotalsize - this->nRasterYSize);
        }
        this->m_pImageIO->readImageBlock2Band( this->nBand, pImage, this->nBlockXSize * nBlockXOff,
                                            this->nBlockYSize * nBlockYOff,
                                            nxsize, nysize, this->nBlockXSize, this->nBlockYSize, 
                                            this->m_eKEADataType );
        return CE_None;
    }
    catch (const kealib::KEAIOException &e)
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                "Failed to read file: %s", e.what() );
        return CE_Failure;
    }
}

// virtual method to write a block
CPLErr KEARasterBand::IWriteBlock( int nBlockXOff, int nBlockYOff, void * pImage )
{
    try
    {
        // GDAL deals in blocks - if we are at the end of a row
        // we need to adjust the amount written so we don't go over the edge
        int nxsize = this->nBlockXSize;
        int nxtotalsize = this->nBlockXSize * (nBlockXOff + 1);
        if( nxtotalsize > this->nRasterXSize )
        {
            nxsize -= (nxtotalsize - this->nRasterXSize);
        }
        int nysize = this->nBlockYSize;
        int nytotalsize = this->nBlockYSize * (nBlockYOff + 1);
        if( nytotalsize > this->nRasterYSize )
        {
            nysize -= (nytotalsize - this->nRasterYSize);
        }

        this->m_pImageIO->writeImageBlock2Band( this->nBand, pImage, this->nBlockXSize * nBlockXOff,
                                            this->nBlockYSize * nBlockYOff,
                                            nxsize, nysize, this->nBlockXSize, this->nBlockYSize,
                                            this->m_eKEADataType );
        return CE_None;
    }
    catch (const kealib::KEAIOException &e)
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                "Failed to write file: %s", e.what() );
        return CE_Failure;
    }
}

void KEARasterBand::SetDescription(const char *pszDescription)
{
    CPLMutexHolderD( &m_hMutex );
    try
    {
        this->m_pImageIO->setImageBandDescription(this->nBand, pszDescription);
        this->sDescription = pszDescription;
    }
    catch (const kealib::KEAIOException &e)
    {
        // ignore?
    }
}

const char *KEARasterBand::GetDescription() const
{
    // do we need to implement this?
    return this->sDescription.c_str();
}

// set a metadata item
CPLErr KEARasterBand::SetMetadataItem(const char *pszName, const char *pszValue, const char *pszDomain)
{
    CPLMutexHolderD( &m_hMutex );
    // only deal with 'default' domain - no geolocation etc
    if( ( pszDomain != NULL ) && ( *pszDomain != '\0' ) )
        return CE_Failure;

    // kealib doesn't currently support removing values
    if( pszValue == NULL )
        return CE_Failure;

    try
    {
        // if it is LAYER_TYPE handle it seperately
        if( EQUAL( pszName, "LAYER_TYPE" ) )
        {
            if( EQUAL( pszValue, "athematic" ) )
            {
                this->m_pImageIO->setImageBandLayerType(this->nBand, kealib::kea_continuous );
            }
            else
            {
                this->m_pImageIO->setImageBandLayerType(this->nBand, kealib::kea_thematic );
            }
        }
        else if( EQUAL( pszName, "STATISTICS_HISTOBINVALUES" ) )
        {
            if( this->SetHistogramFromString(pszValue) != CE_None )
                return CE_Failure;
            else
                return CE_None;
        }
        else if( EQUAL( pszName, "STATISTICS_HISTONUMBINS" ) )
        {
#ifdef HAVE_RFC40
            GDALRasterAttributeTable *pTable = this->GetDefaultRAT();
            if( pTable != NULL )
                pTable->SetRowCount(atol(pszValue));
#else
            // this is quite hard so I'm leaving it
#endif
            // leave to update m_papszMetadataList below
        }
        else
        {
            // otherwise set it as normal
            this->m_pImageIO->setImageBandMetaData(this->nBand, pszName, pszValue );
        }
        // CSLSetNameValue will update if already there
        m_papszMetadataList = CSLSetNameValue( m_papszMetadataList, pszName, pszValue );
        return CE_None;
    }
    catch (const kealib::KEAIOException &e)
    {
        return CE_Failure;
    }
}

// get a single metadata item
const char *KEARasterBand::GetMetadataItem (const char *pszName, const char *pszDomain)
{
    CPLMutexHolderD( &m_hMutex );
    // only deal with 'default' domain - no geolocation etc
    if( ( pszDomain != NULL ) && ( *pszDomain != '\0' ) )
        return NULL;

    if(EQUAL( pszName, "STATISTICS_HISTOBINVALUES" ) )
    {
        if( m_pszHistoBinValues != NULL )
            CPLFree(m_pszHistoBinValues); // could have changed
        m_pszHistoBinValues = this->GetHistogramAsString();
        return m_pszHistoBinValues;
    }
    // get it out of the CSLStringList so we can be sure it is persistant
    return CSLFetchNameValue(m_papszMetadataList, pszName);
}

// get all the metadata as a CSLStringList - not thread safe
char **KEARasterBand::GetMetadata(const char *pszDomain)
{
    // only deal with 'default' domain - no geolocation etc
    if( ( pszDomain != NULL ) && ( *pszDomain != '\0' ) )
        return NULL;
    // Note: ignoring STATISTICS_HISTOBINVALUES as these are likely to be very long
    // not sure user should get those unless they really ask...

    // conveniently we already have it in this format
    return m_papszMetadataList; 
}

// set the metadata as a CSLStringList
CPLErr KEARasterBand::SetMetadata(char **papszMetadata, const char *pszDomain)
{
    CPLMutexHolderD( &m_hMutex );
    // only deal with 'default' domain - no geolocation etc
    if( ( pszDomain != NULL ) && ( *pszDomain != '\0' ) )
        return CE_Failure;
    int nIndex = 0;
    char *pszName;
    const char *pszValue;
    try
    {
        // iterate through each one
        while( papszMetadata[nIndex] != NULL )
        {
            pszValue = CPLParseNameValue( papszMetadata[nIndex], &pszName );

            // it is LAYER_TYPE? if so handle seperately
            if( EQUAL( pszName, "LAYER_TYPE" ) )
            {
                if( EQUAL( pszValue, "athematic" ) )
                {
                    this->m_pImageIO->setImageBandLayerType(this->nBand, kealib::kea_continuous );
                }
                else
                {
                    this->m_pImageIO->setImageBandLayerType(this->nBand, kealib::kea_thematic );
                }
            }
            else if( EQUAL( pszName, "STATISTICS_HISTOBINVALUES" ) )
            {
                if( this->SetHistogramFromString(pszValue) != CE_None )
                    return CE_Failure;
            }
            else
            {
                // write it into the image
                this->m_pImageIO->setImageBandMetaData(this->nBand, pszName, pszValue );
            }
            nIndex++;
        }
    }
    catch (const kealib::KEAIOException &e)
    {
        return CE_Failure;
    }
    // destroy our list and duplicate the one passed in
    // and use that as our list from now on
    CSLDestroy(m_papszMetadataList);
    m_papszMetadataList = CSLDuplicate(papszMetadata);
    return CE_None;
}

// get the no data value
double KEARasterBand::GetNoDataValue(int *pbSuccess)
{
    try
    {
        double dVal;
        this->m_pImageIO->getNoDataValue(this->nBand, &dVal, kealib::kea_64float);
        if( pbSuccess != NULL )
            *pbSuccess = 1;

        return dVal;
    }
    catch (const kealib::KEAIOException &e)
    {
        if( pbSuccess != nullptr )
            *pbSuccess = 0;
        return -1;
    }
}

int64_t KEARasterBand::GetNoDataValueAsInt64(int *pbSuccess)
{
    try
    {
        int64_t nVal;
        this->m_pImageIO->getNoDataValue(this->nBand, &nVal, kealib::kea_64int);
        if( pbSuccess != nullptr )
            *pbSuccess = 1;

        return nVal;
    }
    catch (const kealib::KEAIOException &)
    {
        if( pbSuccess != nullptr )
            *pbSuccess = 0;
        return -1;
    }
}

uint64_t KEARasterBand::GetNoDataValueAsUInt64(int *pbSuccess)
{
    try
    {
        uint64_t nVal;
        this->m_pImageIO->getNoDataValue(this->nBand, &nVal, kealib::kea_64uint);
        if( pbSuccess != nullptr )
            *pbSuccess = 1;

        return nVal;
    }
    catch (const kealib::KEAIOException &)
    {
        if( pbSuccess != nullptr )
            *pbSuccess = 0;
        return std::numeric_limits<uint64_t>::max();
    }
}

// set the no data value
CPLErr KEARasterBand::SetNoDataValue(double dfNoData)
{
    // need to check for out of range values
    bool bSet = true;
    GDALDataType dtype = this->GetRasterDataType();
    switch( dtype )
    {
        case GDT_Byte:
            bSet = (dfNoData >= 0) && (dfNoData <= UCHAR_MAX);
            break;
        case GDT_UInt16:
            bSet = (dfNoData >= 0) && (dfNoData <= USHRT_MAX);
            break;
        case GDT_Int16:
            bSet = (dfNoData >= SHRT_MIN) && (dfNoData <= SHRT_MAX);
            break;
        case GDT_UInt32:
            bSet = (dfNoData >= 0) && (dfNoData <= UINT_MAX);
            break;
        case GDT_Int32:
            bSet = (dfNoData >= INT_MIN) && (dfNoData <= INT_MAX);
            break;
        default:
            // for other types we can't really tell if outside the range
            break;
    }

    try
    {
        if( bSet )
        {
            this->m_pImageIO->setNoDataValue(this->nBand, &dfNoData, kealib::kea_64float);
        }
        else
        {
            this->m_pImageIO->undefineNoDataValue(this->nBand);
        }
        return CE_None;
    }
    catch (const kealib::KEAIOException &e)
    {
        return CE_Failure;
    }
}

CPLErr KEARasterBand::SetNoDataValueAsInt64(int64_t nNoData)
{
    try
    {
        this->m_pImageIO->setNoDataValue(this->nBand, &nNoData, kealib::kea_64int);
    }
    catch (const kealib::KEAIOException &)
    {
        return CE_Failure;
    }
    return CE_None;
}

CPLErr KEARasterBand::SetNoDataValueAsUInt64(uint64_t nNoData)
{
    try
    {
        this->m_pImageIO->setNoDataValue(this->nBand, &nNoData, kealib::kea_64uint);
    }
    catch (const kealib::KEAIOException &)
    {
        return CE_Failure;
    }
    return CE_None;
}

CPLErr KEARasterBand::DeleteNoDataValue()
{
    try
    {
        m_pImageIO->undefineNoDataValue(this->nBand);
        return CE_None;
    }
    catch (const kealib::KEAIOException &)
    {
        return CE_Failure;
    }
}

CPLErr KEARasterBand::GetDefaultHistogram( double *pdfMin, double *pdfMax,
                                        int *pnBuckets, GUIntBig ** ppanHistogram,
                                        int bForce,
                                        GDALProgressFunc fn, void *pProgressData)
{
    if( bForce )
    {
#ifdef HAVE_RFC40
        return GDALPamRasterBand::GetDefaultHistogram(pdfMin, pdfMax, pnBuckets, 
                        ppanHistogram, bForce, fn, pProgressData);
#else
        // GetDefaultHistogram actually only introduced in more recent GDAL so shouldn't 
        // be being used. Just fail.
        return CE_Failure;
#endif
    }
    else
    {
        // returned cached if avail
        // I've used the RAT interface here as it deals with data type
        // conversions. Would be nice to have GUIntBig support in RAT though...
#ifdef HAVE_RFC40
        GDALRasterAttributeTable *pTable = this->GetDefaultRAT();
#else
        const GDALRasterAttributeTable *pTable = this->GetDefaultRAT();
#endif
        if( pTable == NULL )
            return CE_Failure;
        int nRows = pTable->GetRowCount();
        // find histogram column if it exists
        int nCol = pTable->GetColOfUsage(GFU_PixelCount);
        if( nCol == -1 )
            return CE_Warning;

        double dfRow0Min, dfBinSize;
        if( !pTable->GetLinearBinning(&dfRow0Min, &dfBinSize) )
            return CE_Warning;

#ifdef HAVE_RFC40
        *ppanHistogram = (GUIntBig*)VSIMalloc2(nRows, sizeof(GUIntBig));
        if( *ppanHistogram == NULL )
        {
            CPLError( CE_Failure, CPLE_OutOfMemory,
                    "Memory Allocation failed in KEARasterBand::GetDefaultHistogram");
            return CE_Failure;
        }
        double *pDoubleHisto = (double*)VSIMalloc2(nRows, sizeof(double));
        if( pDoubleHisto == NULL )
        {
            CPLFree(*ppanHistogram);
            CPLError( CE_Failure, CPLE_OutOfMemory,
                    "Memory Allocation failed in KEARasterBand::GetDefaultHistogram");
            return CE_Failure;
        }

        if( pTable->ValuesIO(GF_Read, nCol, 0, nRows, pDoubleHisto) != CE_None )
            return CE_Failure;

        // convert to GUIntBig
        for( int n = 0; n < nRows; n++ )
            (*ppanHistogram)[n] = pDoubleHisto[n];

        CPLFree(pDoubleHisto);
#else
        for( int n = 0; n < nRows; n++ )
            (*ppanHistogram)[n] = (GUIntBig)pTable->GetValueAsDouble(n, nCol);
#endif
        *pnBuckets = nRows;
        *pdfMin = dfRow0Min;
        *pdfMax = dfRow0Min + ((nRows + 1) * dfBinSize);
        return CE_None;
    }
}

CPLErr KEARasterBand::SetDefaultHistogram( double dfMin, double dfMax,
                                        int nBuckets, GUIntBig *panHistogram )
{
#ifdef HAVE_RFC40
    GDALRasterAttributeTable *pTable = this->GetDefaultRAT();
    if( pTable == NULL )
        return CE_Failure;

    // find histogram column if it exists
    int nCol = pTable->GetColOfUsage(GFU_PixelCount);
    if( nCol == -1 )
    {
        if( pTable->CreateColumn("Histogram", GFT_Real, GFU_PixelCount) != CE_None )
            return CE_Failure;
        nCol = pTable->GetColumnCount() - 1;
    }
    if( nBuckets > pTable->GetRowCount() )
        pTable->SetRowCount(nBuckets);

    // convert to double (RATs don't take GUIntBig yet)
    double *pDoubleHist = (double*)VSIMalloc2(nBuckets, sizeof(double));
    if( pDoubleHist == NULL )
    {
        CPLError( CE_Failure, CPLE_OutOfMemory,
                "Memory Allocation failed in KEARasterBand::SetDefaultHistogram");
        return CE_Failure;
    }

    for( int n = 0; n < nBuckets; n++ )
        pDoubleHist[n] = static_cast<double>(panHistogram[n]);

    if( pTable->ValuesIO(GF_Write, nCol, 0, nBuckets, pDoubleHist) != CE_None )
    {
        CPLFree(pDoubleHist);
        return CE_Failure;
    }

    CPLFree(pDoubleHist);

#else
    // just create a blank rat then populate it
    GDALRasterAttributeTable *pTable = new GDALRasterAttributeTable();
    if( pTable->CreateColumn("Histogram", GFT_Real, GFU_PixelCount) != CE_None )
        return CE_Failure;
    int nCol = 0;
    pTable->SetRowCount(nBuckets);

    for( int n = 0; n < nBuckets; n++ )
        pTable->SetValue(n, nCol, (double)panHistogram[n]);

    // need to set it back if we don't have RFC40
    this->SetDefaultRAT(pTable);
    delete pTable;
#endif

    return CE_None;
}

#ifdef HAVE_RFC40
GDALRasterAttributeTable *KEARasterBand::GetDefaultRAT()
{
    CPLMutexHolderD( &m_hMutex );
    if( this->m_pAttributeTable == NULL )
    {
        try
        {
            // we assume this is never NULL - creates a new one if none exists
            // (or raises exception)
            kealib::KEAAttributeTable *pKEATable = this->m_pImageIO->getAttributeTable(kealib::kea_att_file, this->nBand);
            this->m_pAttributeTable = new KEARasterAttributeTable(pKEATable, this);
        }
        catch(const kealib::KEAException &e)
        {
            CPLError( CE_Failure, CPLE_AppDefined, "Failed to read attributes: %s", e.what() );
        }
    }
    return this->m_pAttributeTable;
}

#else
// read the attributes into a GDALAttributeTable
const GDALRasterAttributeTable *KEARasterBand::GetDefaultRAT()
{
    CPLMutexHolderD( &m_hMutex );
    if( this->m_pAttributeTable == NULL )
    {
        try
        {
            if( this->m_pImageIO->attributeTablePresent(this->nBand) )
            {
                // we need to create one
                this->m_pAttributeTable = new GDALRasterAttributeTable();

                // we assume this is never NULL - creates a new one if none exists
                kealib::KEAAttributeTable *pKEATable = this->m_pImageIO->getAttributeTable(kealib::kea_att_mem, this->nBand);
    
                // create a mapping between GDAL column number and the field info
                std::vector<kealib::KEAATTField> vecKEAField;
                for( size_t nColumnIndex = 0; nColumnIndex < pKEATable->getMaxGlobalColIdx(); nColumnIndex++ )
                {
                    kealib::KEAATTField sKEAField;
                    try
                    {
                        sKEAField = pKEATable->getField(nColumnIndex);
                    }
                    catch(const kealib::KEAATTException &e)
                    {
                        // pKEATable->getField raised exception because we have a missing column
                        continue;
                    }

                    GDALRATFieldType eGDALType;
                    switch( sKEAField.dataType )
                    {
                        case kealib::kea_att_bool:
                        case kealib::kea_att_int:
                            eGDALType = GFT_Integer;
                            break;
                        case kealib::kea_att_float:
                            eGDALType = GFT_Real;
                            break;
                        case kealib::kea_att_string:
                            eGDALType = GFT_String;
                            break;
                        default:
                            continue;
                            break;
                    }

                    GDALRATFieldUsage eGDALUsage;
                    if( sKEAField.usage == "PixelCount" )
                        eGDALUsage = GFU_PixelCount;
                    else if( sKEAField.usage == "Name" )
                        eGDALUsage = GFU_Name;
                    else if( sKEAField.usage == "Red" )
                        eGDALUsage = GFU_Red;
                    else if( sKEAField.usage == "Green" )
                        eGDALUsage = GFU_Green;
                    else if( sKEAField.usage == "Blue" )
                        eGDALUsage = GFU_Blue;
                    else if( sKEAField.usage == "Alpha" )
                        eGDALUsage = GFU_Alpha;
                    else
                    {
                        // don't recognise any other special names - generic column
                        eGDALUsage = GFU_Generic;
                    }

                    if( this->m_pAttributeTable->CreateColumn(sKEAField.name.c_str(), eGDALType, eGDALUsage) != CE_None )
                    {
                        CPLError( CE_Warning, CPLE_AppDefined, "Unable to create column %s", sKEAField.name.c_str() );
                        continue;
                    }

                    vecKEAField.push_back(sKEAField);
                }

                // OK now we have filled in vecKEAField we can go through each row and fill in the fields
                for( size_t nRowIndex = 0; nRowIndex < pKEATable->getSize(); nRowIndex++ )
                {
                    // get the feature
                    kealib::KEAATTFeature *pKEAFeature = pKEATable->getFeature(nRowIndex);

                    // iterate through the columns - same order as we added columns to GDAL
                    int nGDALColNum = 0;
                    for( auto itr = vecKEAField.begin(); itr != vecKEAField.end(); itr++ )
                    {
                        kealib::KEAATTField sKEAField = (*itr);
                        if( sKEAField.dataType == kealib::kea_att_bool )
                        {
                            bool bVal = pKEAFeature->boolFields->at(sKEAField.idx);
                            int nVal = bVal? 1 : 0; // convert to int - GDAL doesn't do bool
                            this->m_pAttributeTable->SetValue(nRowIndex, nGDALColNum, nVal);
                        }
                        else if( sKEAField.dataType == kealib::kea_att_int )
                        {
                            int nVal = pKEAFeature->intFields->at(sKEAField.idx);
                            this->m_pAttributeTable->SetValue(nRowIndex, nGDALColNum, nVal);
                        }
                        else if( sKEAField.dataType == kealib::kea_att_float )
                        {
                            double dVal = pKEAFeature->floatFields->at(sKEAField.idx);
                            this->m_pAttributeTable->SetValue(nRowIndex, nGDALColNum, dVal);
                        }
                        else
                        {
                            std::string sVal = pKEAFeature->strFields->at(sKEAField.idx);
                            this->m_pAttributeTable->SetValue(nRowIndex, nGDALColNum, sVal.c_str());
                        }
                        nGDALColNum++;
                    }
                }

                delete pKEATable;
            }
        }
        catch(const kealib::KEAException &e)
        {
            CPLError( CE_Failure, CPLE_AppDefined, "Failed to read attributes: %s", e.what() );
            delete this->m_pAttributeTable;
            this->m_pAttributeTable = NULL;
        }
    }
    return this->m_pAttributeTable;
}
#endif // HAVE_RFC40

#ifdef HAVE_RFC40
CPLErr KEARasterBand::SetDefaultRAT(const GDALRasterAttributeTable *poRAT)
{
    if( poRAT == NULL )
        return CE_Failure;

    try
    {
        KEARasterAttributeTable *pKEATable = (KEARasterAttributeTable*)this->GetDefaultRAT();
        if( pKEATable == NULL )
            return CE_Failure;

        int numRows = poRAT->GetRowCount();
        pKEATable->SetRowCount(numRows);

        for( int nGDALColumnIndex = 0; nGDALColumnIndex < poRAT->GetColumnCount(); nGDALColumnIndex++ )
        {
            const char *pszColumnName = poRAT->GetNameOfCol(nGDALColumnIndex);
            GDALRATFieldType eFieldType = poRAT->GetTypeOfCol(nGDALColumnIndex);

            // do we have it?
            bool bExists = false;
            int nKEAColumnIndex;
            for( nKEAColumnIndex = 0; nKEAColumnIndex < pKEATable->GetColumnCount(); nKEAColumnIndex++ )
            {
                if( EQUAL(pszColumnName, pKEATable->GetNameOfCol(nKEAColumnIndex) ))
                {
                    bExists = true;
                    break;
                }
            }

            if( !bExists )
            {
                if( pKEATable->CreateColumn(pszColumnName, eFieldType,
                                            poRAT->GetUsageOfCol(nGDALColumnIndex) ) != CE_None )
                {
                    CPLError( CE_Failure, CPLE_AppDefined, "Failed to create column");
                    return CE_Failure;
                }
                nKEAColumnIndex = pKEATable->GetColumnCount() - 1;
            }

            // ok now copy data
            if( eFieldType == GFT_Integer )
            {
                int *panIntData = (int*)VSIMalloc2(numRows, sizeof(int));
                if( panIntData == NULL )
                {
                    CPLError( CE_Failure, CPLE_OutOfMemory,
                        "Memory Allocation failed in KEARasterAttributeTable::SetDefaultRAT");
                    return CE_Failure;
                }

                if( ((GDALRasterAttributeTable*)poRAT)->ValuesIO(GF_Read, nGDALColumnIndex, 0, numRows, panIntData ) == CE_None )
                {
                    pKEATable->ValuesIO(GF_Write, nKEAColumnIndex, 0, numRows, panIntData);
                }
                CPLFree(panIntData);
            }
            else if( eFieldType == GFT_Real )
            {
                double *padfFloatData = (double*)VSIMalloc2(numRows, sizeof(double));
                if( padfFloatData == NULL )
                {
                    CPLError( CE_Failure, CPLE_OutOfMemory,
                        "Memory Allocation failed in KEARasterAttributeTable::SetDefaultRAT");
                    return CE_Failure;
                }

                if( ((GDALRasterAttributeTable*)poRAT)->ValuesIO(GF_Read, nGDALColumnIndex, 0, numRows, padfFloatData ) == CE_None )
                {
                    pKEATable->ValuesIO(GF_Write, nKEAColumnIndex, 0, numRows, padfFloatData);
                }
                CPLFree(padfFloatData);
            }
            else
            {
                char **papszStringData = (char**)VSIMalloc2(numRows, sizeof(char*));
                if( papszStringData == NULL )
                {
                    CPLError( CE_Failure, CPLE_OutOfMemory,
                        "Memory Allocation failed in KEARasterAttributeTable::SetDefaultRAT");
                    return CE_Failure;
                }

                if( ((GDALRasterAttributeTable*)poRAT)->ValuesIO(GF_Read, nGDALColumnIndex, 0, numRows, papszStringData ) == CE_None )
                {
                    pKEATable->ValuesIO(GF_Write, nKEAColumnIndex, 0, numRows, papszStringData);
                    for( int n = 0; n < numRows; n++ )
                        CPLFree(papszStringData[n]);
                }
                CPLFree(papszStringData);

            }
        }
    }
    catch(const kealib::KEAException &e)
    {
        CPLError( CE_Failure, CPLE_AppDefined, "Failed to write attributes: %s", e.what() );
        return CE_Failure;
    }
    return CE_None;
}
#else
CPLErr KEARasterBand::SetDefaultRAT(const GDALRasterAttributeTable *poRAT)
{
    if( poRAT == NULL )
        return CE_Failure;

    CPLMutexHolderD( &m_hMutex );
    try
    {
        // we assume this is never NULL - creates a new one if none exists
        kealib::KEAAttributeTable *pKEATable = this->m_pImageIO->getAttributeTable(kealib::kea_att_mem, this->nBand);

        // add rows to the table if needed
        if( pKEATable->getSize() < (size_t)poRAT->GetRowCount() )
        {
            pKEATable->addRows( poRAT->GetRowCount() - pKEATable->getSize() );
        }

        // mapping between GDAL column indices and kealib::KEAATTField
        std::map<int, kealib::KEAATTField> mapGDALtoKEA;

        for( int nGDALColumnIndex = 0; nGDALColumnIndex < poRAT->GetColumnCount(); nGDALColumnIndex++ )
        {
            const char *pszColumnName = poRAT->GetNameOfCol(nGDALColumnIndex);
            bool bExists = true;
            kealib::KEAATTField sKEAField;
            try
            {
                sKEAField = pKEATable->getField(pszColumnName);
                // if this works we assume same usage, type etc
            }
            catch(const kealib::KEAATTException &e)
            {
                // doesn't exist on file - create it
                bExists = false;
            }

            if( ! bExists )
            {
                std::string strUsage = "Generic";
                GDALRATFieldType eType = poRAT->GetTypeOfCol(nGDALColumnIndex); // may need to update
                switch(poRAT->GetUsageOfCol(nGDALColumnIndex))
                {
                    case GFU_PixelCount:
                        strUsage = "PixelCount";
                        eType = GFT_Real;
                        break;
                    case GFU_Name:
                        strUsage = "Name";
                        eType = GFT_String;
                        break;
                    case GFU_Red:
                        strUsage = "Red";
                        eType = GFT_Integer;
                        break;
                    case GFU_Green:
                        strUsage = "Green";
                        eType = GFT_Integer;
                        break;
                    case GFU_Blue:
                        strUsage = "Blue";
                        eType = GFT_Integer;
                        break;
                    case GFU_Alpha:
                        strUsage = "Alpha";
                        eType = GFT_Integer;
                        break;
                    default:
                        // leave as "Generic"
                        break;
                }

                if(eType == GFT_Integer)
                {
                    pKEATable->addAttIntField(pszColumnName, 0, strUsage);
                }
                else if(eType == GFT_Real)
                {
                    pKEATable->addAttFloatField(pszColumnName, 0, strUsage);
                }
                else
                {
                    pKEATable->addAttStringField(pszColumnName, "", strUsage);
                }

                // assume we can just grab this now
                sKEAField = pKEATable->getField(pszColumnName);
            }
            // insert into map
            mapGDALtoKEA[nGDALColumnIndex] = sKEAField;
        }

        // go through each row to be added
        for( int nRowIndex = 0; nRowIndex < poRAT->GetRowCount(); nRowIndex++ )
        {
            // get the feature - don't need to set this back since it is a pointer to 
            // internal datastruct
            kealib::KEAATTFeature *pKEAFeature = pKEATable->getFeature(nRowIndex);

            // iterate through the map
            for( auto itr = mapGDALtoKEA.begin(); itr != mapGDALtoKEA.end(); itr++ )
            {
                // get the KEA field from the map
                int nGDALColIndex = (*itr).first;
                kealib::KEAATTField sKEAField = (*itr).second;

                if( sKEAField.dataType == kealib::kea_att_bool )
                {
                    // write it as a bool even tho GDAL stores as int
                    bool bVal = poRAT->GetValueAsInt(nRowIndex, nGDALColIndex) != 0;
                    pKEAFeature->boolFields->at(sKEAField.idx) = bVal;
                }
                else if( sKEAField.dataType == kealib::kea_att_int )
                {
                    int nVal = poRAT->GetValueAsInt(nRowIndex, nGDALColIndex);
                    pKEAFeature->intFields->at(sKEAField.idx) = nVal;
                }
                else if( sKEAField.dataType == kealib::kea_att_float )
                {
                    double dVal = poRAT->GetValueAsDouble(nRowIndex, nGDALColIndex);
                    pKEAFeature->floatFields->at(sKEAField.idx) = dVal;
                }
                else
                {
                    const char *pszValue = poRAT->GetValueAsString(nRowIndex, nGDALColIndex);
                    pKEAFeature->strFields->at(sKEAField.idx) = pszValue;
                }
            }
        }

        this->m_pImageIO->setAttributeTable(pKEATable, this->nBand);
        delete pKEATable;

        // our cached attribute table object is now out of date
        // delete it and next call to GetDefaultRAT() will re-read it
        delete this->m_pAttributeTable;
        this->m_pAttributeTable = NULL;
    }
    catch(const kealib::KEAException &e)
    {
        CPLError( CE_Failure, CPLE_AppDefined, "Failed to write attributes: %s", e.what() );
        return CE_Failure;
    }
    return CE_None;
}
#endif // HAVE_RFC40

#ifdef HAVE_RFC40
GDALColorTable *KEARasterBand::GetColorTable()
{
    CPLMutexHolderD( &m_hMutex );
    if( this->m_pColorTable == NULL )
    {
        try
        {
            GDALRasterAttributeTable *pKEATable = this->GetDefaultRAT();
            if( pKEATable == NULL )
                return NULL;
            int nRedIdx = -1;
            int nGreenIdx = -1;
            int nBlueIdx = -1;
            int nAlphaIdx = -1;

            for( int nColIdx = 0; nColIdx < pKEATable->GetColumnCount(); nColIdx++ )
            {
                if( pKEATable->GetTypeOfCol(nColIdx) == GFT_Integer )
                {
                    GDALRATFieldUsage eFieldUsage = pKEATable->GetUsageOfCol(nColIdx);
                    if( eFieldUsage == GFU_Red )
                        nRedIdx = nColIdx;
                    else if( eFieldUsage == GFU_Green )
                        nGreenIdx = nColIdx;
                    else if( eFieldUsage == GFU_Blue )
                        nBlueIdx = nColIdx;
                    else if( eFieldUsage == GFU_Alpha )
                        nAlphaIdx = nColIdx;
                }
            }

            if( ( nRedIdx != -1 ) && ( nGreenIdx != -1 ) && ( nBlueIdx != -1 ) && ( nAlphaIdx != -1 ) )
            {
                // we need to create one - only do RGB palettes
                this->m_pColorTable = new GDALColorTable(GPI_RGB);

                // OK go through each row and fill in the fields
                for( int nRowIndex = 0; nRowIndex < pKEATable->GetRowCount(); nRowIndex++ )
                {
                    // maybe could be more efficient using ValuesIO
                    GDALColorEntry colorEntry;
                    colorEntry.c1 = pKEATable->GetValueAsInt(nRowIndex, nRedIdx);
                    colorEntry.c2 = pKEATable->GetValueAsInt(nRowIndex, nGreenIdx);
                    colorEntry.c3 = pKEATable->GetValueAsInt(nRowIndex, nBlueIdx);
                    colorEntry.c4 = pKEATable->GetValueAsInt(nRowIndex, nAlphaIdx);
                    this->m_pColorTable->SetColorEntry(nRowIndex, &colorEntry);
                }
            }
        }
        catch(const kealib::KEAException &e)
        {
            CPLError( CE_Failure, CPLE_AppDefined, "Failed to read color table: %s", e.what() );
            delete this->m_pColorTable;
            this->m_pColorTable = NULL;
        }
    }
    return this->m_pColorTable;
}
#else

GDALColorTable *KEARasterBand::GetColorTable()
{
    CPLMutexHolderD( &m_hMutex );
    if( this->m_pColorTable == NULL )
    {
        try
        {
            // see if there is a suitable attribute table with color columns
            if( this->m_pImageIO->attributeTablePresent(this->nBand) )
            {
                // we assume this is never NULL - creates a new one if none exists
                kealib::KEAAttributeTable *pKEATable = this->m_pImageIO->getAttributeTable(kealib::kea_att_mem, this->nBand);
    
                // create a mapping between color entry number and the field info
                std::vector<kealib::KEAATTField> vecKEAField(4);
                for( size_t nColumnIndex = 0; nColumnIndex < pKEATable->getMaxGlobalColIdx(); nColumnIndex++ )
                {
                    kealib::KEAATTField sKEAField;
                    try
                    {
                        sKEAField = pKEATable->getField(nColumnIndex);
                    }
                    catch(const kealib::KEAATTException &e)
                    {
                        // pKEATable->getField raised exception because we have a missing column
                        continue;
                    }

                    // color tables are only int as far as I am aware
                    if( sKEAField.dataType == kealib::kea_att_int )
                    {
                        // check the 'usage' column
                        // we don't check the name also (maybe we should?)
                        // store in the right place in our vector
                        if( sKEAField.usage == "Red" )
                            vecKEAField[0] = sKEAField;
                        if( sKEAField.usage == "Green" )
                            vecKEAField[1] = sKEAField;
                        if( sKEAField.usage == "Blue" )
                            vecKEAField[2] = sKEAField;
                        if( sKEAField.usage == "Alpha" )
                            vecKEAField[3] = sKEAField;
                    }

                }

                // check that we did get a valid field for each color
                // the usage field will still be empty if not set above
                bool bHaveCT = true;
                for( auto itr = vecKEAField.begin(); (itr != vecKEAField.end()) && bHaveCT; itr++ )
                {
                    if( (*itr).usage.empty() )
                        bHaveCT = false;
                }

                if( bHaveCT )
                {
                    // we need to create one - only do RGB palettes
                    this->m_pColorTable = new GDALColorTable(GPI_RGB);

                    // OK go through each row and fill in the fields
                    for( size_t nRowIndex = 0; nRowIndex < pKEATable->getSize(); nRowIndex++ )
                    {
                        // get the feature
                        kealib::KEAATTFeature *pKEAFeature = pKEATable->getFeature(nRowIndex);

                        GDALColorEntry colorEntry;
                        colorEntry.c1 = pKEAFeature->intFields->at(vecKEAField[0].idx);
                        colorEntry.c2 = pKEAFeature->intFields->at(vecKEAField[1].idx);
                        colorEntry.c3 = pKEAFeature->intFields->at(vecKEAField[2].idx);
                        colorEntry.c4 = pKEAFeature->intFields->at(vecKEAField[3].idx);

                        this->m_pColorTable->SetColorEntry(nRowIndex, &colorEntry);
                    }
                }

                delete pKEATable;
            }
        }
        catch(const kealib::KEAException &e)
        {
            CPLError( CE_Failure, CPLE_AppDefined, "Failed to read color table: %s", e.what() );
            delete this->m_pColorTable;
            this->m_pColorTable = NULL;
        }
    }
    return this->m_pColorTable;
}
#endif // HAVE_RFC40

#ifdef HAVE_RFC40
CPLErr KEARasterBand::SetColorTable(GDALColorTable *poCT)
{
    if( poCT == NULL )
        return CE_Failure;

    CPLMutexHolderD( &m_hMutex );
    try
    {
        GDALRasterAttributeTable *pKEATable = this->GetDefaultRAT();
        if( pKEATable == NULL )
            return CE_Failure;
        int nRedIdx = -1;
        int nGreenIdx = -1;
        int nBlueIdx = -1;
        int nAlphaIdx = -1;

        if( poCT->GetColorEntryCount() > pKEATable->GetRowCount() )
        {
            pKEATable->SetRowCount(poCT->GetColorEntryCount());
        }

        for( int nColIdx = 0; nColIdx < pKEATable->GetColumnCount(); nColIdx++ )
        {
            if( pKEATable->GetTypeOfCol(nColIdx) == GFT_Integer )
            {
                GDALRATFieldUsage eFieldUsage = pKEATable->GetUsageOfCol(nColIdx);
                if( eFieldUsage == GFU_Red )
                    nRedIdx = nColIdx;
                else if( eFieldUsage == GFU_Green )
                    nGreenIdx = nColIdx;
                else if( eFieldUsage == GFU_Blue )
                    nBlueIdx = nColIdx;
                else if( eFieldUsage == GFU_Alpha )
                    nAlphaIdx = nColIdx;
            }
        }

        // create if needed
        if( nRedIdx == -1 )
        {
            if( pKEATable->CreateColumn("Red", GFT_Integer, GFU_Red ) != CE_None )
            {
                CPLError( CE_Failure, CPLE_AppDefined, "Failed to create column" );
                return CE_Failure;
            }
            nRedIdx = pKEATable->GetColumnCount() - 1;
        }
        if( nGreenIdx == -1 )
        {
            if( pKEATable->CreateColumn("Green", GFT_Integer, GFU_Green ) != CE_None )
            {
                CPLError( CE_Failure, CPLE_AppDefined, "Failed to create column" );
                return CE_Failure;
            }
            nGreenIdx = pKEATable->GetColumnCount() - 1;
        }
        if( nBlueIdx == -1 )
        {
            if( pKEATable->CreateColumn("Blue", GFT_Integer, GFU_Blue ) != CE_None )
            {
                CPLError( CE_Failure, CPLE_AppDefined, "Failed to create column" );
                return CE_Failure;
            }
            nBlueIdx = pKEATable->GetColumnCount() - 1;
        }
        if( nAlphaIdx == -1 )
        {
            if( pKEATable->CreateColumn("Alpha", GFT_Integer, GFU_Alpha ) != CE_None )
            {
                CPLError( CE_Failure, CPLE_AppDefined, "Failed to create column" );
                return CE_Failure;
            }
            nAlphaIdx = pKEATable->GetColumnCount() - 1;
        }

        // OK go through each row and fill in the fields
        for( int nRowIndex = 0; nRowIndex < poCT->GetColorEntryCount(); nRowIndex++ )
        {
            // maybe could be more efficient using ValuesIO
            GDALColorEntry colorEntry;
            poCT->GetColorEntryAsRGB(nRowIndex, &colorEntry);
            pKEATable->SetValue(nRowIndex, nRedIdx, colorEntry.c1);
            pKEATable->SetValue(nRowIndex, nGreenIdx, colorEntry.c2);
            pKEATable->SetValue(nRowIndex, nBlueIdx, colorEntry.c3);
            pKEATable->SetValue(nRowIndex, nAlphaIdx, colorEntry.c4);
        }

        // out of date
        delete this->m_pColorTable;
        this->m_pColorTable = NULL;
    }
    catch(const kealib::KEAException &e)
    {
        CPLError( CE_Failure, CPLE_AppDefined, "Failed to write color table: %s", e.what() );
        return CE_Failure;
    }
    return CE_None;
}

#else

CPLErr KEARasterBand::SetColorTable(GDALColorTable *poCT)
{
    if( poCT == NULL )
        return CE_Failure;

    CPLMutexHolderD( &m_hMutex );
    try
    {
        // we assume this is never NULL - creates a new one if none exists
        kealib::KEAAttributeTable *pKEATable = this->m_pImageIO->getAttributeTable(kealib::kea_att_mem, this->nBand);

        // add rows to the table if needed
        if( pKEATable->getSize() < (size_t)poCT->GetColorEntryCount() )
        {
            pKEATable->addRows( poCT->GetColorEntryCount() - pKEATable->getSize() );
        }

        // create a mapping between color entry number and the field info
        std::vector<kealib::KEAATTField> vecKEAField(4);
        for( size_t nColumnIndex = 0; nColumnIndex < pKEATable->getMaxGlobalColIdx(); nColumnIndex++ )
        {
            kealib::KEAATTField sKEAField;
            try
            {
                sKEAField = pKEATable->getField(nColumnIndex);
            }
            catch(const kealib::KEAATTException &e)
            {
                // pKEATable->getField raised exception because we have a missing column
                continue;
            }

            // color tables are only int as far as I am aware
            if( sKEAField.dataType == kealib::kea_att_int )
            {
                // check the 'usage' column
                // we don't check the name also (maybe we should?)
                // store in the right place in our vector
                if( sKEAField.usage == "Red" )
                    vecKEAField[0] = sKEAField;
                else if( sKEAField.usage == "Green" )
                    vecKEAField[1] = sKEAField;
                else if( sKEAField.usage == "Blue" )
                    vecKEAField[2] = sKEAField;
                else if( sKEAField.usage == "Alpha" )
                    vecKEAField[3] = sKEAField;
            }
        }

        // create any missing fields
        if( vecKEAField[0].usage.empty() )
        {
            pKEATable->addAttIntField("Red", 0, "Red");
            vecKEAField[0] = pKEATable->getField("Red");
        }
        if( vecKEAField[1].usage.empty() )
        {
            pKEATable->addAttIntField("Green", 0, "Green");
            vecKEAField[1] = pKEATable->getField("Green");
        }
        if( vecKEAField[2].usage.empty() )
        {
            pKEATable->addAttIntField("Blue", 0, "Blue");
            vecKEAField[2] = pKEATable->getField("Blue");
        }
        if( vecKEAField[3].usage.empty() )
        {
            pKEATable->addAttIntField("Alpha", 0, "Alpha");
            vecKEAField[3] = pKEATable->getField("Alpha");
        }

        // go through each row to be added
        for( int nRowIndex = 0; nRowIndex < poCT->GetColorEntryCount(); nRowIndex++ )
        {
            // get the feature - don't need to set this back since it is a pointer to 
            // internal datastruct
            kealib::KEAATTFeature *pKEAFeature = pKEATable->getFeature(nRowIndex);

            // get the GDAL entry - as RGB to be sure
            GDALColorEntry colorEntry;
            if( poCT->GetColorEntryAsRGB(nRowIndex, &colorEntry) )
            {
                // set the value
                pKEAFeature->intFields->at(vecKEAField[0].idx) = colorEntry.c1;
                pKEAFeature->intFields->at(vecKEAField[1].idx) = colorEntry.c2;
                pKEAFeature->intFields->at(vecKEAField[2].idx) = colorEntry.c3;
                pKEAFeature->intFields->at(vecKEAField[3].idx) = colorEntry.c4;
            }
        }

        this->m_pImageIO->setAttributeTable(pKEATable, this->nBand);
        delete pKEATable;

        // replace our color table with the one passed in
        // unlike attributes there are no extra fields present in the file etc
        // so should be safe to do this
        delete this->m_pColorTable;
        this->m_pColorTable = poCT->Clone();
    }
    catch(const kealib::KEAException &e)
    {
        CPLError( CE_Failure, CPLE_AppDefined, "Failed to write color table: %s", e.what() );
        return CE_Failure;
    }
    return CE_None;
}
#endif // HAVE_RFC40

GDALColorInterp KEARasterBand::GetColorInterpretation()
{
    kealib::KEABandClrInterp ekeainterp;
    try
    {
        ekeainterp = this->m_pImageIO->getImageBandClrInterp(this->nBand);
    }
    catch(const kealib::KEAException &e)
    {
        return GCI_GrayIndex;
    }
        
    GDALColorInterp egdalinterp;
    switch(ekeainterp)
    {
        case kealib::kea_generic:
        case kealib::kea_greyindex:
            egdalinterp = GCI_GrayIndex;
            break;
        case kealib::kea_paletteindex:
            egdalinterp = GCI_PaletteIndex;
            break;
        case kealib::kea_redband:
            egdalinterp = GCI_RedBand;
            break;
        case kealib::kea_greenband:
            egdalinterp = GCI_GreenBand;
            break;
        case kealib::kea_blueband:
            egdalinterp = GCI_BlueBand;
            break;
        case kealib::kea_alphaband:
            egdalinterp = GCI_AlphaBand;
            break;
        case kealib::kea_hueband:
            egdalinterp = GCI_HueBand;
            break;
        case kealib::kea_saturationband:
            egdalinterp = GCI_SaturationBand;
            break;
        case kealib::kea_lightnessband:
            egdalinterp = GCI_LightnessBand;
            break;
        case kealib::kea_cyanband:
            egdalinterp = GCI_CyanBand;
            break;
        case kealib::kea_magentaband:
            egdalinterp = GCI_MagentaBand;
            break;
        case kealib::kea_yellowband:
            egdalinterp = GCI_YellowBand;
            break;
        case kealib::kea_blackband:
            egdalinterp = GCI_BlackBand;
            break;
        case kealib::kea_ycbcr_yband:
            egdalinterp = GCI_YCbCr_YBand;
            break;
        case kealib::kea_ycbcr_cbband:
            egdalinterp = GCI_YCbCr_CbBand;
            break;
        case kealib::kea_ycbcr_crband:
            egdalinterp = GCI_YCbCr_CrBand;
            break;
        default:
            egdalinterp = GCI_GrayIndex;
            break;
    }
        
    return egdalinterp;
}

CPLErr KEARasterBand::SetColorInterpretation(GDALColorInterp egdalinterp)
{
    kealib::KEABandClrInterp ekeainterp;
    switch(egdalinterp)
    {
        case GCI_GrayIndex:
            ekeainterp = kealib::kea_greyindex;
            break;
        case GCI_PaletteIndex:
            ekeainterp = kealib::kea_paletteindex;
            break;
        case GCI_RedBand:
            ekeainterp = kealib::kea_redband;
            break;
        case GCI_GreenBand:
            ekeainterp = kealib::kea_greenband;
            break;
        case GCI_BlueBand:
            ekeainterp = kealib::kea_blueband;
            break;
        case GCI_AlphaBand:
            ekeainterp = kealib::kea_alphaband;
            break;
        case GCI_HueBand:
            ekeainterp = kealib::kea_hueband;
            break;
        case GCI_SaturationBand:
            ekeainterp = kealib::kea_saturationband;
            break;
        case GCI_LightnessBand:
            ekeainterp = kealib::kea_lightnessband;
            break;
        case GCI_CyanBand:
            ekeainterp = kealib::kea_cyanband;
            break;
        case GCI_MagentaBand:
            ekeainterp = kealib::kea_magentaband;
            break;
        case GCI_YellowBand:
            ekeainterp = kealib::kea_yellowband;
            break;
        case GCI_BlackBand:
            ekeainterp = kealib::kea_blackband;
            break;
        case GCI_YCbCr_YBand:
            ekeainterp = kealib::kea_ycbcr_yband;
            break;
        case GCI_YCbCr_CbBand:
            ekeainterp = kealib::kea_ycbcr_cbband;
            break;
        case GCI_YCbCr_CrBand:
            ekeainterp = kealib::kea_ycbcr_crband;
            break;
        default:
            ekeainterp = kealib::kea_greyindex;
            break;
    }

    try
    {
        this->m_pImageIO->setImageBandClrInterp(this->nBand, ekeainterp);
    }
    catch(const kealib::KEAException &e)
    {
        // do nothing? The docs say CE_Failure only if unsupporte by format
    }
    return CE_None;
}

// clean up our overview objects
// assumes mutex being held by caller
void KEARasterBand::deleteOverviewObjects()
{
    // deletes the objects - not the overviews themselves
    int nCount;
    for( nCount = 0; nCount < m_nOverviews; nCount++ )
    {
        delete m_panOverviewBands[nCount];
    }
    CPLFree(m_panOverviewBands);
    m_panOverviewBands = NULL;
    m_nOverviews = 0;
}

// read in any overviews in the file into our array of objects
void KEARasterBand::readExistingOverviews()
{
    CPLMutexHolderD( &m_hMutex );
    // delete any existing overview bands
    this->deleteOverviewObjects();

    m_nOverviews = this->m_pImageIO->getNumOfOverviews(this->nBand);
    m_panOverviewBands = (KEAOverview**)CPLMalloc(sizeof(KEAOverview*) * m_nOverviews);

    uint64_t nXSize, nYSize;    
    for( int nCount = 0; nCount < m_nOverviews; nCount++ )
    {
        this->m_pImageIO->getOverviewSize(this->nBand, nCount + 1, &nXSize, &nYSize);
        m_panOverviewBands[nCount] = new KEAOverview((KEADataset*)this->poDS, this->nBand, GA_ReadOnly,
                                        this->m_pImageIO, this->m_pRefCount, nCount + 1, nXSize, nYSize);
    }
}

// number of overviews
int KEARasterBand::GetOverviewCount()
{
    return m_nOverviews;
}

// get a given overview
GDALRasterBand* KEARasterBand::GetOverview(int nOverview)
{
    if( nOverview >= m_nOverviews )
    {
        return NULL;
    }
    else
    {
        return m_panOverviewBands[nOverview];
    }
}

CPLErr KEARasterBand::CreateMaskBand(int nFlags)
{
    CPLMutexHolderD( &m_hMutex );
    if( m_bMaskBandOwned )
        delete m_pMaskBand;
    m_pMaskBand = NULL;
    try
    {
        this->m_pImageIO->createMask(this->nBand);
    }
    catch(const kealib::KEAException &e)
    {
        CPLError( CE_Failure, CPLE_AppDefined, "Failed to create mask band: %s", e.what());
        return CE_Failure;
    }
    return CE_None;
}

GDALRasterBand* KEARasterBand::GetMaskBand()
{
    CPLMutexHolderD( &m_hMutex );
    if( m_pMaskBand == NULL )
    {
        try
        {
            if( this->m_pImageIO->maskCreated(this->nBand) )
            {
                m_pMaskBand = new KEAMaskBand(this, this->m_pImageIO, this->m_pRefCount);
                m_bMaskBandOwned = true;
            }
            else
            {
                // use the base class implementation - GDAL will delete
                //fprintf( stderr, "returning base GetMaskBand()\n" );
#ifdef HAVE_RFC40                
                m_pMaskBand = GDALPamRasterBand::GetMaskBand();
#else
                m_pMaskBand = NULL;
#endif
            }
        }
        catch(const kealib::KEAException &e)
        {
            // do nothing?
        }
    }
    return m_pMaskBand;
}

int KEARasterBand::GetMaskFlags()
{
    try
    {
        if( ! this->m_pImageIO->maskCreated(this->nBand) )
        {
            // need to return the base class one since we are using
            // the base class implementation of GetMaskBand()
            //fprintf( stderr, "returning base GetMaskFlags()\n" );
#ifdef HAVE_RFC40            
            return GDALPamRasterBand::GetMaskFlags();
#else
            return 0;
#endif
        }
    }
    catch(const kealib::KEAException &e)
    {
        // do nothing?
    }

    // none of the other flags seem to make sense...
    return 0;
}

kealib::KEALayerType KEARasterBand::getLayerType() const
{
    return m_pImageIO->getImageBandLayerType(nBand);
}
void KEARasterBand::setLayerType(kealib::KEALayerType eLayerType) 
{
    m_pImageIO->setImageBandLayerType(nBand, eLayerType);
}
