
#ifndef KEAOVERVIEW_H
#define KEAOVERVIEW_H

#include "KEABand.h"

// overview class. Derives from our band class
// and just overrited and read/write block functions
class KEAOverview : public KEARasterBand
{
    int         m_nOverviewIndex; // the index of this overview
public:
    KEAOverview(KEADataset *pDataset, int nSrcBand, libkea::KEAImageIO *pImageIO, int *pRefCount,
                int nOverviewIndex, int nXSize, int nYSize );
    ~KEAOverview();

    // virtual methods for RATs - not implemented for overviews
    const GDALRasterAttributeTable *GetDefaultRAT();

protected:
    // we just override these functions from KEARasterBand
    virtual CPLErr IReadBlock( int, int, void * );
    virtual CPLErr IWriteBlock( int, int, void * );
};

#endif //KEAOVERVIEW_H
