
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

    void CreateOverviews(int nOverviews, int *panOverviewList);
    KEAOverview** GetOverviewList() { return m_panOverviewBands; }

protected:
    virtual CPLErr IReadBlock( int, int, void * );
    virtual CPLErr IWriteBlock( int, int, void * );

    libkea::KEAImageIO  *m_pImageIO;
};


#endif //KEABAND_H
