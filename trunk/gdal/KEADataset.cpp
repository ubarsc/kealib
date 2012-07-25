
#include "KEADataset.h"
#include "KEABand.h"

// Function for converting a libkea type into a GDAL type
GDALDataType KEA_to_GDAL_Type( libkea::KEADataType keaType )
{
    GDALDataType gdalType = GDT_Unknown;
    switch( keaType )
    {
        case libkea::kea_8int:
        case libkea::kea_8uint:
            gdalType = GDT_Byte;
            break;
        case libkea::kea_16int:
            gdalType = GDT_Int16;
            break;
        case libkea::kea_32int:
            gdalType = GDT_Int32;
            break;
        case libkea::kea_16uint:
            gdalType = GDT_UInt16;
            break;
        case libkea::kea_32uint:
            gdalType = GDT_UInt32;
            break;
        case libkea::kea_32float:
            gdalType = GDT_Float32;
            break;
        case libkea::kea_64float:
            gdalType = GDT_Float64;
            break;
        default:
            gdalType = GDT_Unknown;
            break;
    }
    return gdalType;
}

// function for converting a GDAL type to a libkea type
libkea::KEADataType GDAL_to_KEA_Type( GDALDataType gdalType )
{
    libkea::KEADataType keaType = libkea::kea_undefined;
    switch( gdalType )
    {
        case GDT_Byte:
            keaType = libkea::kea_8uint;
            break;
        case GDT_Int16:
            keaType = libkea::kea_16int;
            break;
        case GDT_Int32:
            keaType = libkea::kea_32int;
            break;
        case GDT_UInt16:
            keaType = libkea::kea_16uint;
            break;
        case GDT_UInt32:
            keaType = libkea::kea_32uint;
            break;
        case GDT_Float32:
            keaType = libkea::kea_32float;
            break;
        case GDT_Float64:
            keaType = libkea::kea_64float;
            break;
        default:
            keaType = libkea::kea_undefined;
            break;
    }
    return keaType;
}

// static function - pointer set in driver 
GDALDataset *KEADataset::Open( GDALOpenInfo * poOpenInfo )
{
    bool isKEA = false;
    try
    {
        // is this a KEA file?
        isKEA = libkea::KEAImageIO::isKEAImage( poOpenInfo->pszFilename );
    }
    catch (libkea::KEAIOException &e)
    {
        isKEA = false;
    }

    if( isKEA )
    {
        try
        {
            // try and open it in the appropriate mode
            H5::H5File *pH5File;
            if( poOpenInfo->eAccess == GA_ReadOnly )
            {
                pH5File = libkea::KEAImageIO::openKeaH5RDOnly( poOpenInfo->pszFilename );
            }
            else
            {
                pH5File = libkea::KEAImageIO::openKeaH5RW( poOpenInfo->pszFilename );
            }
            // create the KEADataset object
            KEADataset *pDataset = new KEADataset( pH5File );

            // set the description as the name
            pDataset->SetDescription( poOpenInfo->pszFilename );

            return pDataset;
        }
        catch (libkea::KEAIOException &e)
        {
            // was a problem - can't be a valid file
            return NULL;
        }
    }
    else
    {
        // not a KEA file
        return NULL;
    }
}

// static function- pointer set in driver
// this function is called in preference to Open
// 
int KEADataset::Identify( GDALOpenInfo * poOpenInfo )
{
    bool isKEA = false;
    try
    {
        // is this a KEA file?
        isKEA = libkea::KEAImageIO::isKEAImage( poOpenInfo->pszFilename );
    }
    catch (libkea::KEAIOException &e)
    {
        isKEA = false;
    }
    if( isKEA )
        return 1;
    else
        return 0;
}

// static function- pointer set in driver
GDALDataset *KEADataset::Create( const char * pszFilename,
                                  int nXSize, int nYSize, int nBands,
                                  GDALDataType eType,
                                  char ** papszParmList  )
{
    // process any creation options in papszParmList
    // default value
    unsigned int imageblockSize = libkea::KEA_IMAGE_CHUNK_SIZE;
    // see if they have provided a different value
    const char *pszValue = CSLFetchNameValue( papszParmList, "IMAGEBLOCKSIZE" );
    if( pszValue != NULL )
        imageblockSize = atol( pszValue );

    unsigned int attblockSize = libkea::KEA_ATT_CHUNK_SIZE;
    pszValue = CSLFetchNameValue( papszParmList, "ATTBLOCKSIZE" );
    if( pszValue != NULL )
        attblockSize = atol( pszValue );

    unsigned int mdcElmts = libkea::KEA_MDC_NELMTS;
    pszValue = CSLFetchNameValue( papszParmList, "MDC_NELMTS" );
    if( pszValue != NULL )
        mdcElmts = atol( pszValue );

    hsize_t rdccNElmts = libkea::KEA_RDCC_NELMTS;
    pszValue = CSLFetchNameValue( papszParmList, "RDCC_NELMTS" );
    if( pszValue != NULL )
        rdccNElmts = atol( pszValue );

    hsize_t rdccNBytes = libkea::KEA_RDCC_NBYTES;
    pszValue = CSLFetchNameValue( papszParmList, "RDCC_NBYTES" );
    if( pszValue != NULL )
        rdccNBytes = atol( pszValue );

    double rdccW0 = libkea::KEA_RDCC_W0;
    pszValue = CSLFetchNameValue( papszParmList, "RDCC_W0" );
    if( pszValue != NULL )
        rdccW0 = atof( pszValue );

    hsize_t sieveBuf = libkea::KEA_SIEVE_BUF;
    pszValue = CSLFetchNameValue( papszParmList, "SIEVE_BUF" );
    if( pszValue != NULL )
        sieveBuf = atol( pszValue );

    hsize_t metaBlockSize = libkea::KEA_META_BLOCKSIZE;
    pszValue = CSLFetchNameValue( papszParmList, "META_BLOCKSIZE" );
    if( pszValue != NULL )
        metaBlockSize = atol( pszValue );

    unsigned int deflate = libkea::KEA_DEFLATE;
    pszValue = CSLFetchNameValue( papszParmList, "DEFLATE" );
    if( pszValue != NULL )
        deflate = atol( pszValue );

    try
    {
        // now create it
        H5::H5File *keaImgH5File = libkea::KEAImageIO::createKEAImage( pszFilename,
                                                    GDAL_to_KEA_Type( eType ),
                                                    nXSize, nYSize, nBands,
                                                    NULL, NULL, imageblockSize, attblockSize, mdcElmts, rdccNElmts,
                                                    rdccNBytes, rdccW0, sieveBuf, metaBlockSize, deflate );
        // create our dataset object                            
        KEADataset *pDataset = new KEADataset( keaImgH5File );

        pDataset->SetDescription( pszFilename );

        return pDataset;
    }
    catch (libkea::KEAIOException &e)
    {
        CPLError( CE_Failure, CPLE_OpenFailed,
                  "Attempt to create file `%s' failed. Error: %s\n",
                  pszFilename, e.what() );
        return NULL;
    }
}

// constructor
KEADataset::KEADataset( H5::H5File *keaImgH5File )
{
    try
    {
        // create the image IO and initilize the refcount
        m_pImageIO = new libkea::KEAImageIO();
        m_pnRefcount = new int(1);

        // open the file
        m_pImageIO->openKEAImageHeader( keaImgH5File );
        libkea::KEAImageSpatialInfo *pSpatialInfo = m_pImageIO->getSpatialInfo();

        // get the dimensions
        nBands = m_pImageIO->getNumOfImageBands();
        nRasterXSize = pSpatialInfo->xSize;
        nRasterYSize = pSpatialInfo->ySize;

        // create all the bands
        for( int nCount = 0; nCount < nBands; nCount++ )
        {
            // note GDAL uses indices starting at 1 and so does libkea
            // create band object
            KEARasterBand *pBand = new KEARasterBand( this, nCount + 1, m_pImageIO, m_pnRefcount );
            // read in overviews
            pBand->readExistingOverviews();
            // set the band into this dataset
            this->SetBand( nCount + 1, pBand );            
        }

        // read in the metadata
        m_papszMetadataList = NULL;
        this->UpdateMetadataList();
    }
    catch (libkea::KEAIOException &e)
    {
        // ignore?
        CPLError( CE_Warning, CPLE_AppDefined,
                "Caught exception in KEADataset constructor %s", e.what() );
    }
}

KEADataset::~KEADataset()
{
    // destroy the metadata
    CSLDestroy(m_papszMetadataList);
    // decrement the refcount and delete if needed
    (*m_pnRefcount)--;
    if( *m_pnRefcount == 0 )
    {
        m_pImageIO->close();
        delete m_pImageIO;
        delete m_pnRefcount;
    }
}

// read in the metadata into our CSLStringList
void KEADataset::UpdateMetadataList()
{
    std::vector< std::pair<std::string, std::string> > data;
    // get all the metadata
    data = this->m_pImageIO->getImageMetaData();
    for(std::vector< std::pair<std::string, std::string> >::iterator iterMetaData = data.begin(); iterMetaData != data.end(); ++iterMetaData)
    {
        m_papszMetadataList = CSLSetNameValue(m_papszMetadataList, iterMetaData->first.c_str(), iterMetaData->second.c_str());
    }
}

// read in the geotransform
CPLErr KEADataset::GetGeoTransform( double * padfTransform )
{
    try
    {
        libkea::KEAImageSpatialInfo *pSpatialInfo = m_pImageIO->getSpatialInfo();
        // GDAL uses an array format
        padfTransform[0] = pSpatialInfo->tlX;
        padfTransform[1] = pSpatialInfo->xRes;
        padfTransform[2] = pSpatialInfo->xRot;
        padfTransform[3] = pSpatialInfo->tlY;
        padfTransform[4] = pSpatialInfo->yRot;
        padfTransform[5] = pSpatialInfo->yRes;
    
        return CE_None;
    }
    catch (libkea::KEAIOException &e)
    {
        CPLError( CE_Warning, CPLE_AppDefined,
                "Unable to read geotransform: %s", e.what() );
        return CE_Failure;
    }
}

// read in the projection ref
const char *KEADataset::GetProjectionRef()
{
    try
    {
        libkea::KEAImageSpatialInfo *pSpatialInfo = m_pImageIO->getSpatialInfo();
        // should be safe since pSpatialInfo should be around a while...
        return pSpatialInfo->wktString.c_str();
    }
    catch (libkea::KEAIOException &e)
    {
        return NULL;
    }
}

// set the geotransform
CPLErr KEADataset::SetGeoTransform (double *padfTransform )
{
    try
    {
        // get the spatial info and update it
        libkea::KEAImageSpatialInfo *pSpatialInfo = m_pImageIO->getSpatialInfo();
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
    catch (libkea::KEAIOException &e)
    {
        CPLError( CE_Warning, CPLE_AppDefined,
                "Unable to write geotransform: %s", e.what() );
        return CE_Failure;
    }
}

// set the projection
CPLErr KEADataset::SetProjection( const char *pszWKT )
{
    try
    {
        // get the spatial info and update it
        libkea::KEAImageSpatialInfo *pSpatialInfo = m_pImageIO->getSpatialInfo();

        pSpatialInfo->wktString = pszWKT;

        m_pImageIO->setSpatialInfo( pSpatialInfo );
        return CE_None;
    }
    catch (libkea::KEAIOException &e)
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
CPLErr KEADataset::IBuildOverviews(const char *pszResampling, int nOverviews, int *panOverviewList, 
                                    int nListBands, int *panBandList, GDALProgressFunc pfnProgress, 
                                    void *pProgressData)
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
        if( GDALRegenerateOverviews( (GDALRasterBandH)pBand, nOverviews, (GDALRasterBandH*)pBand->GetOverviewList(),
                                    pszResampling, pfnProgress, pProgressData ) != CE_None )
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
    // only deal with 'default' domain - no geolocation etc
    if( ( pszDomain != NULL ) && ( *pszDomain != '\0' ) )
        return CE_Failure;

    try
    {
        this->m_pImageIO->setImageMetaData(pszName, pszValue );
        // CSLSetNameValue will update if already there
        m_papszMetadataList = CSLSetNameValue( m_papszMetadataList, pszName, pszValue );
        return CE_None;
    }
    catch (libkea::KEAIOException &e)
    {
        return CE_Failure;
    }
}

// get a single metadata item
const char *KEADataset::GetMetadataItem (const char *pszName, const char *pszDomain)
{
    // only deal with 'default' domain - no geolocation etc
    if( ( pszDomain != NULL ) && ( *pszDomain != '\0' ) )
        return NULL;
    // string returned from CSLFetchNameValue should be persistant
    return CSLFetchNameValue(m_papszMetadataList, pszName);
}

// get the whole metadata as CSLStringList
char **KEADataset::GetMetadata(const char *pszDomain)
{ 
    // only deal with 'default' domain - no geolocation etc
    if( ( pszDomain != NULL ) && ( *pszDomain != '\0' ) )
        return NULL;
    // this is what we store it as anyway
    return m_papszMetadataList; 
}

// set the whole metadata as a CSLStringList
CPLErr KEADataset::SetMetadata(char **papszMetadata, const char *pszDomain)
{
    // only deal with 'default' domain - no geolocation etc
    if( ( pszDomain != NULL ) && ( *pszDomain != '\0' ) )
        return CE_Failure;

    int nIndex = 0;
    char *pszName;
    const char *pszValue;
    try
    {
        // go through each item
        while( papszMetadata[nIndex] != NULL )
        {
            // get the value/name
            pszValue = CPLParseNameValue( papszMetadata[nIndex], &pszName );
            // set it with imageio
            this->m_pImageIO->setImageMetaData(pszName, pszValue );
            nIndex++;
        }
    }
    catch (libkea::KEAIOException &e)
    {
        return CE_Failure;
    }

    // destroy our one and replace it
    CSLDestroy(m_papszMetadataList);
    m_papszMetadataList = CSLDuplicate(papszMetadata);
    return CE_None;
}
