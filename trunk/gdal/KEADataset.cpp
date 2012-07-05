
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
            H5::H5File *pH5File;
            if( poOpenInfo->eAccess == GA_ReadOnly )
            {
                pH5File = libkea::KEAImageIO::openKEAImageRDOnly( poOpenInfo->pszFilename );
            }
            else
            {
                pH5File = libkea::KEAImageIO::openKEAImageRW( poOpenInfo->pszFilename );
            }
            KEADataset *pDataset = new KEADataset( pH5File );

            pDataset->SetDescription( poOpenInfo->pszFilename );

            return pDataset;
        }
        catch (libkea::KEAIOException &e)
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

// static function- pointer set in driver
GDALDataset *KEADataset::Create( const char * pszFilename,
                                  int nXSize, int nYSize, int nBands,
                                  GDALDataType eType,
                                  char ** papszParmList  )
{
    // default value
    unsigned int blockSize = libkea::KEA_WRITE_CHUNK_SIZE;
    // see if they have provided a different value
    const char *pszValue = CSLFetchNameValue( papszParmList, "BLOCKSIZE" );
    if( pszValue != NULL )
    {
        blockSize = atol( pszValue );
    }

    try
    {
        H5::H5File *keaImgH5File = libkea::KEAImageIO::createKEAImage( pszFilename,
                                                    GDAL_to_KEA_Type( eType ),
                                                    nXSize, nYSize, nBands,
                                                    NULL, NULL, blockSize );
                                                    
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
        // open the file
        m_ImageIO.openKEAImage( keaImgH5File );
        libkea::KEAImageSpatialInfo *pSpatialInfo = m_ImageIO.getSpatialInfo();

        // get the dimensions
        nBands = m_ImageIO.getNumOfImageBands();
        nRasterXSize = pSpatialInfo->xSize;
        nRasterYSize = pSpatialInfo->ySize;

        // create all the bands
        for( int nCount = 0; nCount < nBands; nCount++ )
        {
            // note GDAL uses indices starting at 1
            KEARasterBand *pBand = new KEARasterBand( this, nCount + 1, &m_ImageIO );
            this->SetBand( nCount + 1, pBand );            
        }
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
    m_ImageIO.close();
}

CPLErr KEADataset::GetGeoTransform( double * padfTransform )
{
    try
    {
        libkea::KEAImageSpatialInfo *pSpatialInfo = m_ImageIO.getSpatialInfo();

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

const char *KEADataset::GetProjectionRef()
{
    try
    {
        libkea::KEAImageSpatialInfo *pSpatialInfo = m_ImageIO.getSpatialInfo();
        return pSpatialInfo->wktString.c_str();
    }
    catch (libkea::KEAIOException &e)
    {
        return NULL;
    }
}

CPLErr KEADataset::SetGeoTransform (double *padfTransform )
{
    try
    {
        libkea::KEAImageSpatialInfo *pSpatialInfo = m_ImageIO.getSpatialInfo();
        pSpatialInfo->tlX = padfTransform[0];
        pSpatialInfo->xRes = padfTransform[1];
        pSpatialInfo->xRot = padfTransform[2];
        pSpatialInfo->tlY = padfTransform[3];
        pSpatialInfo->yRot = padfTransform[4];
        pSpatialInfo->yRes = padfTransform[5];

        m_ImageIO.setSpatialInfo( pSpatialInfo );
        return CE_None;
    }
    catch (libkea::KEAIOException &e)
    {
        CPLError( CE_Warning, CPLE_AppDefined,
                "Unable to write geotransform: %s", e.what() );
        return CE_Failure;
    }
}

CPLErr KEADataset::SetProjection( const char *pszWKT )
{
    try
    {
        libkea::KEAImageSpatialInfo *pSpatialInfo = m_ImageIO.getSpatialInfo();

        pSpatialInfo->wktString = pszWKT;

        m_ImageIO.setSpatialInfo( pSpatialInfo );
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
    return &m_ImageIO;
}

