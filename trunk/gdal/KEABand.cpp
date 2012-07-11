
#include "KEABand.h"
#include "KEAOverview.h"

KEARasterBand::KEARasterBand( KEADataset *pDataset, int nBand, libkea::KEAImageIO *pImageIO, int *pRefCount )
{
    this->poDS = pDataset;
    this->nBand = nBand;
    this->eDataType = KEA_to_GDAL_Type( pImageIO->getImageBandDataType(nBand) );
    this->nBlockXSize = pImageIO->getImageBlockSize(nBand);
    this->nBlockYSize = pImageIO->getImageBlockSize(nBand);
    this->nRasterXSize = this->poDS->GetRasterXSize();
    this->nRasterYSize = this->poDS->GetRasterYSize();

    this->m_pImageIO = pImageIO;
    this->m_pnRefCount = pRefCount;
    // increment the refcount as we now have a reference to imageio
    (*this->m_pnRefCount)++;

    // overviews
    m_nOverviews = 0;
    m_panOverviewBands = NULL;
}

KEARasterBand::~KEARasterBand()
{
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

void KEARasterBand::CreateOverviews(int nOverviews, int *panOverviewList)
{
    // delete any existing overview bands
    this->deleteOverviewObjects();

    m_panOverviewBands = (KEAOverview**)CPLMalloc(sizeof(KEAOverview*) * nOverviews);
    m_nOverviews = nOverviews;

    int nFactor, nXSize, nYSize;
    for( int nCount = 0; nCount < m_nOverviews; nCount++ )
    {
        nFactor = panOverviewList[nCount];
        nXSize = this->nRasterXSize / nFactor;
        nYSize = this->nRasterYSize / nFactor;

        this->m_pImageIO->createOverview(this->nBand, nCount + 1, nXSize, nYSize);

        m_panOverviewBands[nCount] = new KEAOverview((KEADataset*)this->poDS, this->nBand, 
                                        this->m_pImageIO, this->m_pnRefCount, nCount + 1, nXSize, nYSize);
    }
}


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
                                            xsize, ysize, 
                                            this->m_pImageIO->getImageBandDataType(this->nBand) );
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
                                            xsize, ysize, 
                                            this->m_pImageIO->getImageBandDataType(this->nBand) );
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
    }
    catch (libkea::KEAIOException &e)
    {
    }
}

const char *KEARasterBand::GetDescription() const
{
    try
    {
        const char *psz = this->m_pImageIO->getImageBandDescription(this->nBand).c_str();
        return strdup(psz);
    }
    catch (libkea::KEAIOException &e)
    {
        return "";
    }
}

CPLErr KEARasterBand::SetMetadataItem (const char *pszName, const char *pszValue, const char *pszDomain)
{
    try
    {
        this->m_pImageIO->setImageBandMetaData(this->nBand, pszName, pszValue );
        return CE_None;
    }
    catch (libkea::KEAIOException &e)
    {
        return CE_Failure;
    }
}

const char *KEARasterBand::GetMetadataItem (const char *pszName, const char *pszDomain)
{
    try
    {
        const char *psz = this->m_pImageIO->getImageBandMetaData(this->nBand, pszName).c_str();
        return strdup(psz);
    }
    catch (libkea::KEAIOException &e)
    {
        return NULL;
    }
}

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

int KEARasterBand::GetOverviewCount()
{
    return m_nOverviews;
}

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
