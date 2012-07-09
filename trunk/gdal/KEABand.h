
#ifndef KEABAND_H
#define KEABAND_H

#include "gdal_pam.h"
#include "KEADataset.h"

class KEARasterBand : public GDALPamRasterBand
{
    libkea::KEAImageIO  *m_pImageIO;
    int                 *m_pnRefCount;
    int                  m_nXSize;
    int                  m_nYSize;
public:
    KEARasterBand( KEADataset *pDataset, int nBand, libkea::KEAImageIO *pImageIO, int* pRefCount );
    ~KEARasterBand();

protected:
    virtual CPLErr IReadBlock( int, int, void * );
    virtual CPLErr IWriteBlock( int, int, void * );
};


#endif //KEABAND_H
