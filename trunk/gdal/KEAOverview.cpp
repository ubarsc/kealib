
#include "KEAOverview.h"

KEAOverview::KEAOverview(KEADataset *pDataset, int nBand, 
                libkea::KEAImageIO *pImageIO, int *pRefCount,
                int nOverviewIndex, int nXSize, int nYSize)
 : KEARasterBand( pDataset, nBand, pImageIO, pRefCount )
{
    this->m_nOverviewIndex = nOverviewIndex;
    // overridden from the band
    this->nBlockXSize = pImageIO->getOverviewBlockSize(nBand, nOverviewIndex);
    this->nBlockYSize = pImageIO->getOverviewBlockSize(nBand, nOverviewIndex);
    this->nRasterXSize = nXSize;
    this->nRasterYSize = nYSize;
}

KEAOverview::~KEAOverview()
{

}

CPLErr KEAOverview::IReadBlock( int nBlockXOff, int nBlockYOff, void * pImage )
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
        this->m_pImageIO->readFromOverview( this->nBand, this->m_nOverviewIndex,
                                            pImage, this->nBlockXSize * nBlockXOff,
                                            this->nBlockYSize * nBlockYOff,
                                            xsize, ysize, 
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

CPLErr KEAOverview::IWriteBlock( int nBlockXOff, int nBlockYOff, void * pImage )
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

        this->m_pImageIO->writeToOverview( this->nBand, this->m_nOverviewIndex,
                                            pImage, this->nBlockXSize * nBlockXOff,
                                            this->nBlockYSize * nBlockYOff,
                                            xsize, ysize, 
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
