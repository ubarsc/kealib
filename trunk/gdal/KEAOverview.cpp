
#include "KEAOverview.h"

// constructor
KEAOverview::KEAOverview(KEADataset *pDataset, int nSrcBand, 
                libkea::KEAImageIO *pImageIO, int *pRefCount,
                int nOverviewIndex, int nXSize, int nYSize)
 : KEARasterBand( pDataset, nSrcBand, pImageIO, pRefCount )
{
    this->m_nOverviewIndex = nOverviewIndex;
    // overridden from the band - not the same size as the band obviously
    this->nBlockXSize = pImageIO->getOverviewBlockSize(nSrcBand, nOverviewIndex);
    this->nBlockYSize = pImageIO->getOverviewBlockSize(nSrcBand, nOverviewIndex);
    this->nRasterXSize = nXSize;
    this->nRasterYSize = nYSize;
}

KEAOverview::~KEAOverview()
{

}

// overridden implementation - calls readFromOverview instead
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

// overridden implementation - calls writeToOverview instead
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

const GDALRasterAttributeTable *KEAOverview::GetDefaultRAT()
{
    // KEARasterBand implements this, but we don't want to
    return NULL;
}

CPLErr KEAOverview::SetDefaultRAT(const GDALRasterAttributeTable *poRAT)
{
    // KEARasterBand implements this, but we don't want to
    return CE_Failure;    
}
