
#include "KEABand.h"

KEARasterBand::KEARasterBand( KEADataset *pDataset, int nBand, libkea::KEAImageIO *pImageIO )
{
    this->poDS = pDataset;
    this->nBand = nBand;
    this->eDataType = KEA_to_GDAL_Type( pImageIO->getImageDataType() );
    // TODO
    this->nBlockXSize = libkea::KEA_WRITE_CHUNK_SIZE;
    this->nBlockYSize = libkea::KEA_WRITE_CHUNK_SIZE;

    this->m_pImageIO = pImageIO;
}

CPLErr KEARasterBand::IReadBlock( int nBlockXOff, int nBlockYOff, void * pImage )
{
    try
    {
        this->m_pImageIO->readImageBlock2Band( this->nBand - 1, pImage, this->nBlockXSize * nBlockXOff,
                                            this->nBlockYSize * nBlockYOff,
                                            this->nBlockXSize, this->nBlockYSize, 
                                            this->m_pImageIO->getImageDataType() );
        return CE_None;
    }
    catch (libkea::KEAIOException &e)
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                "Failed to read file" );
        return CE_Failure;
    }
}

CPLErr KEARasterBand::IWriteBlock( int nBlockXOff, int nBlockYOff, void * pImage )
{
    try
    {
        this->m_pImageIO->writeImageBlock2Band( this->nBand - 1, pImage, this->nBlockXSize * nBlockXOff,
                                            this->nBlockYSize * nBlockYOff,
                                            this->nBlockXSize, this->nBlockYSize, 
                                            this->m_pImageIO->getImageDataType() );
        return CE_None;
    }
    catch (libkea::KEAIOException &e)
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                "Failed to write file" );
        return CE_Failure;
    }
}

