
#ifndef KEABAND_H
#define KEABAND_H

#include "gdal_pam.h"
#include "KEADataset.h"

class KEAOverview;

// Provides the implementation of a GDAL raster band
class KEARasterBand : public GDALPamRasterBand
{
private:
    int                 *m_pnRefCount; // reference count of m_pImageIO

    int                  m_nOverviews; // number of overviews
    KEAOverview        **m_panOverviewBands; // array of overview objects

    GDALRasterAttributeTable  *m_pAttributeTable; // pointer to the attribute table
                                                 // created on first call to GetDefaultRAT()

public:
    // constructor/destructor
    KEARasterBand( KEADataset *pDataset, int nSrcBand, libkea::KEAImageIO *pImageIO, int *pRefCount );
    ~KEARasterBand();

    // virtual methods for overview support
    int GetOverviewCount();
    GDALRasterBand* GetOverview(int nOverview);

    // virtual methods for band names (aka description)
    void SetDescription(const char *);
    const char *GetDescription () const;

    // virtual methods for handling the metadata
    CPLErr SetMetadataItem (const char *pszName, const char *pszValue, const char *pszDomain="");
    const char *GetMetadataItem (const char *pszName, const char *pszDomain="");
    char **GetMetadata(const char *pszDomain="");
    CPLErr SetMetadata(char **papszMetadata, const char *pszDomain="");

    // virtual methods for the no data value
    double GetNoDataValue(int *pbSuccess=NULL);
    CPLErr SetNoDataValue(double dfNoData);

    // virtual methods for RATs
    const GDALRasterAttributeTable *GetDefaultRAT();

    // internal methods for overviews
    void readExistingOverviews();
    void deleteOverviewObjects();
    void CreateOverviews(int nOverviews, int *panOverviewList);
    KEAOverview** GetOverviewList() { return m_panOverviewBands; }

protected:
    // methods for accessing data as blocks
    virtual CPLErr IReadBlock( int, int, void * );
    virtual CPLErr IWriteBlock( int, int, void * );

    // updates m_papszMetadataList
    void UpdateMetadataList();

    libkea::KEAImageIO  *m_pImageIO; // our image access pointer - refcounted
    char               **m_papszMetadataList; // CPLStringList of metadata
    libkea::KEADataType  m_eKEADataType; // data type as KEA enum
};


#endif //KEABAND_H
