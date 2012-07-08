
#include "KEABand.h"

KEARasterBand::KEARasterBand( KEADataset *pDataset, int nBand, libkea::KEAImageIO *pImageIO )
{
    this->poDS = pDataset;
    this->nBand = nBand;
    this->eDataType = KEA_to_GDAL_Type( pImageIO->getImageDataType() );
    this->nBlockXSize = pImageIO->getImageBlockSize();
    this->nBlockYSize = pImageIO->getImageBlockSize();

    this->m_pImageIO = pImageIO;
}

CPLErr KEARasterBand::IReadBlock( int nBlockXOff, int nBlockYOff, void * pImage )
{
    try
    {
        // note GDAL uses indices starting at 1
        this->m_pImageIO->readImageBlock2Band( this->nBand, pImage, this->nBlockXSize * nBlockXOff,
                                            this->nBlockYSize * nBlockYOff,
                                            this->nBlockXSize, this->nBlockYSize, 
                                            this->m_pImageIO->getImageDataType() );
        return CE_None;
    }
    catch (libkea::KEAIOException &e)
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                "Failed to read file: %s", e.what() );
        return CE_Failure;
    }
}

CPLErr KEARasterBand::IWriteBlock( int nBlockXOff, int nBlockYOff, void * pImage )
{
    try
    {
        // note GDAL uses indices starting at 1
        this->m_pImageIO->writeImageBlock2Band( this->nBand, pImage, this->nBlockXSize * nBlockXOff,
                                            this->nBlockYSize * nBlockYOff,
                                            this->nBlockXSize, this->nBlockYSize, 
                                            this->m_pImageIO->getImageDataType() );
        return CE_None;
    }
    catch (libkea::KEAIOException &e)
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                "Failed to write file: %s", e.what() );
        return CE_Failure;
    }
}

