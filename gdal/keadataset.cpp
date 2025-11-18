/*
 *  keadataset.cpp
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
#include "keadataset.h"
#include "keacopy.h"
#include "hdf5vfl.h"

#include "libkea/KEACommon.h"
#include "cpl_vsi_virtual.h"

// Function for converting a libkea type into a GDAL type
GDALDataType KEA_to_GDAL_Type( kealib::KEADataType ekeaType )
{
    GDALDataType egdalType = GDT_Unknown;
    switch( ekeaType )
    {
        case kealib::kea_8int:
        case kealib::kea_8uint:
            egdalType = GDT_Byte;
            break;
        case kealib::kea_16int:
            egdalType = GDT_Int16;
            break;
        case kealib::kea_32int:
            egdalType = GDT_Int32;
            break;
#ifdef HAVE_64BITIMAGES
        case kealib::kea_64int:
            egdalType = GDT_Int64;
            break;
#endif
        case kealib::kea_16uint:
            egdalType = GDT_UInt16;
            break;
        case kealib::kea_32uint:
            egdalType = GDT_UInt32;
            break;
#ifdef HAVE_64BITIMAGES
        case kealib::kea_64uint:
            egdalType = GDT_UInt64;
            break;
#endif
        case kealib::kea_32float:
            egdalType = GDT_Float32;
            break;
        case kealib::kea_64float:
            egdalType = GDT_Float64;
            break;
        default:
            egdalType = GDT_Unknown;
            break;
    }
    return egdalType;
}

// function for converting a GDAL type to a kealib type
kealib::KEADataType GDAL_to_KEA_Type( GDALDataType egdalType )
{
    kealib::KEADataType ekeaType = kealib::kea_undefined;
    switch( egdalType )
    {
        case GDT_Byte:
            ekeaType = kealib::kea_8uint;
            break;
        case GDT_Int16:
            ekeaType = kealib::kea_16int;
            break;
        case GDT_Int32:
            ekeaType = kealib::kea_32int;
            break;
#ifdef HAVE_64BITIMAGES
        case GDT_Int64:
            ekeaType = kealib::kea_64int;
            break;
#endif
        case GDT_UInt16:
            ekeaType = kealib::kea_16uint;
            break;
        case GDT_UInt32:
            ekeaType = kealib::kea_32uint;
            break;
#ifdef HAVE_64BITIMAGES
        case GDT_UInt64:
            ekeaType = kealib::kea_64uint;
            break;
#endif
        case GDT_Float32:
            ekeaType = kealib::kea_32float;
            break;
        case GDT_Float64:
            ekeaType = kealib::kea_64float;
            break;
        default:
            ekeaType = kealib::kea_undefined;
            break;
    }
    return ekeaType;
}

// static function - pointer set in driver 
GDALDataset *KEADataset::Open( GDALOpenInfo * poOpenInfo )
{
    if( Identify(poOpenInfo) )
    {
        try
        {
            // try and open it in the appropriate mode
            H5::H5File *pH5File;
            if( poOpenInfo->eAccess == GA_ReadOnly )
            {
                // use the virtual driver so we can open files using
                // /vsicurl etc
                // do this same as libkea
                H5::FileAccPropList keaAccessPlist =
                    H5::FileAccPropList(H5::FileAccPropList::DEFAULT);
                keaAccessPlist.setCache(
                    kealib::KEA_MDC_NELMTS, kealib::KEA_RDCC_NELMTS,
                    kealib::KEA_RDCC_NBYTES, kealib::KEA_RDCC_W0);
                keaAccessPlist.setSieveBufSize(kealib::KEA_SIEVE_BUF);
                hsize_t blockSize = kealib::KEA_META_BLOCKSIZE;
                keaAccessPlist.setMetaBlockSize(blockSize);
                // but set the driver
                keaAccessPlist.setDriver(HDF5VFLGetFileDriver(), nullptr);

                const H5std_string keaImgFilePath(poOpenInfo->pszFilename);
                pH5File = new H5::H5File(keaImgFilePath, H5F_ACC_RDONLY,
                                         H5::FileCreatPropList::DEFAULT,
                                         keaAccessPlist);
            }
            else
            {
                pH5File = kealib::KEAImageIO::openKeaH5RW( poOpenInfo->pszFilename );
            }
            // create the KEADataset object
            KEADataset *pDataset = new KEADataset( pH5File, poOpenInfo->eAccess );

            // set the description as the name
            pDataset->SetDescription( poOpenInfo->pszFilename );

            return pDataset;
        }
        catch (const kealib::KEAIOException &e)
        {
            // was a problem - can't be a valid file
            CPLError( CE_Failure, CPLE_OpenFailed,
                  "Attempt to open file `%s' failed. Error: %s\n",
                  poOpenInfo->pszFilename, e.what() );
            return nullptr;
        }
        catch (...)
        {
            // was a problem - can't be a valid file
            CPLError(CE_Failure, CPLE_OpenFailed,
                     "Attempt to open file `%s' failed. Error: unknown\n",
                     poOpenInfo->pszFilename);
            return nullptr;
        }
    }
    else
    {
        // not a KEA file
        return nullptr;
    }
}

// static function- pointer set in driver
// this function is called in preference to Open
// can't just try and open it as could be vsi
// 
int KEADataset::Identify( GDALOpenInfo * poOpenInfo )
{
    /* -------------------------------------------------------------------- */
    /*      Is it an HDF5 file?                                             */
    /* -------------------------------------------------------------------- */
    static const char achSignature[] = "\211HDF\r\n\032\n";

    if (poOpenInfo->pabyHeader == nullptr ||
        memcmp(poOpenInfo->pabyHeader, achSignature, 8) != 0)
    {
        return 0;
    }

    return EQUAL(CPLGetExtension(poOpenInfo->pszFilename), "KEA");
}

// static function- pointer set in driver
GDALDataset *KEADataset::Create( const char * pszFilename,
                                  int nXSize, int nYSize, int nBands,
                                  GDALDataType eType,
                                  char ** papszParmList  )
{
    GDALDriverH hDriver = GDALGetDriverByName( "KEA" );
    if( ( hDriver == nullptr ) || !GDALValidateCreationOptions( hDriver, papszParmList ) )
    {
        CPLError( CE_Failure, CPLE_OpenFailed,
                  "Attempt to create file `%s' failed. Invalid creation option(s)\n", pszFilename);
        return nullptr;
    }
    
    // This helps avoiding issues with H5File handles in a bad state, that
    // may cause crashes at process termination
    // Cf https://github.com/OSGeo/gdal/issues/8743
    if (VSIFileManager::GetHandler(pszFilename) !=
        VSIFileManager::GetHandler(""))
    {
        CPLError(CE_Failure, CPLE_OpenFailed,
                 "Attempt to create file `%s' failed. /vsi file systems not "
                 "supported\n",
                 pszFilename);
        return nullptr;
    }
        
    // process any creation options in papszParmList
    // default value
    unsigned int nimageblockSize = kealib::KEA_IMAGE_CHUNK_SIZE;
    // see if they have provided a different value
    const char *pszValue = CSLFetchNameValue( papszParmList, "IMAGEBLOCKSIZE" );
    if( pszValue != nullptr )
        nimageblockSize = atol( pszValue );

    unsigned int nattblockSize = kealib::KEA_ATT_CHUNK_SIZE;
    pszValue = CSLFetchNameValue( papszParmList, "ATTBLOCKSIZE" );
    if( pszValue != nullptr )
        nattblockSize = atol( pszValue );

    unsigned int nmdcElmts = kealib::KEA_MDC_NELMTS;
    pszValue = CSLFetchNameValue( papszParmList, "MDC_NELMTS" );
    if( pszValue != nullptr )
        nmdcElmts = atol( pszValue );

    hsize_t nrdccNElmts = kealib::KEA_RDCC_NELMTS;
    pszValue = CSLFetchNameValue( papszParmList, "RDCC_NELMTS" );
    if( pszValue != nullptr )
        nrdccNElmts = atol( pszValue );

    hsize_t nrdccNBytes = kealib::KEA_RDCC_NBYTES;
    pszValue = CSLFetchNameValue( papszParmList, "RDCC_NBYTES" );
    if( pszValue != nullptr )
        nrdccNBytes = atol( pszValue );

    double nrdccW0 = kealib::KEA_RDCC_W0;
    pszValue = CSLFetchNameValue( papszParmList, "RDCC_W0" );
    if( pszValue != nullptr )
        nrdccW0 = atof( pszValue );

    hsize_t nsieveBuf = kealib::KEA_SIEVE_BUF;
    pszValue = CSLFetchNameValue( papszParmList, "SIEVE_BUF" );
    if( pszValue != nullptr )
        nsieveBuf = atol( pszValue );

    hsize_t nmetaBlockSize = kealib::KEA_META_BLOCKSIZE;
    pszValue = CSLFetchNameValue( papszParmList, "META_BLOCKSIZE" );
    if( pszValue != nullptr )
        nmetaBlockSize = atol( pszValue );

    unsigned int ndeflate = kealib::KEA_DEFLATE;
    pszValue = CSLFetchNameValue( papszParmList, "DEFLATE" );
    if( pszValue != nullptr )
        ndeflate = atol( pszValue );

    bool bThematic = false;
    pszValue = CSLFetchNameValue( papszParmList, "THEMATIC" );
    if( pszValue != nullptr )
        bThematic = EQUAL(pszValue, "YES");

    try
    {
        // now create it
        H5::H5File *keaImgH5File = kealib::KEAImageIO::createKEAImage( pszFilename,
                                                    GDAL_to_KEA_Type( eType ),
                                                    nXSize, nYSize, nBands,
                                                    nullptr, nullptr, nimageblockSize, 
                                                    nattblockSize, nmdcElmts, nrdccNElmts,
                                                    nrdccNBytes, nrdccW0, nsieveBuf, 
                                                    nmetaBlockSize, ndeflate );

        // create our dataset object                            
        KEADataset *pDataset = new KEADataset( keaImgH5File, GA_Update );

        pDataset->SetDescription( pszFilename );

        // set all to thematic if asked
        if( bThematic )
        {
            for( int nCount = 0; nCount < nBands; nCount++ )
            {
                GDALRasterBand *pBand = pDataset->GetRasterBand(nCount+1);
                pBand->SetMetadataItem("LAYER_TYPE", "thematic");
            }
        }

        return pDataset;
    }
    catch (const kealib::KEAIOException &e)
    {
        CPLError( CE_Failure, CPLE_OpenFailed,
                  "Attempt to create file `%s' failed. Error: %s\n",
                  pszFilename, e.what() );
        return nullptr;
    }
    catch (...)
    {
        CPLError(CE_Failure, CPLE_OpenFailed,
                 "Attempt to create file `%s' failed. Error: unknown\n",
                 pszFilename);
        return nullptr;
    }
}

GDALDataset *KEADataset::CreateCopy( const char * pszFilename, GDALDataset *pSrcDs,
                                int bStrict, char **  papszParmList, 
                                GDALProgressFunc pfnProgress, void *pProgressData )
{
    GDALDriverH hDriver = GDALGetDriverByName( "KEA" );
    if( ( hDriver == nullptr ) || !GDALValidateCreationOptions( hDriver, papszParmList ) )
    {
        CPLError( CE_Failure, CPLE_OpenFailed,
                  "Attempt to create file `%s' failed. Invalid creation option(s)\n", pszFilename);
        return nullptr;
    }
    // process any creation options in papszParmList
    // default value
    unsigned int nimageblockSize = kealib::KEA_IMAGE_CHUNK_SIZE;
    // see if they have provided a different value
    const char *pszValue = CSLFetchNameValue( papszParmList, "IMAGEBLOCKSIZE" );
    if( pszValue != nullptr )
        nimageblockSize = atol( pszValue );

    unsigned int nattblockSize = kealib::KEA_ATT_CHUNK_SIZE;
    pszValue = CSLFetchNameValue( papszParmList, "ATTBLOCKSIZE" );
    if( pszValue != nullptr )
        nattblockSize = atol( pszValue );

    unsigned int nmdcElmts = kealib::KEA_MDC_NELMTS;
    pszValue = CSLFetchNameValue( papszParmList, "MDC_NELMTS" );
    if( pszValue != nullptr )
        nmdcElmts = atol( pszValue );

    hsize_t nrdccNElmts = kealib::KEA_RDCC_NELMTS;
    pszValue = CSLFetchNameValue( papszParmList, "RDCC_NELMTS" );
    if( pszValue != nullptr )
        nrdccNElmts = atol( pszValue );

    hsize_t nrdccNBytes = kealib::KEA_RDCC_NBYTES;
    pszValue = CSLFetchNameValue( papszParmList, "RDCC_NBYTES" );
    if( pszValue != nullptr )
        nrdccNBytes = atol( pszValue );

    double nrdccW0 = kealib::KEA_RDCC_W0;
    pszValue = CSLFetchNameValue( papszParmList, "RDCC_W0" );
    if( pszValue != nullptr )
        nrdccW0 = atof( pszValue );

    hsize_t nsieveBuf = kealib::KEA_SIEVE_BUF;
    pszValue = CSLFetchNameValue( papszParmList, "SIEVE_BUF" );
    if( pszValue != nullptr )
        nsieveBuf = atol( pszValue );

    hsize_t nmetaBlockSize = kealib::KEA_META_BLOCKSIZE;
    pszValue = CSLFetchNameValue( papszParmList, "META_BLOCKSIZE" );
    if( pszValue != nullptr )
        nmetaBlockSize = atol( pszValue );

    unsigned int ndeflate = kealib::KEA_DEFLATE;
    pszValue = CSLFetchNameValue( papszParmList, "DEFLATE" );
    if( pszValue != nullptr )
        ndeflate = atol( pszValue );

    bool bThematic = false;
    pszValue = CSLFetchNameValue( papszParmList, "THEMATIC" );
    if( pszValue != nullptr )
        bThematic = EQUAL(pszValue, "YES");

    // get the data out of the input dataset
    int nXSize = pSrcDs->GetRasterXSize();
    int nYSize = pSrcDs->GetRasterYSize();
    int nBands = pSrcDs->GetRasterCount();
    GDALDataType eType = pSrcDs->GetRasterBand(1)->GetRasterDataType();

    try
    {
        // now create it
        H5::H5File *keaImgH5File = kealib::KEAImageIO::createKEAImage( pszFilename,
                                                    GDAL_to_KEA_Type( eType ),
                                                    nXSize, nYSize, nBands,
                                                    nullptr, nullptr, nimageblockSize, 
                                                    nattblockSize, nmdcElmts, nrdccNElmts,
                                                    nrdccNBytes, nrdccW0, nsieveBuf, 
                                                    nmetaBlockSize, ndeflate );

        // create the imageio
        kealib::KEAImageIO *pImageIO = new kealib::KEAImageIO();
        
        // open the file
        pImageIO->openKEAImageHeader( keaImgH5File );

        // copy file
        if( !CopyFile( pSrcDs, pImageIO, pfnProgress, pProgressData) )
        {
            delete pImageIO;
            return nullptr;
        }

        // close it
        try
        {
            pImageIO->close();
        }
        catch (const kealib::KEAIOException &e)
        {
        }
        delete pImageIO;

        // now open it again - because the constructor loads all the info
        // in we need to copy the data first....
        keaImgH5File = kealib::KEAImageIO::openKeaH5RW( pszFilename );

        // and wrap it in a dataset
        KEADataset *pDataset = new KEADataset( keaImgH5File, GA_Update );
        pDataset->SetDescription( pszFilename );

        // set all to thematic if asked - overrides whatever set by CopyFile
        if( bThematic )
        {
            for( int nCount = 0; nCount < nBands; nCount++ )
            {
                GDALRasterBand *pBand = pDataset->GetRasterBand(nCount+1);
                pBand->SetMetadataItem("LAYER_TYPE", "thematic");
            }
        }

        return pDataset;
    }
    catch (const kealib::KEAException &e)
    {
        CPLError( CE_Failure, CPLE_OpenFailed,
                  "Attempt to create file `%s' failed. Error: %s\n",
                  pszFilename, e.what() );
        return nullptr;
    }

}

// constructor
KEADataset::KEADataset( H5::H5File *keaImgH5File, GDALAccess eAccess )
{
    this->m_hMutex = CPLCreateMutex();
    CPLReleaseMutex( this->m_hMutex );
    try
    {
        // create the image IO and initilize the refcount
        m_pImageIO = new kealib::KEAImageIO();
        m_pRefcount = new LockedRefCount();

        // open the file
        m_pImageIO->openKEAImageHeader( keaImgH5File );
        kealib::KEAImageSpatialInfo *pSpatialInfo = m_pImageIO->getSpatialInfo();

        // get the dimensions
        this->nBands = m_pImageIO->getNumOfImageBands();
        this->nRasterXSize = pSpatialInfo->xSize;
        this->nRasterYSize = pSpatialInfo->ySize;
        this->eAccess = eAccess;

        // create all the bands
        for( int nCount = 0; nCount < nBands; nCount++ )
        {
            // note GDAL uses indices starting at 1 and so does kealib
            // create band object
            KEARasterBand *pBand = new KEARasterBand( this, nCount + 1, eAccess, m_pImageIO, m_pRefcount );
            // read in overviews
            pBand->readExistingOverviews();
            // set the band into this dataset
            this->SetBand( nCount + 1, pBand );            
        }

        // read in the metadata
        m_papszMetadataList = nullptr;
        this->UpdateMetadataList();

        // nullptr until we read them in 
        m_pGCPs = nullptr;
    }
    catch (const kealib::KEAIOException &e)
    {
        // ignore?
        CPLError( CE_Warning, CPLE_AppDefined,
                "Caught exception in KEADataset constructor %s", e.what() );
    }
}

KEADataset::~KEADataset()
{
    {
        CPLMutexHolderD( &m_hMutex );
        // destroy the metadata
        CSLDestroy(m_papszMetadataList);
        this->DestroyGCPs();
    }
    // decrement the refcount and delete if needed
    if( m_pRefcount->DecRef() )
    {
        try
        {
            m_pImageIO->close();
        }
        catch (const kealib::KEAIOException &e)
        {
        }
        delete m_pImageIO;
        delete m_pRefcount;
    }

    CPLDestroyMutex( m_hMutex );
    m_hMutex = nullptr;
}

// read in the metadata into our CSLStringList
void KEADataset::UpdateMetadataList()
{
    CPLMutexHolderD( &m_hMutex );
    std::vector< std::pair<std::string, std::string> > odata;
    // get all the metadata
    odata = this->m_pImageIO->getImageMetaData();
    for(auto iterMetaData = odata.begin(); iterMetaData != odata.end(); ++iterMetaData)
    {
        m_papszMetadataList = CSLSetNameValue(m_papszMetadataList, iterMetaData->first.c_str(), iterMetaData->second.c_str());
    }
}

// read in the geotransform
#ifdef HAVE_SETVALUE_CPLERR
CPLErr KEADataset::GetGeoTransform(GDALGeoTransform &gt) const
{
    try
    {
        kealib::KEAImageSpatialInfo *pSpatialInfo =
            m_pImageIO->getSpatialInfo();
        // GDAL uses an array format
        gt[0] = pSpatialInfo->tlX;
        gt[1] = pSpatialInfo->xRes;
        gt[2] = pSpatialInfo->xRot;
        gt[3] = pSpatialInfo->tlY;
        gt[4] = pSpatialInfo->yRot;
        gt[5] = pSpatialInfo->yRes;

        return CE_None;
    }
    catch (const kealib::KEAIOException &e)
    {
        CPLError(CE_Warning, CPLE_AppDefined, "Unable to read geotransform: %s",
                 e.what());
        return CE_Failure;
    }
}

// set the geotransform
CPLErr KEADataset::SetGeoTransform(const GDALGeoTransform &gt)
{
    try
    {
        // get the spatial info and update it
        kealib::KEAImageSpatialInfo *pSpatialInfo =
            m_pImageIO->getSpatialInfo();
        // convert back from GDAL's array format
        pSpatialInfo->tlX = gt[0];
        pSpatialInfo->xRes = gt[1];
        pSpatialInfo->xRot = gt[2];
        pSpatialInfo->tlY = gt[3];
        pSpatialInfo->yRot = gt[4];
        pSpatialInfo->yRes = gt[5];

        m_pImageIO->setSpatialInfo(pSpatialInfo);
        return CE_None;
    }
    catch (const kealib::KEAIOException &e)
    {
        CPLError(CE_Warning, CPLE_AppDefined,
                 "Unable to write geotransform: %s", e.what());
        return CE_Failure;
    }
}

#else

CPLErr KEADataset::GetGeoTransform( double * padfTransform )
{
    try
    {
        kealib::KEAImageSpatialInfo *pSpatialInfo = m_pImageIO->getSpatialInfo();
        // GDAL uses an array format
        padfTransform[0] = pSpatialInfo->tlX;
        padfTransform[1] = pSpatialInfo->xRes;
        padfTransform[2] = pSpatialInfo->xRot;
        padfTransform[3] = pSpatialInfo->tlY;
        padfTransform[4] = pSpatialInfo->yRot;
        padfTransform[5] = pSpatialInfo->yRes;
    
        return CE_None;
    }
    catch (const kealib::KEAIOException &e)
    {
        CPLError( CE_Warning, CPLE_AppDefined,
                "Unable to read geotransform: %s", e.what() );
        return CE_Failure;
    }
}

// set the geotransform
CPLErr KEADataset::SetGeoTransform (double *padfTransform )
{
    try
    {
        // get the spatial info and update it
        kealib::KEAImageSpatialInfo *pSpatialInfo = m_pImageIO->getSpatialInfo();
        // convert back from GDAL's array format
        pSpatialInfo->tlX = padfTransform[0];
        pSpatialInfo->xRes = padfTransform[1];
        pSpatialInfo->xRot = padfTransform[2];
        pSpatialInfo->tlY = padfTransform[3];
        pSpatialInfo->yRot = padfTransform[4];
        pSpatialInfo->yRes = padfTransform[5];

        m_pImageIO->setSpatialInfo( pSpatialInfo );
        return CE_None;
    }
    catch (const kealib::KEAIOException &e)
    {
        CPLError( CE_Warning, CPLE_AppDefined,
                "Unable to write geotransform: %s", e.what() );
        return CE_Failure;
    }
}

#endif

const OGRSpatialReference* KEADataset::GetSpatialRef() const
{
    if( !m_oSRS.IsEmpty() )
        return &m_oSRS;

    try
    {
        kealib::KEAImageSpatialInfo *pSpatialInfo = m_pImageIO->getSpatialInfo();
        m_oSRS.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
        m_oSRS.importFromWkt(pSpatialInfo->wktString.c_str());
        return m_oSRS.IsEmpty() ? nullptr: &m_oSRS;
    }
    catch (const kealib::KEAIOException &e)
    {
        return nullptr;
    }
}

// set the projection
CPLErr KEADataset::SetSpatialRef(const OGRSpatialReference* poSRS)
{
    try
    {
        // get the spatial info and update it
        kealib::KEAImageSpatialInfo *pSpatialInfo = m_pImageIO->getSpatialInfo();
        m_oSRS.Clear();
        if( poSRS )
        {
            m_oSRS = *poSRS;
            char* pszWKT = nullptr;
            m_oSRS.exportToWkt(&pszWKT);
            pSpatialInfo->wktString = pszWKT ? pszWKT: "";
            CPLFree(pszWKT);
        }
        else
        {
            pSpatialInfo->wktString.clear();
        }

        m_pImageIO->setSpatialInfo( pSpatialInfo );
        return CE_None;
    }
    catch (const kealib::KEAIOException &e)
    {
        CPLError( CE_Warning, CPLE_AppDefined,
                "Unable to write projection: %s", e.what() );
        return CE_Failure;
    }
}

// Thought this might be handy to pass back to the application
void * KEADataset::GetInternalHandle(const char *)
{
    return m_pImageIO;
}

// this is called by GDALDataset::BuildOverviews. we implement this function to support
// building of overviews
#ifdef HAVE_OVERVIEWOPTIONS
CPLErr KEADataset::IBuildOverviews(const char *pszResampling, int nOverviews, const int *panOverviewList, 
                                    int nListBands, const int *panBandList, GDALProgressFunc pfnProgress, 
                                    void *pProgressData, CSLConstList papszOptions)
#else
CPLErr KEADataset::IBuildOverviews(const char *pszResampling, int nOverviews, int *panOverviewList, 
                                    int nListBands, int *panBandList, GDALProgressFunc pfnProgress, 
                                    void *pProgressData)
#endif
{
    // go through the list of bands that have been passed in
    int nCurrentBand, nOK = 1;
    for( int nBandCount = 0; (nBandCount < nListBands) && nOK; nBandCount++ )
    {
        // get the band number
        nCurrentBand = panBandList[nBandCount];
        // get the band
        KEARasterBand *pBand = (KEARasterBand*)this->GetRasterBand(nCurrentBand);
        // create the overview object
        pBand->CreateOverviews( nOverviews, panOverviewList );

        // get GDAL to do the hard work. It will calculate the overviews and write them
        // back into the objects
#ifdef HAVE_OVERVIEWOPTIONS
        if( GDALRegenerateOverviewsEx( (GDALRasterBandH)pBand, nOverviews, (GDALRasterBandH*)pBand->GetOverviewList(),
                                    pszResampling, pfnProgress, pProgressData, papszOptions) != CE_None )
#else
        if( GDALRegenerateOverviews( (GDALRasterBandH)pBand, nOverviews, (GDALRasterBandH*)pBand->GetOverviewList(),
                                    pszResampling, pfnProgress, pProgressData ) != CE_None )
#endif
        {
            nOK = 0;
        }
    }
    if( !nOK )
    {
        return CE_Failure;
    }
    else
    {
        return CE_None;
    }
}

// set a single metadata item
CPLErr KEADataset::SetMetadataItem(const char *pszName, const char *pszValue, const char *pszDomain)
{
    CPLMutexHolderD( &m_hMutex );
    // only deal with 'default' domain - no geolocation etc
    if( ( pszDomain != nullptr ) && ( *pszDomain != '\0' ) )
        return CE_Failure;

    try
    {
        this->m_pImageIO->setImageMetaData(pszName, pszValue );
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
const char *KEADataset::GetMetadataItem (const char *pszName, const char *pszDomain)
{
    CPLMutexHolderD( &m_hMutex );
    // only deal with 'default' domain - no geolocation etc
    if( ( pszDomain != nullptr ) && ( *pszDomain != '\0' ) )
        return nullptr;
    // string returned from CSLFetchNameValue should be persistant
    return CSLFetchNameValue(m_papszMetadataList, pszName);
}

// get the whole metadata as CSLStringList - note may be thread safety issues
char **KEADataset::GetMetadata(const char *pszDomain)
{ 
    // only deal with 'default' domain - no geolocation etc
    if( ( pszDomain != nullptr ) && ( *pszDomain != '\0' ) )
        return nullptr;
    // this is what we store it as anyway
    return m_papszMetadataList; 
}

// set the whole metadata as a CSLStringList
CPLErr KEADataset::SetMetadata(char **papszMetadata, const char *pszDomain)
{
    CPLMutexHolderD( &m_hMutex );
    // only deal with 'default' domain - no geolocation etc
    if( ( pszDomain != nullptr ) && ( *pszDomain != '\0' ) )
        return CE_Failure;

    int nIndex = 0;
    char *pszName;
    const char *pszValue;
    try
    {
        // go through each item
        while( papszMetadata[nIndex] != nullptr )
        {
            // get the value/name
            pszValue = CPLParseNameValue( papszMetadata[nIndex], &pszName );
            // set it with imageio
            this->m_pImageIO->setImageMetaData(pszName, pszValue );
            nIndex++;
        }
    }
    catch (const kealib::KEAIOException &e)
    {
        return CE_Failure;
    }

    // destroy our one and replace it
    CSLDestroy(m_papszMetadataList);
    m_papszMetadataList = CSLDuplicate(papszMetadata);
    return CE_None;
}

CPLErr KEADataset::AddBand(GDALDataType eType, char **papszOptions)
{
    // process any creation options in papszOptions
    unsigned int nimageBlockSize = kealib::KEA_IMAGE_CHUNK_SIZE;
    unsigned int nattBlockSize = kealib::KEA_ATT_CHUNK_SIZE;
    unsigned int ndeflate = kealib::KEA_DEFLATE;
    if (papszOptions != nullptr) {
        const char *pszValue = CSLFetchNameValue(papszOptions,"IMAGEBLOCKSIZE");
        if ( pszValue != nullptr ) {
            nimageBlockSize = atol(pszValue);
        }

        pszValue = CSLFetchNameValue(papszOptions, "ATTBLOCKSIZE");
        if (pszValue != nullptr) {
            nattBlockSize = atol(pszValue);
        }

        pszValue = CSLFetchNameValue(papszOptions, "DEFLATE");
        if (pszValue != nullptr) {
            ndeflate = atol(pszValue);
        }
    }

    try {
        m_pImageIO->addImageBand(GDAL_to_KEA_Type(eType), "", nimageBlockSize,
                nattBlockSize, ndeflate);
    } catch (const kealib::KEAIOException &e) {
        return CE_Failure;
    }

    // update the GDALDataset
    this->nBands = m_pImageIO->getNumOfImageBands();

    // create a new band and add it to the dataset
    // note GDAL uses indices starting at 1 and so does kealib
    KEARasterBand *pBand = new KEARasterBand(this, this->nBands, this->eAccess,
            m_pImageIO, m_pRefcount);
    this->SetBand(this->nBands, pBand);            

    return CE_None;
}

OGRErr KEADataset::DeleteLayer(int iLayer)
{
    try
    {
        m_pImageIO->removeImageBand(iLayer);
    }
    catch (const kealib::KEAIOException &e) 
    {
        CPLError( CE_Warning, CPLE_AppDefined,
                "Unable to delete a layer: %s", e.what() );
        return OGRERR_FAILURE;
    }
    return OGRERR_NONE;
}

int KEADataset::GetGCPCount()
{
    try
    {
        return m_pImageIO->getGCPCount();
    }
    catch (const kealib::KEAIOException &e) 
    {
        return 0;
    }

}

const OGRSpatialReference* KEADataset::GetGCPSpatialRef() const
{
    CPLMutexHolderD( &m_hMutex );
    if( m_oGCPSRS.IsEmpty() )
    {
        try
        {
            std::string sProj = m_pImageIO->getGCPProjection();
            m_oGCPSRS.SetAxisMappingStrategy(OAMS_TRADITIONAL_GIS_ORDER);
            m_oGCPSRS.Clear();
            if( !sProj.empty() )
                m_oGCPSRS.importFromWkt( sProj.c_str() );
        }
        catch (const kealib::KEAIOException &e) 
        {
            return nullptr;
        }
    }
    return m_oGCPSRS.IsEmpty() ? nullptr : &m_oGCPSRS;
}

const GDAL_GCP* KEADataset::GetGCPs()
{
    CPLMutexHolderD( &m_hMutex );
    if( m_pGCPs == nullptr )
    {
        // convert to GDAL data structures
        try
        {
            unsigned int nCount = m_pImageIO->getGCPCount();
            std::vector<kealib::KEAImageGCP*> *pKEAGCPs = m_pImageIO->getGCPs();

            m_pGCPs = (GDAL_GCP*)calloc(nCount, sizeof(GDAL_GCP));
            for( unsigned int nIndex = 0; nIndex < nCount; nIndex++)
            {
                GDAL_GCP *pGCP = &m_pGCPs[nIndex];
                kealib::KEAImageGCP *pKEAGCP = pKEAGCPs->at(nIndex);
                pGCP->pszId = strdup( pKEAGCP->pszId.c_str() );
                pGCP->pszInfo = strdup( pKEAGCP->pszInfo.c_str() );
                pGCP->dfGCPPixel = pKEAGCP->dfGCPPixel;
                pGCP->dfGCPLine = pKEAGCP->dfGCPLine;
                pGCP->dfGCPX = pKEAGCP->dfGCPX;
                pGCP->dfGCPY = pKEAGCP->dfGCPY;
                pGCP->dfGCPZ = pKEAGCP->dfGCPZ;
            }
        }
        catch (const kealib::KEAIOException &e) 
        {
            return nullptr;
        }
    }
    return m_pGCPs;
}

CPLErr KEADataset::SetGCPs(int nGCPCount, const GDAL_GCP *pasGCPList, const OGRSpatialReference* poSRS)
{
    CPLMutexHolderD( &m_hMutex );
    this->DestroyGCPs();
    m_oGCPSRS.Clear();
    CPLErr result = CE_None;

    std::vector<kealib::KEAImageGCP*> *pKEAGCPs = new std::vector<kealib::KEAImageGCP*>(nGCPCount);
    for( int nIndex = 0; nIndex < nGCPCount; nIndex++ )
    {
        const GDAL_GCP *pGCP = &pasGCPList[nIndex];
        kealib::KEAImageGCP *pKEA = new kealib::KEAImageGCP;
        pKEA->pszId = pGCP->pszId;
        pKEA->pszInfo = pGCP->pszInfo;
        pKEA->dfGCPPixel = pGCP->dfGCPPixel;
        pKEA->dfGCPLine = pGCP->dfGCPLine;
        pKEA->dfGCPX = pGCP->dfGCPX;
        pKEA->dfGCPY = pGCP->dfGCPY;
        pKEA->dfGCPZ = pGCP->dfGCPZ;
        pKEAGCPs->at(nIndex) = pKEA;
    }
    try
    {
        char* pszGCPProjection = nullptr;
        if( poSRS )
        {
            m_oGCPSRS = *poSRS;
            poSRS->exportToWkt(&pszGCPProjection);
            m_pImageIO->setGCPs(pKEAGCPs, pszGCPProjection ? pszGCPProjection : "");
            CPLFree(pszGCPProjection);
        }
        else
        {
            m_pImageIO->setGCPs(pKEAGCPs, "");
        }
    }
    catch (const kealib::KEAIOException &e) 
    {
        CPLError( CE_Warning, CPLE_AppDefined,
                "Unable to write GCPs: %s", e.what() );
        result = CE_Failure;
    }

    for( auto itr = pKEAGCPs->begin(); itr != pKEAGCPs->end(); itr++)
    {
        kealib::KEAImageGCP *pKEA = (*itr);
        delete pKEA;
    }
    delete pKEAGCPs;

    return result;
}

void KEADataset::DestroyGCPs()
{
    CPLMutexHolderD( &m_hMutex );
    if( m_pGCPs != nullptr )
    {
        // we assume this is always the same as the internal list...
        int nCount = this->GetGCPCount();
        for( int nIndex = 0; nIndex < nCount; nIndex++ )
        {
            GDAL_GCP *pGCP = &m_pGCPs[nIndex];
            free( pGCP->pszId );
            free( pGCP->pszInfo );
        }
        free( m_pGCPs );
        m_pGCPs = nullptr;
    }
}
