
#include "gdal_pam.h"
#include "libkea/KEAImageIO.h"

class KEADataset : public GDALPamDataset
{
    libkea::KEAImageIO m_ImageIO;
public:
    KEADataset( H5::H5File *keaImgH5File );
    ~KEADataset();
    
    static GDALDataset *Open( GDALOpenInfo * );

    CPLErr      GetGeoTransform( double * padfTransform );
    const char *GetProjectionRef();
};
