
#include "KEADataset.h"

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
            H5::H5File *pH5File = libkea::KEAImageIO::openKEAImageRDOnly( poOpenInfo->pszFilename );
            KEADataset *pDataset = new KEADataset( pH5File );
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

KEADataset::KEADataset( H5::H5File *keaImgH5File )
{
    m_ImageIO.openKEAImage( keaImgH5File );
}

KEADataset::~KEADataset()
{
    m_ImageIO.close();
}

CPLErr KEADataset::GetGeoTransform( double * padfTransform )
{
    padfTransform[0] = 0;
    padfTransform[1] = 0;
    padfTransform[2] = 0;
    padfTransform[3] = 0;
    padfTransform[4] = 0;
    padfTransform[5] = 0;
    
    return CE_None;
}

const char *KEADataset::GetProjectionRef()
{
    return "";
}

