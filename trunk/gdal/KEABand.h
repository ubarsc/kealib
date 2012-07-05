
#ifndef KEABAND_H
#define KEABAND_H

#include "gdal_pam.h"
#include "KEADataset.h"

class KEARasterBand : public GDALPamRasterBand
{
    libkea::KEAImageIO *m_pImageIO;
public:
    KEARasterBand( KEADataset *pDataset, int nBand, libkea::KEAImageIO *pImageIO );

    virtual CPLErr IReadBlock( int, int, void * );
    virtual CPLErr IWriteBlock( int, int, void * );
};


#endif //KEABAND_H
