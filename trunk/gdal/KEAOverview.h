
#ifndef KEAOVERVIEW_H
#define KEAOVERVIEW_H

#include "KEABand.h"

class KEAOverview : public KEARasterBand
{
    int         m_nOverviewIndex;
public:
    KEAOverview(KEADataset *pDataset, int nBand, libkea::KEAImageIO *pImageIO, int *pRefCount,
                int nOverviewIndex, int nXSize, int nYSize );
    ~KEAOverview();

protected:
    virtual CPLErr IReadBlock( int, int, void * );
    virtual CPLErr IWriteBlock( int, int, void * );
};

#endif //KEAOVERVIEW_H