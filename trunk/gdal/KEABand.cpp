
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

    // grab the description here
    this->sDescription = pImageIO->getImageBandDescription(nBand);

    m_papszMetadataList = NULL;
    this->UpdateMetadataList();
}

KEARasterBand::~KEARasterBand()
{
    CSLDestroy(m_papszMetadataList);
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

void KEARasterBand::UpdateMetadataList()
{
    std::vector< std::pair<std::string, std::string> > data;

    data = this->m_pImageIO->getImageBandMetaData(this->nBand);
    for(std::vector< std::pair<std::string, std::string> >::iterator iterMetaData = data.begin(); iterMetaData != data.end(); ++iterMetaData)
    {
        m_papszMetadataList = CSLSetNameValue(m_papszMetadataList, iterMetaData->first.c_str(), iterMetaData->second.c_str());
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
        this->sDescription = pszDescription;
    }
    catch (libkea::KEAIOException &e)
    {
    }
}

const char *KEARasterBand::GetDescription() const
{
    // do we need to implement this?
    return this->sDescription.c_str();
}

CPLErr KEARasterBand::SetMetadataItem(const char *pszName, const char *pszValue, const char *pszDomain)
{
    try
    {
        this->m_pImageIO->setImageBandMetaData(this->nBand, pszName, pszValue );
        // CSLSetNameValue will update if already there
        m_papszMetadataList = CSLSetNameValue( m_papszMetadataList, pszName, pszValue );
        return CE_None;
    }
    catch (libkea::KEAIOException &e)
    {
        return CE_Failure;
    }
}

const char *KEARasterBand::GetMetadataItem (const char *pszName, const char *pszDomain)
{
    return CSLFetchNameValue(m_papszMetadataList, pszName);
}

char **KEARasterBand::GetMetadata(const char *pszDomain)
{ 
    return m_papszMetadataList; 
}

CPLErr KEARasterBand::SetMetadata(char **papszMetadata, const char *pszDomain)
{
    int nIndex = 0;
    char *pszName;
    const char *pszValue;
    try
    {
        while( papszMetadata[nIndex] != NULL )
        {
            pszValue = CPLParseNameValue( papszMetadata[nIndex], &pszName );
            this->m_pImageIO->setImageBandMetaData(this->nBand, pszName, pszValue );
            nIndex++;
        }
    }
    catch (libkea::KEAIOException &e)
    {
        return CE_Failure;
    }

    CSLDestroy(m_papszMetadataList);
    m_papszMetadataList = CSLDuplicate(papszMetadata);
    return CE_None;
}

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
