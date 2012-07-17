
#ifndef KEADATASET_H
#define KEADATASET_H

#include "gdal_pam.h"
#include "libkea/KEAImageIO.h"

// class that implements a GDAL dataset
class KEADataset : public GDALPamDataset
{
public:
    // constructor/destructor
    KEADataset( H5::H5File *keaImgH5File );
    ~KEADataset();
    
    // static methods that handle open and creation
    // the driver class has pointers to these
    static GDALDataset *Open( GDALOpenInfo * );
    static GDALDataset *Create( const char * pszFilename,
                                  int nXSize, int nYSize, int nBands,
                                  GDALDataType eType,
                                  char **  papszParmList );

    // virtual methods for dealing with transform and projection
    CPLErr      GetGeoTransform( double * padfTransform );
    const char *GetProjectionRef();

    CPLErr  SetGeoTransform (double *padfTransform );
    CPLErr SetProjection( const char *pszWKT );

    // method to get a pointer to the imageio class
    void *GetInternalHandle (const char *);

    // virtual methods for dealing with metadata
    CPLErr SetMetadataItem (const char *pszName, const char *pszValue, const char *pszDomain="");
    const char *GetMetadataItem (const char *pszName, const char *pszDomain="");

    char **GetMetadata(const char *pszDomain="");
    CPLErr SetMetadata(char **papszMetadata, const char *pszDomain="");

protected:
    // this method builds overviews for the specified bands. 
    virtual CPLErr IBuildOverviews(const char *pszResampling, int nOverviews, int *panOverviewList, 
                                    int nListBands, int *panBandList, GDALProgressFunc pfnProgress, 
                                    void *pProgressData);

    // internal method to update m_papszMetadataList
    void UpdateMetadataList();

private:
    // pointer to KEAImageIO class and the refcount for it
    libkea::KEAImageIO  *m_pImageIO;
    int                 *m_pnRefcount;
    char               **m_papszMetadataList; // CSLStringList for metadata

};

// conversion functions
GDALDataType KEA_to_GDAL_Type( libkea::KEADataType keaType );
libkea::KEADataType GDAL_to_KEA_Type( GDALDataType gdalType );

#endif //KEADATASET_H
