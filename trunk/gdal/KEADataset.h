
#ifndef KEADATASET_H
#define KEADATASET_H

#include "gdal_pam.h"
#include "libkea/KEAImageIO.h"

class KEADataset : public GDALPamDataset
{
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

    void *GetInternalHandle (const char *);

    CPLErr SetMetadataItem (const char *pszName, const char *pszValue, const char *pszDomain="");
    const char *GetMetadataItem (const char *pszName, const char *pszDomain="");

    char **GetMetadata(const char *pszDomain="");
    CPLErr SetMetadata(char **papszMetadata, const char *pszDomain="");

protected:
    virtual CPLErr IBuildOverviews(const char *pszResampling, int nOverviews, int *panOverviewList, 
                                    int nListBands, int *panBandList, GDALProgressFunc pfnProgress, 
                                    void *pProgressData);

    void UpdateMetadataList();

private:
    // pointer to KEAImageIO class and the refcount for it
    libkea::KEAImageIO  *m_pImageIO;
    int                 *m_pnRefcount;
    char               **m_papszMetadataList;

};

// conversion functions
GDALDataType KEA_to_GDAL_Type( libkea::KEADataType keaType );
libkea::KEADataType GDAL_to_KEA_Type( GDALDataType gdalType );

#endif //KEADATASET_H
