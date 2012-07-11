
#ifndef KEABAND_H
#define KEABAND_H

#include "gdal_pam.h"
#include "KEADataset.h"

class KEAOverview;

class KEARasterBand : public GDALPamRasterBand
{
    int                 *m_pnRefCount;

    int                  m_nOverviews;
    KEAOverview        **m_panOverviewBands;
public:
    KEARasterBand( KEADataset *pDataset, int nBand, libkea::KEAImageIO *pImageIO, int *pRefCount );
    ~KEARasterBand();

    int GetOverviewCount();
    GDALRasterBand* GetOverview(int nOverview);

    void SetDescription(const char *);
    const char *GetDescription () const;
    CPLErr SetMetadataItem (const char *pszName, const char *pszValue, const char *pszDomain="");
    const char *GetMetadataItem (const char *pszName, const char *pszDomain="");
    double GetNoDataValue(int *pbSuccess=NULL);
    CPLErr SetNoDataValue(double dfNoData);

    void readExistingOverviews();
    void deleteOverviewObjects();
    void CreateOverviews(int nOverviews, int *panOverviewList);
    KEAOverview** GetOverviewList() { return m_panOverviewBands; }

protected:
    virtual CPLErr IReadBlock( int, int, void * );
    virtual CPLErr IWriteBlock( int, int, void * );

    libkea::KEAImageIO  *m_pImageIO;
};


#endif //KEABAND_H
