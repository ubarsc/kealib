/*
 *  keadataset.h
 *
 *  Created by Pete Bunting on 01/08/2012.
 *  Copyright 2012 LibKEA. All rights reserved.
 *
 *  This file is part of LibKEA.
 *
 *  Permission is hereby granted, free of charge, to any person 
 *  obtaining a copy of this software and associated documentation 
 *  files (the "Software"), to deal in the Software without restriction, 
 *  including without limitation the rights to use, copy, modify, 
 *  merge, publish, distribute, sublicense, and/or sell copies of the 
 *  Software, and to permit persons to whom the Software is furnished 
 *  to do so, subject to the following conditions:
 *
 *  The above copyright notice and this permission notice shall be 
 *  included in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
 *  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR 
 *  ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
 *  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
 *  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 */

#ifndef KEADATASET_H
#define KEADATASET_H

#include "gdal_priv.h"
#include "cpl_multiproc.h"
#include "libkea/KEAImageIO.h"

class LockedRefCount;

// class that implements a GDAL dataset
class KEADataset : public GDALDataset
{
public:
    // constructor/destructor
    KEADataset( H5::H5File *keaImgH5File, GDALAccess eAccess );
    ~KEADataset();
    
    // static methods that handle open and creation
    // the driver class has pointers to these
    static GDALDataset *Open( GDALOpenInfo * );
    static int Identify( GDALOpenInfo * poOpenInfo );
    static GDALDataset *Create( const char * pszFilename,
                                  int nXSize, int nYSize, int nBands,
                                  GDALDataType eType,
                                  char **  papszParmList );
    static GDALDataset *CreateCopy( const char * pszFilename, GDALDataset *pSrcDs,
                                int bStrict, char **  papszParmList, 
                                GDALProgressFunc pfnProgress, void *pProgressData );

    // virtual methods for dealing with transform and projection
#ifdef HAVE_SETVALUE_CPLERR
    CPLErr      GetGeoTransform(GDALGeoTransform &gt ) const override;
    CPLErr SetGeoTransform(const GDALGeoTransform &gt) override;
#else    
    CPLErr      GetGeoTransform( double * padfTransform );
    CPLErr  SetGeoTransform (double *padfTransform );
#endif

    const OGRSpatialReference* GetSpatialRef() const override;
    CPLErr SetSpatialRef(const OGRSpatialReference* poSRS) override;

    // method to get a pointer to the imageio class
    void *GetInternalHandle (const char *);

    // virtual methods for dealing with metadata
    CPLErr SetMetadataItem (const char *pszName, const char *pszValue, const char *pszDomain="");
    const char *GetMetadataItem (const char *pszName, const char *pszDomain="");

    char **GetMetadata(const char *pszDomain="");
    CPLErr SetMetadata(char **papszMetadata, const char *pszDomain="");

    // virtual method for adding new image bands
    CPLErr AddBand(GDALDataType eType, char **papszOptions = NULL);
    // removing image bands (only after GDAL 2.0)
    OGRErr DeleteLayer(int iLayer);

    // GCPs
    int GetGCPCount();
    const GDAL_GCP* GetGCPs();
    const OGRSpatialReference* GetGCPSpatialRef() const override;
    CPLErr SetGCPs( int nGCPCount, const GDAL_GCP *pasGCPList,
            const OGRSpatialReference* poSRS ) override;

protected:
    // this method builds overviews for the specified bands. 
#ifdef HAVE_OVERVIEWOPTIONS
    virtual CPLErr IBuildOverviews(const char *pszResampling, int nOverviews, const int *panOverviewList, 
                                    int nListBands, const int *panBandList, GDALProgressFunc pfnProgress, 
                                    void *pProgressData, CSLConstList papszOptions) override;
#else
    virtual CPLErr IBuildOverviews(const char *pszResampling, int nOverviews, int *panOverviewList, 
                                    int nListBands, int *panBandList, GDALProgressFunc pfnProgress, 
                                    void *pProgressData) override;
#endif

    // internal method to update m_papszMetadataList
    void UpdateMetadataList();

    void DestroyGCPs();

private:
    // pointer to KEAImageIO class and the refcount for it
    kealib::KEAImageIO  *m_pImageIO;
    LockedRefCount      *m_pRefcount;
    char               **m_papszMetadataList; // CSLStringList for metadata
    GDAL_GCP            *m_pGCPs;
    mutable OGRSpatialReference  m_oGCPSRS{};
    mutable CPLMutex            *m_hMutex;
    mutable OGRSpatialReference  m_oSRS{};
};

// conversion functions
GDALDataType KEA_to_GDAL_Type( kealib::KEADataType ekeaType );
kealib::KEADataType GDAL_to_KEA_Type( GDALDataType egdalType );

// A thresafe reference count. Used to manage shared pointer to
// the kealib::KEAImageIO instance between bands and dataset.
class LockedRefCount
{
private:
    int m_nRefCount;
    CPLMutex *m_hMutex;

    CPL_DISALLOW_COPY_ASSIGN(LockedRefCount)
    
public:
    LockedRefCount(int initCount=1)
    {
        m_nRefCount = initCount;
        m_hMutex = CPLCreateMutex();
        CPLReleaseMutex( m_hMutex );
    }
    ~LockedRefCount()
    {
        CPLDestroyMutex( m_hMutex );
        m_hMutex = NULL;
    }
    
    void IncRef()
    {
        CPLMutexHolderD( &m_hMutex );
        m_nRefCount++;
    }
    
    // returns true if reference count now 0
    bool DecRef()
    {
        CPLMutexHolderD( &m_hMutex );
        m_nRefCount--;
        return m_nRefCount <= 0;
    }
};

#endif //KEADATASET_H
