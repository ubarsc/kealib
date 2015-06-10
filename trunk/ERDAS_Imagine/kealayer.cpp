/*
 *  kealayer.cpp
 *  LibKEA
 *
 *  Created by Sam Gillingham on 26/07/2013.
 *  Copyright 2013 LibKEA. All rights reserved.
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

#include "kea.h"

// #define KEADEBUG 1

long
keaLayerOpen(void *fHandle, char *lName, unsigned long *pType, unsigned long *width, unsigned long *height, unsigned long *compression,
		unsigned long *bWidth, unsigned long *bHeight, void **lHandle)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %s %p\n", __FUNCTION__, lName, fHandle );
#endif
    KEA_File *pKEAFile = (KEA_File*)fHandle;
    long rCode = -1;
    *lHandle = NULL;
    KEA_Layer *pLayer = NULL;
    std::string sName = lName;
    std::string sSearch = ":Mask:Mask";
    size_t pos = sName.find(sSearch, 0);
    if( pos != std::string::npos )
    {
        // Imagine does some very weird things - this is a workaround.
        sName.replace(pos, sSearch.length(), ":Mask");
#ifdef KEADEBUG
        keaDebugOut( "Converted %s -> %s in layer name\n", lName, sName.c_str());
#endif        
    }
    
    for( std::vector<KEA_Layer*>::iterator itr = pKEAFile->aLayers.begin();
            (itr != pKEAFile->aLayers.end()) && (pLayer == NULL); itr++ )
    {
        KEA_Layer *pCandidate = (*itr);
        if( pCandidate->sName == sName )
        {
            pLayer = pCandidate;
            *pType = (unsigned long)pLayer->eKEAType; // see keaInstancePixelTypesGet
            *width = pLayer->nXSize;
            *height = pLayer->nYSize;
            *compression = 0; // zlib, see keaInstanceCompressionTypesGet
            *bWidth = pLayer->nBlockSize;
            *bHeight = pLayer->nBlockSize;
            *lHandle = pLayer;
            rCode = 0;
#ifdef KEADEBUG
            keaDebugOut( "%s returning %p\n", __FUNCTION__, pLayer );
#endif
        }
    }
    if( pLayer == NULL )
    {
#ifdef KEADEBUG
        keaDebugOut( "Can't find layer %s\n", sName.c_str() );
#endif
        }
#ifdef KEADEBUG
    keaDebugOut( "open layer returning %p\n", pLayer );
#endif
	return rCode;
}

long
keaLayerClose(void *lHandle)
{
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;
#ifdef KEADEBUG
    keaDebugOut( "%s %s %p\n", __FUNCTION__, pLayer->sName.c_str(), pLayer );
#endif
    // do nothing since the layers are owned by the dataset
	return 0;
}

long
keaLayerCreate(void  *fileHandle,  /* Input */
 char  **layerName,  /* Output */
 unsigned long  pType,  /* Input */
 unsigned long  width,  /* Input */
 unsigned long  height,  /* Input */
 unsigned long  compression,  /* Input */
 unsigned long  *bWidth,  /* In/Out */
 unsigned long  *bHeight,  /* In/Out */
 void  **layerHandle  /* Output */)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %p %ld %ld\n", __FUNCTION__, fileHandle, width, height);
#endif
    long rCode = -1;    
    KEA_File *pKEAFile = (KEA_File*)fileHandle;
    Eerr_ErrorReport* err = NULL;
    
    if( !pKEAFile->bUpdate )
    {
        return -1;
    }
    
    uint32_t nBand = 1;
    if( pKEAFile->pImageIO == NULL )
    {
        // first layer - create it. Guess 1 band.
        // default other options
        H5::H5File *keaImgH5File = kealib::KEAImageIO::createKEAImage(pKEAFile->sFilePath,
                                (kealib::KEADataType)pType, width, height, 1);
        if( keaImgH5File != NULL )
        {
            // create the KEA_Layer etc
            pKEAFile->pH5File = keaImgH5File;
            pKEAFile->pImageIO = new kealib::KEAImageIO();
            pKEAFile->pImageIO->openKEAImageHeader( keaImgH5File );
            // sFilePath etc already set in keaFileTitleIdentifyAndOpen
            // on creation
        }
    }
    else
    {
        // add a band
        // Work out a name - should probably check it doesn't already exist too
        nBand = pKEAFile->pImageIO->getNumOfImageBands() + 1;
        char *name = estr_Sprintf( NULL, (char*)"Layer_%d", &err, 
                                nBand, NULL );
        HANDLE_ERR(err, NULL);
        std::string sName = name;
        emsc_Free(name);
        try
        {
           pKEAFile->pImageIO->addImageBand((kealib::KEADataType)pType, sName); 
        } 
        catch (kealib::KEAIOException &e) 
        {
            keaDebugOut( "layer creation failed: %s\n", e.what());
            return -1;
        }
    }
    
    // all ok
    if( pKEAFile->pImageIO != NULL )
    {
        pKEAFile->modTime = time(NULL);
        kealib::KEAImageIO *pImageIO = pKEAFile->pImageIO;
        kealib::KEAImageSpatialInfo *pSpatialInfo = pImageIO->getSpatialInfo();
        
        // hack - assume overview if smaller than original size
        bool bIsOverview = (width < pSpatialInfo->xSize) || (height < pSpatialInfo->ySize);
        
        // the 'real' layer
        KEA_Layer *pLayer = new KEA_Layer();
        pLayer->pKEAFile = pKEAFile;
        std::string sName = pImageIO->getImageBandDescription(nBand);
        // Imagine doesn't like spaces
        std::replace(sName.begin(), sName.end(), ' ', '_');
        keaDebugOut( "added layer '%s'\n", sName.c_str());
        pLayer->sName = sName;
        pLayer->nBand = nBand;
        pLayer->bIsOverview = bIsOverview;
        pLayer->bIsMask = false;
        pLayer->bMaskIsReal = false;
        pLayer->nOverview = 99999;
        pLayer->eKEAType = pImageIO->getImageBandDataType(nBand);
        pLayer->nXSize = width;
        pLayer->nYSize = height;
        pLayer->nBlockSize = pImageIO->getImageBlockSize(nBand);
        pLayer->pKEAFile = pKEAFile;
        pKEAFile->aLayers.push_back(pLayer);
        
        // mask - Imagine 2015 requires us to have one for each band
        KEA_Layer *pMask = new KEA_Layer();
        pMask->pKEAFile = pKEAFile;
        char *name = estr_Sprintf( NULL, (char*)"%s:Mask", &err, 
                    sName.c_str(), NULL );
        HANDLE_ERR(err, -1);
        pMask->sName = name;
        emsc_Free(name);
        pMask->nBand = nBand;
        pMask->bIsOverview = bIsOverview;
        pMask->bIsMask = true;
        pMask->bMaskIsReal = pImageIO->maskCreated(nBand);
        pMask->nOverview = 99999;
        pMask->eKEAType = kealib::kea_8uint;
        pMask->nXSize = width;
        pMask->nYSize = height;
        pMask->nBlockSize = pImageIO->getImageBlockSize(nBand);
        pMask->pKEAFile = pKEAFile;
        pKEAFile->aLayers.push_back(pMask);
        
        *bWidth = pLayer->nBlockSize;
        *bHeight = pLayer->nBlockSize;
        *layerName = estr_Duplicate(sName.c_str());
        *layerHandle = pLayer;
        rCode = 0;
#ifdef KEADEBUG
        keaDebugOut( "%s returning %p\n", __FUNCTION__, pLayer);
#endif
    }
    return rCode;
}

// This function needs to exist before Imagine will 
// write to KEA. But KEA doesn't support layer deletion
// so we return an error.
long
keaLayerDestroy(void *fHandle, char *layerName)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %p %s\n", __FUNCTION__, fHandle, layerName);
#endif
    return -1;
}

long
keaLayerRasterRead(void	*lHandle, unsigned long	bRow, unsigned long bCol, unsigned char	**pixels)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %ld %ld %p %p\n", __FUNCTION__, bRow, bCol, *pixels, lHandle);
#endif
    long rCode = -1;
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;
#ifdef KEADEBUG
//    fprintf( stderr, "Reading %s\n", pLayer->sName.c_str());
#endif

    uint64_t xsize = pLayer->nBlockSize;
    uint64_t xtotalsize = pLayer->nBlockSize * (bCol + 1);
    if( xtotalsize > pLayer->nXSize )
    {
        xsize -= (xtotalsize - pLayer->nXSize);
    }
    uint64_t ysize = pLayer->nBlockSize;
    uint64_t ytotalsize = pLayer->nBlockSize * (bRow + 1);
    if( ytotalsize > pLayer->nYSize )
    {
        ysize -= (ytotalsize - pLayer->nYSize);
    }
    
    kealib::KEAImageIO *pImageIO = pLayer->getImageIO();
    
    try
    {
        if( pLayer->bIsOverview && !pLayer->bIsMask )
        {
            // overviews can have masks in Imagine
            pImageIO->readFromOverview( pLayer->nBand, pLayer->nOverview,
                                            (*pixels), pLayer->nBlockSize * bCol,
                                            pLayer->nBlockSize * bRow,
                                            xsize, ysize, pLayer->nBlockSize, pLayer->nBlockSize, 
                                            pLayer->eKEAType );
        }
        else if( pLayer->bIsMask )
        {
            // is int8 - set to 1
            // According to Haiyan Qu: "IMAGINE mask layer is a binary layer, 1 is data, 0 is nodata"
            if( pLayer->bMaskIsReal )
            {
                // read the mask out of the KEA file
                pImageIO->readImageBlock2BandMask(pLayer->nBand, (*pixels), 
                                                pLayer->nBlockSize * bCol,
                                                pLayer->nBlockSize * bRow,
                                                xsize, ysize, pLayer->nBlockSize, pLayer->nBlockSize,
                                                pLayer->eKEAType ); // is uint8
                                                
                // KEA/GDAL mask layers are generally 255 where valid data
                // recode the mask we have just read
                unsigned char *pMaskData = *pixels;
                for( uint64_t i = 0; i < (pLayer->nBlockSize*pLayer->nBlockSize); i++ )
                {
                    if( pMaskData[i] > 1 )
                    {
                        pMaskData[i] = 1;
                    }
                }
            }
            else
            {
                // fake it by saying all valid
                memset(*pixels, 1, pLayer->nBlockSize * pLayer->nBlockSize);
            }
        }
        else
        {
            pImageIO->readImageBlock2Band( pLayer->nBand, (*pixels), pLayer->nBlockSize * bCol,
                                            pLayer->nBlockSize * bRow,
                                            xsize, ysize, pLayer->nBlockSize, pLayer->nBlockSize, 
                                            pLayer->eKEAType );
        }
        rCode = 0;
    }
    catch (kealib::KEAIOException &e)
    {
        keaDebugOut( "Exception in %s: %s\n", __FUNCTION__, e.what());
        rCode = -1;
    }
	return rCode;
}

long
keaLayerRasterWrite(void *lHandle, unsigned long bRow, unsigned long bCol, unsigned char *pixels)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %ld %ld %p %p\n", __FUNCTION__, bRow, bCol, *pixels, lHandle);
#endif
    long rCode = -1;
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;
#ifdef KEADEBUG
    keaDebugOut( "writing %s\n", pLayer->sName.c_str());
#endif

    uint64_t xsize = pLayer->nBlockSize;
    uint64_t xtotalsize = pLayer->nBlockSize * (bCol + 1);
    if( xtotalsize > pLayer->nXSize )
    {
        xsize -= (xtotalsize - pLayer->nXSize);
    }
    uint64_t ysize = pLayer->nBlockSize;
    uint64_t ytotalsize = pLayer->nBlockSize * (bRow + 1);
    if( ytotalsize > pLayer->nYSize )
    {
        ysize -= (ytotalsize - pLayer->nYSize);
    }
    
    kealib::KEAImageIO *pImageIO = pLayer->getImageIO();
    
    try
    {
        if( pLayer->bIsOverview && !pLayer->bIsMask)
        {
            pImageIO->writeToOverview( pLayer->nBand, pLayer->nOverview,
                                            pixels, pLayer->nBlockSize * bCol,
                                            pLayer->nBlockSize * bRow,
                                            xsize, ysize, pLayer->nBlockSize, pLayer->nBlockSize, 
                                            pLayer->eKEAType );
        }
        else if( pLayer->bIsMask )
        {
            if( !pLayer->bIsOverview)
            {
                // only do this if not an overview mask - in this case
                // do nothing for now
                if( !pLayer->bMaskIsReal )
                {
                    // well we have been asked to write
                    // it so it is now very real!
                    pImageIO->createMask(pLayer->nBand);
                    pLayer->bMaskIsReal = true;
                }
                
                // KEA/GDAL mask layers are generally 255 where valid data
                // recode the data we about to write - not sure this is a good idea
                // is int8 - set to 1
                // According to Haiyan Qu: "IMAGINE mask layer is a binary layer, 1 is data,
                unsigned char *pGDALMask = (unsigned char*)malloc(pLayer->nBlockSize*pLayer->nBlockSize * sizeof(unsigned char));
                if( pGDALMask == NULL )
                    return -1;
                for( uint64_t i = 0; i < (pLayer->nBlockSize*pLayer->nBlockSize); i++ )
                {
                    if( pixels[i] == 1 )
                    {
                        pGDALMask[i] = 255;
                    }
                    else
                    {
                        pGDALMask[i] = 0;
                    }
                }

                    // write the mask to the KEA file
                pImageIO->writeImageBlock2BandMask(pLayer->nBand, pGDALMask, 
                                                    pLayer->nBlockSize * bCol,
                                                    pLayer->nBlockSize * bRow,
                                                    xsize, ysize, pLayer->nBlockSize, pLayer->nBlockSize,
                                                    pLayer->eKEAType ); // is uint8
                free(pGDALMask);
            }
        }
        else
        {
            pImageIO->writeImageBlock2Band( pLayer->nBand, pixels, pLayer->nBlockSize * bCol,
                                            pLayer->nBlockSize * bRow,
                                            xsize, ysize, pLayer->nBlockSize, pLayer->nBlockSize, 
                                            pLayer->eKEAType );
        }
        rCode = 0;
    }
    catch (kealib::KEAIOException &e)
    {
        keaDebugOut( "Exception in %s: %s\n", __FUNCTION__, e.what());
        rCode = -1;
    }
	return rCode;
}

long
keaLayerLayerTypeRead(void  *lHandle, unsigned long  *lType)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %p\n", __FUNCTION__, lHandle );
#endif
    long rCode = -1;
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;
    kealib::KEAImageIO *pImageIO = pLayer->getImageIO();
    try
    {
        kealib::KEALayerType keaType = pImageIO->getImageBandLayerType(pLayer->nBand);
        *lType = (unsigned long)keaType; // see keaInstanceLayerTypesGet
        rCode = 0;
    }
    catch (kealib::KEAIOException &e)
    {
#ifdef KEADEBUG
        keaDebugOut( "Exception in %s: %s\n", __FUNCTION__, e.what());
#endif
        rCode = -1;
    }
    return rCode;
}

long
keaLayerLayerTypeWrite(void  *lHandle, unsigned long  lType)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %p\n", __FUNCTION__, lHandle );
#endif
    long rCode = -1;
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;
    kealib::KEAImageIO *pImageIO = pLayer->getImageIO();
    try
    {
        pImageIO->setImageBandLayerType(pLayer->nBand, (kealib::KEALayerType)lType); // see keaInstanceLayerTypesGet
        rCode = 0;
    }
    catch (kealib::KEAIOException &e)
    {
#ifdef KEADEBUG
        keaDebugOut( "Exception in %s: %s\n", __FUNCTION__, e.what());
#endif
        rCode = -1;
    }
    return rCode;
}

long 
keaLayerRRDLayerNamesGet(void *lHandle, unsigned long  *count, char  ***layerNames, char  **algorithm)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %p\n", __FUNCTION__, lHandle );
#endif
    Eerr_ErrorReport* err = NULL;
    long rCode = -1;
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;
    
    try
    {
        unsigned long layerCount = 0;
        // get the pointer back to the KEA_File object 
        // and iterate through the list of layers looking for overviews
        for( std::vector<KEA_Layer*>::iterator itr = pLayer->pKEAFile->aLayers.begin();
            itr != pLayer->pKEAFile->aLayers.end(); itr++ )
        {
            KEA_Layer *pCandidate = (*itr);
            if( pCandidate->bIsOverview && !pCandidate->bIsMask )
            {
                size_t nColonLoc = pCandidate->sName.find(':');
                if( ( nColonLoc != std::string::npos ) && ( pCandidate->sName.compare(0, nColonLoc, pLayer->sName) == 0 ) )
                {
                    layerCount++;
                }
            }
        }

        if( layerCount > 0 )
        {
            *count = layerCount;
            *layerNames = emsc_New(layerCount, char *);
            *algorithm = estr_Duplicate((char*)"Unknown");
            layerCount = 0;
            for( std::vector<KEA_Layer*>::iterator itr = pLayer->pKEAFile->aLayers.begin();
            itr != pLayer->pKEAFile->aLayers.end(); itr++ )
            {
                KEA_Layer *pCandidate = (*itr);
                if( pCandidate->bIsOverview  && !pCandidate->bIsMask )
                {
                    size_t nColonLoc = pCandidate->sName.find(':');
                    if( ( nColonLoc != std::string::npos ) && ( pCandidate->sName.compare(0, nColonLoc, pLayer->sName) == 0 ) )
                    {
                        // couldn't work out efnp_FileNodeCreate...
                        std::string sFilePath = pLayer->getFilePath();
                        char *name = estr_Sprintf( NULL, (char*)"%s(:%s)", &err, 
                            sFilePath.c_str(), pCandidate->sName.c_str(), NULL );
                        HANDLE_ERR(err, -1);
                        (*layerNames)[layerCount] = name;
#ifdef KEADEBUG                        
                        keaDebugOut( "RRD: %s\n", name);
#endif                        
                        layerCount++;
                    }
                }
            }
        }
        else
        {
            // no overviews
            *layerNames = NULL;
            *count = 0;
            *algorithm = NULL;
        }
        rCode = 0;
    }
    catch (kealib::KEAIOException &e)
    {
        keaDebugOut( "Exception in %s: %s\n", __FUNCTION__, e.what());
        rCode = -1;
    }
    return rCode;
}

long
keaLayerRRDLayerNamesSet(void  *lHandle, unsigned long  count, char  **layerNames, 
 char  *algorithm)
 {
 #ifdef KEADEBUG
    keaDebugOut( "%s %p\n", __FUNCTION__, lHandle );
#endif
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;

    for(unsigned long n = 0; n < count; n++ )
    {
        keaDebugOut( "request to RRD %s\n", layerNames[n]);
    }
    return -1;
 }

// common GDAL strings
#define METADATA_MIN "STATISTICS_MINIMUM" 
#define METADATA_MAX "STATISTICS_MAXIMUM" 
#define METADATA_MEAN "STATISTICS_MEAN" 
#define METADATA_MEDIAN "STATISTICS_MEDIAN" 
#define METADATA_STDDEV "STATISTICS_STDDEV"
#define METADATA_MODE "STATISTICS_MODE"

long
keaLayerScalarStatisticsRead(void *lHandle, double *minimum,  double *maximum, double *mean, double *median, double *mode, double *stddev)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %p\n", __FUNCTION__, lHandle );
#endif
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;
    long rCode = 0; // we are doing a count
    kealib::KEAImageIO *pImageIO = pLayer->getImageIO();
    try
    {
        // have to get all the metadata in one go and iterate through
        std::vector< std::pair<std::string, std::string> > data;
        data = pImageIO->getImageBandMetaData(pLayer->nBand);
        for(std::vector< std::pair<std::string, std::string> >::iterator iterMetaData = data.begin(); iterMetaData != data.end(); ++iterMetaData)
        {
            if( iterMetaData->first == METADATA_MIN )
            {
                *minimum = atof(iterMetaData->second.c_str());
                rCode++;
            }
            else if( iterMetaData->first == METADATA_MAX )
            {
                *maximum = atof(iterMetaData->second.c_str());
                rCode++;
            }
            else if( iterMetaData->first == METADATA_MEAN )
            {
                *mean = atof(iterMetaData->second.c_str());
                rCode++;
            }
            else if( iterMetaData->first == METADATA_MEDIAN )
            {
                *median = atof(iterMetaData->second.c_str());
                rCode++;
            }
            else if( iterMetaData->first == METADATA_MODE )
            {
                *mode = atof(iterMetaData->second.c_str());
                rCode++;
            }
            else if( iterMetaData->first == METADATA_STDDEV )
            {
                *stddev = atof(iterMetaData->second.c_str());
                rCode++;
            }
        }
    }
    catch (kealib::KEAIOException &e)
    {
        keaDebugOut( "Exception in %s: %s\n", __FUNCTION__, e.what());
        rCode = -1;
    }
    return rCode;
}

long
keaLayerScalarStatisticsWrite(void *lHandle, double *minimum,   
 double *maximum, double *mean, double *median, double *mode,   
 double *stddev)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %p\n", __FUNCTION__, lHandle );
#endif
    long rCode = -1;
    Eerr_ErrorReport* err = NULL;
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;
    kealib::KEAImageIO *pImageIO = pLayer->getImageIO();
    try
    {
        if( minimum != NULL )
        {
            char *value = estr_Sprintf( NULL, (char*)"%f", &err, 
                    *minimum, NULL );
            pImageIO->setImageMetaData(METADATA_MIN, value);
            emsc_Free(value);
        }
        if( maximum != NULL )
        {
            char *value = estr_Sprintf( NULL, (char*)"%f", &err, 
                    *maximum, NULL );
            pImageIO->setImageMetaData(METADATA_MAX, value);
            emsc_Free(value);
        }
        if( mean != NULL )
        {
            char *value = estr_Sprintf( NULL, (char*)"%f", &err, 
                    *mean, NULL );
            pImageIO->setImageMetaData(METADATA_MEAN, value);
            emsc_Free(value);
        }
        if( median != NULL )
        {
            char *value = estr_Sprintf( NULL, (char*)"%f", &err, 
                    *median, NULL );
            pImageIO->setImageMetaData(METADATA_MEDIAN, value);
            emsc_Free(value);
        }
        if( mode != NULL )
        {
            char *value = estr_Sprintf( NULL, (char*)"%f", &err, 
                    *mode, NULL );
            pImageIO->setImageMetaData(METADATA_MODE, value);
            emsc_Free(value);
        }
        if( stddev != NULL )
        {
            char *value = estr_Sprintf( NULL, (char*)"%f", &err, 
                    *stddev, NULL );
            pImageIO->setImageMetaData(METADATA_STDDEV, value);
            emsc_Free(value);
        }      
        rCode = 0;
    }
    catch (kealib::KEAIOException &e)
    {
        keaDebugOut( "Exception in %s: %s\n", __FUNCTION__, e.what());
        rCode = -1;
    }
    return rCode;
} 
 
long
keaLayerMapInfoRead(void *lHandle, char **projection, double *xULC, double *yULC, double *xPixelSize, double *yPixelSize, char **units)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %p\n", __FUNCTION__, lHandle );
#endif
    long rCode = -1;
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;
    kealib::KEAImageIO *pImageIO = pLayer->getImageIO();
    try
    {
        kealib::KEAImageSpatialInfo *pSpatialInfo = pImageIO->getSpatialInfo();
        // KEA/GDAL uses full extent, Imagine uses centre of top left pixel
        *xULC = pSpatialInfo->tlX + (pSpatialInfo->xRes / 2.0);
        *yULC = pSpatialInfo->tlY + (pSpatialInfo->yRes / 2.0); // x res is negative anyway
        *xPixelSize = pSpatialInfo->xRes;
        *yPixelSize = pSpatialInfo->yRes;

        // already have projname and units from when we parsed the wkt
        // when the file was opened
        *projection = estr_Duplicate((char*)pLayer->pKEAFile->sProjName.c_str());
        *units = estr_Duplicate((char*)pLayer->pKEAFile->sUnits.c_str());
        rCode = 0;
    }
    catch (kealib::KEAIOException &e)
    {
        keaDebugOut( "Exception in %s: %s\n", __FUNCTION__, e.what());
        rCode = -1;
    }
    return rCode;
}

long
keaLayerMapInfoWrite(void *lHandle, char *projection, double xULC, 
 double yULC, double xPixelSize, double yPixelSize, char *units)
 {
 #ifdef KEADEBUG
    keaDebugOut( "%s %p %s %s\n", __FUNCTION__, lHandle, projection, units );
#endif
    long rCode = -1;
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;
    kealib::KEAImageIO *pImageIO = pLayer->getImageIO();
    try
    {
        kealib::KEAImageSpatialInfo *pSpatialInfo = pImageIO->getSpatialInfo();
        // KEA/GDAL uses full extent, Imagine uses centre of top left pixel
        pSpatialInfo->tlX = xULC - (xPixelSize / 2.0);
        pSpatialInfo->tlY = yULC - (yPixelSize / 2.0); // x res is negative anyway
        pSpatialInfo->xRes = xPixelSize;
        pSpatialInfo->yRes = yPixelSize;
        pImageIO->setSpatialInfo(pSpatialInfo);
        
        // TODO: save this properly into the file
        if( projection != NULL )
        {
            pLayer->pKEAFile->sProjName = projection;
        }
        if( units != NULL )
        {
            pLayer->pKEAFile->sUnits = units;
        }
        rCode = 0;
    }        
    catch (kealib::KEAIOException &e)
    {
        keaDebugOut( "Exception in %s: %s\n", __FUNCTION__, e.what());
        rCode = -1;
    }
    return rCode;
}
 
long 
keaLayerMapProjectionRead(void *lHandle, char **projTitle, unsigned char **MIFproj,
        unsigned long  *MIFprojSize, char  **MIFprojDictionary, char  **MIFprojName,
        unsigned char  **MIFearthModel, unsigned long  *MIFearthModelSize,
        char  **MIFearthModelDictionary, char  **MIFearthModelName)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %p\n", __FUNCTION__, lHandle );
#endif
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;
    Eerr_ErrorReport* err = NULL;
    Eprj_MapProjection* pProj = pLayer->pKEAFile->pProj;
    if( pProj == NULL ) // it's not failure - just don't have any info
        return 0;

    eprj_ProjectionConvertToMIF(pProj, projTitle, MIFproj,
        MIFprojSize, MIFprojDictionary, MIFprojName,
        MIFearthModel, MIFearthModelSize,
        MIFearthModelDictionary, MIFearthModelName, &err);
    HANDLE_ERR(err, -1)

    return 0;
}

long
keaLayerMapProjectionWrite(void  *lHandle, char  *projTitle,    
 unsigned char  *MIFproj, unsigned long  MIFprojSize,    
 char  *MIFprojDictionary, char  *MIFprojName, 
 unsigned char  *MIFearthModel,  unsigned long  MIFearthModelSize, 
 char  *MIFearthModelDictionary, char  *MIFearthModelName)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %p %s\n", __FUNCTION__, lHandle, projTitle );
#endif
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;
    Eerr_ErrorReport* err = NULL;
    long rCode = -1;
    if(projTitle == NULL)
    {
        // clobber existing data
        kealib::KEAImageIO *pImageIO = pLayer->getImageIO();
        try
        {
            kealib::KEAImageSpatialInfo *pSpatialInfo = pImageIO->getSpatialInfo();
            pSpatialInfo->wktString = "";
            pImageIO->setSpatialInfo(pSpatialInfo);
        }
        catch (kealib::KEAIOException &e)
        {
            keaDebugOut( "Exception in %s: %s\n", __FUNCTION__, e.what());
            rCode = -1;
        }
    }
    else
    {
        Eprj_MapProjection* pProj;

        pProj = eprj_ProjectionConvertFromMIF(projTitle, MIFproj, MIFprojSize,
                    MIFprojDictionary, MIFprojName, MIFearthModel, MIFearthModelSize,
                    MIFearthModelDictionary, MIFearthModelName, &err);
        HANDLE_ERR(err, -1)

        // if we have one, free it
        if( pLayer->pKEAFile->pProj != NULL )
        {
            eprj_ProjectionFree(&pLayer->pKEAFile->pProj);
        }
        
        // save it in case we are asked again
        pLayer->pKEAFile->pProj = pProj;

#ifdef KEADEBUG
        char *pszName = eprj_MapProjectionName(pProj);
        keaDebugOut( "projname = %s\n", pszName );
#endif
        
        rCode = 0;
    }
    return rCode;
}
 
long
keaLayerRasterNullValueRead(void  *lHandle, unsigned char  **pixel)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %p\n", __FUNCTION__, lHandle );
#endif
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;
    kealib::KEAImageIO *pImageIO = pLayer->getImageIO();

    if( pLayer->bIsMask )
    {
        **pixel = 0; // according to Haiyan Qu: "IMAGINE mask layer is a binary layer, 1 is data, 0 is nodata"
    }
    else
    {
        try
        {
            // I *think* this will work - just treat it as the right type...
            pImageIO->getNoDataValue(pLayer->nBand, (void*)(*pixel), pLayer->eKEAType);
        }
        catch (kealib::KEAIOException &e)
        {
            // throws exception when a no data has not been set so can't tell if an error
            *pixel = NULL;
        }
    }
    return 0;
    
}

long
keaLayerRasterNullValueWrite( void  *lHandle, 
 unsigned char  *pixel )
 {
#ifdef KEADEBUG
    keaDebugOut( "%s %p\n", __FUNCTION__, lHandle );
#endif
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;
    kealib::KEAImageIO *pImageIO = pLayer->getImageIO();
    if( !pLayer->bIsMask )    
    {
        try
        {
            pImageIO->setNoDataValue(pLayer->nBand, (void*)pixel, pLayer->eKEAType);
        }
        catch (kealib::KEAIOException &e)
        {
            return -1;
        }
    }
    return 0;
 }

#define METADATA_HISTOMIN "STATISTICS_HISTOMIN"
#define METADATA_HISTOMAX "STATISTICS_HISTOMAX"
#define METADATA_HISTONBINS "STATISTICS_HISTONUMBINS"
#define METADATA_HISTOBINFN "STATISTICS_HISTOBINFUNCTION"

// internal function - returns a Edsc_BinFunction* that describes
// the binning of the histogram. Called from keaFileDataRead
Edsc_BinFunction*
keaLayerGetHistoBinFunction(KEA_Layer *pLayer)
{
    Edsc_BinFunction *pBinFn = emsc_New(1, Edsc_BinFunction);
    if( pBinFn == NULL )
    {
        return NULL;
    }
    kealib::KEAImageIO *pImageIO = pLayer->getImageIO();
    
    // set up some defaults
    pBinFn->numBins = 256;
    pBinFn->binFunctionType = EDSC_DIRECT_BINS;
    pBinFn->minLimit = 0;
    pBinFn->maxLimit = 255;
    pBinFn->binLimits = NULL;
    pBinFn->binFunctionLimitsType = EDSC_LIMITS_GIVEN;

    // now read through the metadata to get this info if present
    std::vector< std::pair<std::string, std::string> > data;
    try
    {
        data = pImageIO->getImageBandMetaData(pLayer->nBand);
    }
    catch (kealib::KEAIOException &e)
    {
        return NULL;
    }
    for(std::vector< std::pair<std::string, std::string> >::iterator iterMetaData = data.begin(); iterMetaData != data.end(); ++iterMetaData)
    {
        if( iterMetaData->first == METADATA_HISTOMIN )
        {
            pBinFn->minLimit = atof(iterMetaData->second.c_str());
        }
        else if( iterMetaData->first == METADATA_HISTOMAX )
        {
            pBinFn->maxLimit = atof(iterMetaData->second.c_str());
        }
        else if( iterMetaData->first == METADATA_HISTONBINS )
        {
            pBinFn->numBins = atol(iterMetaData->second.c_str());
        }
        else if( iterMetaData->first == METADATA_HISTOBINFN )
        {
            if( iterMetaData->second == "direct" )
            {
                pBinFn->binFunctionType = EDSC_DIRECT_BINS;
            }
            else if( iterMetaData->second == "linear" )
            {
                pBinFn->binFunctionType = EDSC_LINEAR_BINS;
            }
            else if( iterMetaData->second == "exponential" )
            {
                pBinFn->binFunctionType = EDSC_LOGARITHMIC_BINS;
            }
            // we don't support explicit
        }
    }

    return pBinFn;
}

#ifdef WIN32
    #define snprintf _snprintf
#endif

// opposite of above
void keaLayerSetHistoBinFunction(KEA_Layer *pLayer, Edsc_BinFunction *pBinFn)
{
    char szTemp[256];
    kealib::KEAImageIO *pImageIO = pLayer->getImageIO();
    
    try
    {
        snprintf(szTemp, 256, "%f", pBinFn->minLimit);
        pImageIO->setImageBandMetaData(pLayer->nBand, METADATA_HISTOMIN, szTemp);

        snprintf(szTemp, 256, "%f", pBinFn->maxLimit);
        pImageIO->setImageBandMetaData(pLayer->nBand, METADATA_HISTOMAX, szTemp);

        snprintf(szTemp, 256, "%ld", pBinFn->numBins);
        pImageIO->setImageBandMetaData(pLayer->nBand, METADATA_HISTONBINS, szTemp);
        
        if( pBinFn->binFunctionType == EDSC_DIRECT_BINS )
        {
            pImageIO->setImageBandMetaData(pLayer->nBand, METADATA_HISTOBINFN, "direct");
        }
        else if( pBinFn->binFunctionType == EDSC_LINEAR_BINS )
        {
            pImageIO->setImageBandMetaData(pLayer->nBand, METADATA_HISTOBINFN, "linear");
        }
        else if( pBinFn->binFunctionType == EDSC_LOGARITHMIC_BINS )
        {
            pImageIO->setImageBandMetaData(pLayer->nBand, METADATA_HISTOBINFN, "exponential");
        }
    }
    catch (kealib::KEAIOException &e)
    {
#ifdef KEADEBUG
        keaDebugOut( "%s setting metadata failed\n", __FUNCTION__ );
#endif
    }
}

// the following don't seem to get called - RAT used instead
/*
long
keaLayerHistogramModTimeGet(void  *lHandle, time_t *modTime)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %p\n", __FUNCTION__, lHandle );
#endif
    return -1;
}

long
keaLayerHistogramRead(void  *lHandle, long startRow, long numRows, long *histogram)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %p %ld %ld\n", __FUNCTION__, lHandle, startRow, numRows );
#endif
    return -1;
}

long 
keaLayerColorRead(void *lHandle, char *colorName, long startRow, long numRows, double *colorTable)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %p %s %ld %ld\n", __FUNCTION__, lHandle, colorName, startRow, numRows );
#endif
    return -1;
}

long
keaLayerColorModTimeGet(void *lHandle, char *colorName, time_t *modTime)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %p %s\n", __FUNCTION__, lHandle, colorName);
#endif
    return -1;
}

long 
keaLayerOpacityRead(void *lHandle, long startRow, long numRows, double *opacity)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %p %ld %ld\n", __FUNCTION__, lHandle, startRow, numRows);
#endif
    return -1;
}

long
keaLayerOpacityModTimeGet(void *lHandle, time_t *modTime)
{
#ifdef KEADEBUG
    keaDebugOut( "%s %p\n", __FUNCTION__, lHandle);
#endif
    return -1;
}

long 
keaLayerClassNamesRead(void *lHandle, long startRow, long numRows, char **classNames)
{
    keaDebugOut( "%s %p\n", __FUNCTION__, lHandle);
    return -1;
}
*/
