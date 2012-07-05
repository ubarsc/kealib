
#ifndef KEADATASET_H
#define KEADATASET_H

#include "gdal_pam.h"
#include "libkea/KEAImageIO.h"

class KEADataset : public GDALPamDataset
{
    libkea::KEAImageIO m_ImageIO;
public:
    KEADataset( H5::H5File *keaImgH5File );
    ~KEADataset();
    
    static GDALDataset *Open( GDALOpenInfo * );
    static GDALDataset *Create( const char * pszFilename,
                                  int nXSize, int nYSize, int nBands,
                                  GDALDataType eType,
                                  char **  papszParmList );

    CPLErr      GetGeoTransform( double * padfTransform );
    const char *GetProjectionRef();

    CPLErr  SetGeoTransform (double *padfTransform );
    CPLErr SetProjection( const char *pszWKT );
};

// conversion functions
GDALDataType KEA_to_GDAL_Type( libkea::KEADataType keaType );
libkea::KEADataType GDAL_to_KEA_Type( GDALDataType gdalType );

#endif //KEADATASET_H
