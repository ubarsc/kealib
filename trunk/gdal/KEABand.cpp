
#include "KEABand.h"
#include "KEAOverview.h"

#include "gdal_rat.h"
#include "libkea/KEAAttributeTable.h"

// constructor
KEARasterBand::KEARasterBand( KEADataset *pDataset, int nSrcBand, libkea::KEAImageIO *pImageIO, int *pRefCount )
{
    this->poDS = pDataset; // our pointer onto the dataset
    this->nBand = nSrcBand; // this is the band we are
    this->m_eKEADataType = pImageIO->getImageBandDataType(nSrcBand); // get the data type as KEA enum
    this->eDataType = KEA_to_GDAL_Type( m_eKEADataType );       // convert to GDAL enum
    this->nBlockXSize = pImageIO->getImageBlockSize(nSrcBand);  // get the native blocksize
    this->nBlockYSize = pImageIO->getImageBlockSize(nSrcBand);
    this->nRasterXSize = this->poDS->GetRasterXSize();          // ask the dataset for the total image size
    this->nRasterYSize = this->poDS->GetRasterYSize();

    // grab the imageio class and its refcount
    this->m_pImageIO = pImageIO;
    this->m_pnRefCount = pRefCount;
    // increment the refcount as we now have a reference to imageio
    (*this->m_pnRefCount)++;

    // initialis overview variables
    m_nOverviews = 0;
    m_panOverviewBands = NULL;

    // grab the description here
    this->sDescription = pImageIO->getImageBandDescription(nSrcBand);

    this->m_pAttributeTable = NULL;  // no RAT yet

    // initialise the metadata as a CPLStringList
    m_papszMetadataList = NULL;
    this->UpdateMetadataList();
}

// destructor
KEARasterBand::~KEARasterBand()
{
    // destroy RAT if any
    delete this->m_pAttributeTable;
    // destroy the metadata
    CSLDestroy(this->m_papszMetadataList);
    // delete any overview bands
    this->deleteOverviewObjects();

    // according to the docs, this is required
    this->FlushCache();

    // decrement the recount and delete if needed
    (*m_pnRefCount)--;
    if( *m_pnRefCount == 0 )
    {
        m_pImageIO->close();
        delete m_pImageIO;
        delete m_pnRefCount;
    }
}

// internal method that updates the metadata into m_papszMetadataList
void KEARasterBand::UpdateMetadataList()
{
    std::vector< std::pair<std::string, std::string> > data;

    // get all the metadata and iterate through
    data = this->m_pImageIO->getImageBandMetaData(this->nBand);
    for(std::vector< std::pair<std::string, std::string> >::iterator iterMetaData = data.begin(); iterMetaData != data.end(); ++iterMetaData)
    {
        // add to our list
        m_papszMetadataList = CSLSetNameValue(m_papszMetadataList, iterMetaData->first.c_str(), iterMetaData->second.c_str());
    }
    // we have a pseudo metadata item that tells if we are thematic 
    // or continuous like the HFA driver
    if( this->m_pImageIO->getImageBandLayerType(this->nBand) == libkea::kea_continuous )
    {
        m_papszMetadataList = CSLSetNameValue(m_papszMetadataList, "LAYER_TYPE", "athematic" );
    }
    else
    {
        m_papszMetadataList = CSLSetNameValue(m_papszMetadataList, "LAYER_TYPE", "thematic" );
    }
}

// internal method to create the overviews
void KEARasterBand::CreateOverviews(int nOverviews, int *panOverviewList)
{
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
        m_panOverviewBands[nCount] = new KEAOverview((KEADataset*)this->poDS, this->nBand, 
                                        this->m_pImageIO, this->m_pnRefCount, nCount + 1, nXSize, nYSize);
    }
}

// virtual method to read a block
CPLErr KEARasterBand::IReadBlock( int nBlockXOff, int nBlockYOff, void * pImage )
{
    try
    {
        // GDAL deals in blocks - if we are at the end of a row
        // we need to adjust the amount read so we don't go over the edge
        int xsize = this->nBlockXSize;
        int xtotalsize = this->nBlockXSize * (nBlockXOff + 1);
        if( xtotalsize > this->nRasterXSize )
        {
            xsize -= (xtotalsize - this->nRasterXSize);
        }
        int ysize = this->nBlockYSize;
        int ytotalsize = this->nBlockYSize * (nBlockYOff + 1);
        if( ytotalsize > this->nRasterYSize )
        {
            ysize -= (ytotalsize - this->nRasterYSize);
        }
        this->m_pImageIO->readImageBlock2Band( this->nBand, pImage, this->nBlockXSize * nBlockXOff,
                                            this->nBlockYSize * nBlockYOff,
                                            xsize, ysize, this->nBlockXSize, this->nBlockYSize, 
                                            this->m_eKEADataType );
        return CE_None;
    }
    catch (libkea::KEAIOException &e)
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
        int xsize = this->nBlockXSize;
        int xtotalsize = this->nBlockXSize * (nBlockXOff + 1);
        if( xtotalsize > this->nRasterXSize )
        {
            xsize -= (xtotalsize - this->nRasterXSize);
        }
        int ysize = this->nBlockYSize;
        int ytotalsize = this->nBlockYSize * (nBlockYOff + 1);
        if( ytotalsize > this->nRasterYSize )
        {
            ysize -= (ytotalsize - this->nRasterYSize);
        }

        this->m_pImageIO->writeImageBlock2Band( this->nBand, pImage, this->nBlockXSize * nBlockXOff,
                                            this->nBlockYSize * nBlockYOff,
                                            xsize, ysize, this->nBlockXSize, this->nBlockYSize,
                                            this->m_eKEADataType );
        return CE_None;
    }
    catch (libkea::KEAIOException &e)
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                "Failed to write file: %s", e.what() );
        return CE_Failure;
    }
}

void KEARasterBand::SetDescription(const char *pszDescription)
{
    try
    {
        this->m_pImageIO->setImageBandDescription(this->nBand, pszDescription);
        this->sDescription = pszDescription;
    }
    catch (libkea::KEAIOException &e)
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
    // only deal with 'default' domain - no geolocation etc
    if( ( pszDomain != NULL ) && ( *pszDomain != '\0' ) )
        return CE_Failure;
    try
    {
        // if it is LAYER_TYPE handle it seperately
        if( EQUAL( pszName, "LAYER_TYPE" ) )
        {
            if( EQUAL( pszValue, "athematic" ) )
            {
                this->m_pImageIO->setImageBandLayerType(this->nBand, libkea::kea_continuous );
            }
            else
            {
                this->m_pImageIO->setImageBandLayerType(this->nBand, libkea::kea_thematic );
            }
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
    catch (libkea::KEAIOException &e)
    {
        return CE_Failure;
    }
}

// get a single metdata item
const char *KEARasterBand::GetMetadataItem (const char *pszName, const char *pszDomain)
{
    // only deal with 'default' domain - no geolocation etc
    if( ( pszDomain != NULL ) && ( *pszDomain != '\0' ) )
        return NULL;
    // get it out of the CSLStringList so we can be sure it is persistant
    return CSLFetchNameValue(m_papszMetadataList, pszName);
}

// get all the metadata as a CSLStringList
char **KEARasterBand::GetMetadata(const char *pszDomain)
{
    // only deal with 'default' domain - no geolocation etc
    if( ( pszDomain != NULL ) && ( *pszDomain != '\0' ) )
        return NULL;
    // conveniently we already have it in this format
    return m_papszMetadataList; 
}

// set the metdata as a CSLStringList
CPLErr KEARasterBand::SetMetadata(char **papszMetadata, const char *pszDomain)
{
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
                    this->m_pImageIO->setImageBandLayerType(this->nBand, libkea::kea_continuous );
                }
                else
                {
                    this->m_pImageIO->setImageBandLayerType(this->nBand, libkea::kea_thematic );
                }
            }
            else
            {
                // write it into the image
                this->m_pImageIO->setImageBandMetaData(this->nBand, pszName, pszValue );
            }
            nIndex++;
        }
    }
    catch (libkea::KEAIOException &e)
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
        this->m_pImageIO->getNoDataValue(this->nBand, &dVal, libkea::kea_64float);
        if( pbSuccess != NULL )
            *pbSuccess = 1;

        return dVal;
    }
    catch (libkea::KEAIOException &e)
    {
        if( pbSuccess != NULL )
            *pbSuccess = 0;
        return -1;
    }
}

// set the no data value
CPLErr KEARasterBand::SetNoDataValue(double dfNoData)
{
    try
    {
        this->m_pImageIO->setNoDataValue(this->nBand, &dfNoData, libkea::kea_64float);
        return CE_None;
    }
    catch (libkea::KEAIOException &e)
    {
        return CE_Failure;
    }
}

// read the attributes into a GDALAttributeTable
const GDALRasterAttributeTable *KEARasterBand::GetDefaultRAT()
{
    if( this->m_pAttributeTable == NULL )
    {
        try
        {
            if( this->m_pImageIO->attributeTablePresent(this->nBand) )
            {
                // we need to create one
                this->m_pAttributeTable = new GDALRasterAttributeTable();

                // we assume this is never NULL - creates a new one if none exists
                libkea::KEAAttributeTable *pKEATable = this->m_pImageIO->getAttributeTable(libkea::kea_att_mem, this->nBand);
    
                for( size_t nColumnIndex = 0; nColumnIndex < pKEATable->getMaxGlobalColIdx(); nColumnIndex++ )
                {
                    libkea::KEAATTField sKEAField;
                    try
                    {
                        sKEAField = pKEATable->getField(nColumnIndex);
                    }
                    catch(libkea::KEAATTException &e)
                    {
                        // pKEATable->getField raised exception because we have a missing column
                        continue;
                    }

                    GDALRATFieldType eGDALType;
                    switch( sKEAField.dataType )
                    {
                        case libkea::kea_att_bool:
                        case libkea::kea_att_int:
                            eGDALType = GFT_Integer;
                            break;
                        case libkea::kea_att_float:
                            eGDALType = GFT_Real;
                            break;
                        case libkea::kea_att_string:
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

                    int nGDALColumnIndex = this->m_pAttributeTable->GetColumnCount() - 1;
                    for( size_t nRowIndex = 0; nRowIndex < pKEATable->getSize(); nRowIndex++ )
                    {
                        if( sKEAField.dataType == libkea::kea_att_bool )
                        {
                            bool bVal = pKEATable->getBoolField(nRowIndex, nColumnIndex);
                            int nVal;
                            if( bVal )
                                nVal = 1;
                            else
                                nVal = 0;
                            this->m_pAttributeTable->SetValue(nRowIndex, nGDALColumnIndex, nVal);
                        }
                        else if( sKEAField.dataType == libkea::kea_att_int )
                        {
                            int nVal = pKEATable->getIntField(nRowIndex, nColumnIndex);
                            this->m_pAttributeTable->SetValue(nRowIndex, nGDALColumnIndex, nVal);
                        }
                        else
                        {
                            std::string sVal = pKEATable->getStringField(nRowIndex, nColumnIndex);
                            this->m_pAttributeTable->SetValue(nRowIndex, nGDALColumnIndex, sVal.c_str());
                        }
                    }
                }

                delete pKEATable;
            }
        }
        catch(libkea::KEAException &e)
        {
            CPLError( CE_Failure, CPLE_AppDefined, "Failed to read attributes: %s", e.what() );
            delete this->m_pAttributeTable;
            this->m_pAttributeTable = NULL;
        }
    }
    return this->m_pAttributeTable;
}

CPLErr KEARasterBand::SetDefaultRAT(const GDALRasterAttributeTable *poRAT)
{
    if( poRAT == NULL )
        return CE_Failure;

    try
    {
        // we assume this is never NULL - creates a new one if none exists
        libkea::KEAAttributeTable *pKEATable = this->m_pImageIO->getAttributeTable(libkea::kea_att_mem, this->nBand);

        for( int nGDALColumnIndex = 0; nGDALColumnIndex < poRAT->GetColumnCount(); nGDALColumnIndex++ )
        {
            const char *pszColumnName = poRAT->GetNameOfCol(nGDALColumnIndex);
            bool bExists = true;
            libkea::KEAATTField sKEAField;
            try
            {
                sKEAField = pKEATable->getField(pszColumnName);
                // if this works we assume same usage, type etc
            }
            catch(libkea::KEAATTException &e)
            {
                // doesn't exist on file - create it
                bExists = false;
            }

            if( ! bExists )
            {
                std::string strUsage = "Generic";
                switch(poRAT->GetUsageOfCol(nGDALColumnIndex))
                {
                    case GFU_PixelCount:
                        strUsage = "PixelCount";
                        break;
                    case GFU_Name:
                        strUsage = "Name";
                        break;
                    case GFU_Red:
                        strUsage = "Red";
                        break;
                    case GFU_Green:
                        strUsage = "Green";
                        break;
                    case GFU_Blue:
                        strUsage = "Blue";
                        break;
                    case GFU_Alpha:
                        strUsage = "Alpha";
                        break;
                    default:
                        // leave as "Generic"
                        break;
                }

                if(poRAT->GetTypeOfCol(nGDALColumnIndex) == GFT_Integer)
                {
                    pKEATable->addAttIntField(pszColumnName, 0, strUsage);
                }
                else if(poRAT->GetTypeOfCol(nGDALColumnIndex) == GFT_Real)
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

            // at this stage sKEAField should represent field added or found
            for( int nRowCount = 0; nRowCount < poRAT->GetRowCount(); nRowCount++ )
            {
                if(poRAT->GetTypeOfCol(nGDALColumnIndex) == GFT_Integer)
                {
                    int nVal = poRAT->GetValueAsInt(nRowCount, nGDALColumnIndex);
                    pKEATable->setIntField(nRowCount, sKEAField.idx, nVal);
                }
                else if(poRAT->GetTypeOfCol(nGDALColumnIndex) == GFT_Real)
                {
                    double dVal = poRAT->GetValueAsDouble(nRowCount, nGDALColumnIndex);
                    pKEATable->setFloatField(nRowCount, sKEAField.idx, dVal);
                }
                else
                {
                    const char *pszValue = poRAT->GetValueAsString(nRowCount, nGDALColumnIndex);
                    pKEATable->setStringField(nRowCount, sKEAField.idx, pszValue);
                }
                // no support for bools sorry
            }
        }

        delete pKEATable;

        // our cached attribute table object is now ouf of date
        // delete it and next call to GetDefaultRAT() will re-read it
        delete this->m_pAttributeTable;
        this->m_pAttributeTable = NULL;
    }
    catch(libkea::KEAException &e)
    {
        CPLError( CE_Failure, CPLE_AppDefined, "Failed to write attributes: %s", e.what() );
        return CE_Failure;
    }
    return CE_None;
}


// clean up our overview objects
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
    // delete any existing overview bands
    this->deleteOverviewObjects();

    m_nOverviews = this->m_pImageIO->getNumOfOverviews(this->nBand);
    m_panOverviewBands = (KEAOverview**)CPLMalloc(sizeof(KEAOverview*) * m_nOverviews);

    unsigned long nXSize, nYSize;    
    for( int nCount = 0; nCount < m_nOverviews; nCount++ )
    {
        this->m_pImageIO->getOverviewSize(this->nBand, nCount + 1, &nXSize, &nYSize);
        m_panOverviewBands[nCount] = new KEAOverview((KEADataset*)this->poDS, this->nBand, 
                                        this->m_pImageIO, this->m_pnRefCount, nCount + 1, nXSize, nYSize);
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
