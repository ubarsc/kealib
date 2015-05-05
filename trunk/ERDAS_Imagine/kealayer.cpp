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

//#define KEADEBUG 1

long
keaLayerOpen(void *fHandle, char *lName, unsigned long *pType, unsigned long *width, unsigned long *height, unsigned long *compression,
		unsigned long *bWidth, unsigned long *bHeight, void **lHandle)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %s %p\n", __FUNCTION__, lName, fHandle );
#endif
    KEA_File *pKEAFile = (KEA_File*)fHandle;
    long rCode = -1;
    *lHandle = NULL;
    KEA_Layer *pLayer = NULL;
    
    for( unsigned int n = 0; (n < pKEAFile->nLayers) && (pLayer == NULL); n++ )
    {
        if( pKEAFile->ppLayers[n]->sName == lName )
        {
            pLayer = pKEAFile->ppLayers[n];
            *pType = (unsigned long)pLayer->eKEAType; // see keaInstancePixelTypesGet
            *width = pLayer->nXSize;
            *height = pLayer->nYSize;
            *compression = 0; // zlib, see keaInstanceCompressionTypesGet
            *bWidth = pLayer->nBlockSize;
            *bHeight = pLayer->nBlockSize;
            *lHandle = pLayer;
            rCode = 0;
        }
    }
    if( pLayer == NULL )
    {
        fprintf( stderr, "Can't find layer %s\n", lName );
    }
#ifdef KEADEBUG
    fprintf( stderr, "open layer returning %p\n", pLayer );
#endif
	return rCode;
}

long
keaLayerClose(void *lHandle)
{
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;
#ifdef KEADEBUG
    fprintf(stderr, "%s %s %p\n", __FUNCTION__, pLayer->sName.c_str(), pLayer );
#endif
    // do nothing since the layers are owned by the dataset
	return 0;
}

long
keaLayerRasterRead(void	*lHandle, unsigned long	bRow, unsigned long bCol, unsigned char	**pixels)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %ld %ld %p %p\n", __FUNCTION__, bRow, bCol, *pixels, lHandle);
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
    try
    {
        if( pLayer->bIsOverview )
        {
            //fprintf(stderr, "overview %d %d %s %d %d %d %d %d\n", (int)pLayer->eKEAType, pLayer->nOverview, pLayer->sName.c_str(),
            //         pLayer->nBlockSize * bCol, pLayer->nBlockSize * bRow, xsize, ysize, pLayer->nBlockSize );
            pLayer->pImageIO->readFromOverview( pLayer->nBand, pLayer->nOverview,
                                            (*pixels), pLayer->nBlockSize * bCol,
                                            pLayer->nBlockSize * bRow,
                                            xsize, ysize, pLayer->nBlockSize, pLayer->nBlockSize, 
                                            pLayer->eKEAType );
            //fprintf( stderr, "finished readFromOverview\n" );
            //memset(*pixels, 1, pLayer->nBlockSize * pLayer->nBlockSize * 2);
        }
        else if( pLayer->bIsMask )
        {
            // is int8 - set to 1
            // According to Haiyan Qu: "IMAGINE mask layer is a binary layer, 1 is data, 0 is nodata"
            if( pLayer->bMaskIsReal )
            {
                // read the mask out of the KEA file
                pLayer->pImageIO->readImageBlock2BandMask(pLayer->nBand, (*pixels), 
                                                pLayer->nBlockSize * bCol,
                                                pLayer->nBlockSize * bRow,
                                                xsize, ysize, pLayer->nBlockSize, pLayer->nBlockSize,
                                                pLayer->eKEAType ); // is uint8
                                                
                // KEA/GDAL mask layers are generally 255 where valid data
                // recode the mask we have just read
                for( uint64_t i = 0; i < (pLayer->nBlockSize*pLayer->nBlockSize); i++ )
                {
                    if( (*pixels)[i] > 0 )
                    {
                        (*pixels)[i] = 1;
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
            //fprintf(stderr, "non overview %d %s\n", (int)pLayer->eKEAType, pLayer->sName.c_str() );
            //fprintf( stderr, "readImageBlock2Band %d %d %d %d %d %d\n", (int)pLayer->nBand, (int)(pLayer->nBlockSize * bCol),
            //                        (int)(pLayer->nBlockSize * bRow), xsize, ysize, (int)pLayer->nBlockSize);
            pLayer->pImageIO->readImageBlock2Band( pLayer->nBand, (*pixels), pLayer->nBlockSize * bCol,
                                            pLayer->nBlockSize * bRow,
                                            xsize, ysize, pLayer->nBlockSize, pLayer->nBlockSize, 
                                            pLayer->eKEAType );
            //fprintf( stderr, "finished readImageBlock2Band\n" );
            //memset(*pixels, 1, pLayer->nBlockSize * pLayer->nBlockSize * 2);
        }
        rCode = 0;
    }
    catch (kealib::KEAIOException &e)
    {
        fprintf(stderr, "Exception in %s: %s\n", __FUNCTION__, e.what());
        rCode = -1;
    }
	return rCode;
}

long
keaLayerRasterWrite(void *lHandle, unsigned long bRow, unsigned long bCol, unsigned char *pixels)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %ld %ld %p %p\n", __FUNCTION__, bRow, bCol, *pixels, lHandle);
#endif
    return -1;
}

long
keaLayerLayerTypeRead(void  *lHandle, unsigned long  *lType)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p\n", __FUNCTION__, lHandle );
#endif
    long rCode = -1;
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;
    
    try
    {
        kealib::KEALayerType keaType = pLayer->pImageIO->getImageBandLayerType(pLayer->nBand);
        *lType = (unsigned long)keaType; // see keaInstanceLayerTypesGet
        rCode = 0;
    }
    catch (kealib::KEAIOException &e)
    {
        fprintf(stderr, "Exception in %s: %s\n", __FUNCTION__, e.what());
        rCode = -1;
    }

    return rCode;
}

long 
keaLayerRRDLayerNamesGet(void *lHandle, unsigned long  *count, char  ***layerNames, char  **algorithm)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p\n", __FUNCTION__, lHandle );
#endif
    Eerr_ErrorReport* err = NULL;
    long rCode = -1;
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;
    try
    {
        unsigned long layerCount = 0;
        // get the pointer back to the KEA_File object 
        // and iterate through the list of layers looking for overviews
        for( unsigned int n = 0; n < pLayer->pKEAFile->nLayers; n++ )
        {
            KEA_Layer *pCandidate = pLayer->pKEAFile->ppLayers[n];
            if( pCandidate->bIsOverview )
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
            for( unsigned int n = 0; n < pLayer->pKEAFile->nLayers; n++ )
            {
                KEA_Layer *pCandidate = pLayer->pKEAFile->ppLayers[n];
                if( pCandidate->bIsOverview )
                {
                    size_t nColonLoc = pCandidate->sName.find(':');
                    if( ( nColonLoc != std::string::npos ) && ( pCandidate->sName.compare(0, nColonLoc, pLayer->sName) == 0 ) )
                    {
                        //fprintf( stderr, "found matching RRD layer: %s\n", pCandidate->sName.c_str() );
                        // couldn't work out efnp_FileNodeCreate...
                        char *name = estr_Sprintf( NULL, (char*)"%s(:%s)", &err, 
                            pLayer->sFilePath.c_str(), pCandidate->sName.c_str(), NULL );
                        HANDLE_ERR(err, -1);
                        (*layerNames)[layerCount] = name;
                        //fprintf( stderr, "RRD: %s\n", name);
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
        fprintf(stderr, "Exception in %s: %s\n", __FUNCTION__, e.what());
        rCode = -1;
    }
    return rCode;
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
    fprintf(stderr, "%s %p\n", __FUNCTION__, lHandle );
#endif
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;
    long rCode = 0; // we are doing a count
    try
    {
        // have to get all the metadata in one go and iterate through
        std::vector< std::pair<std::string, std::string> > data;
        data = pLayer->pImageIO->getImageBandMetaData(pLayer->nBand);
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
        fprintf(stderr, "Exception in %s: %s\n", __FUNCTION__, e.what());
        rCode = -1;
    }
    return rCode;
}

long
keaLayerMapInfoRead(void *lHandle, char **projection, double *xULC, double *yULC, double *xPixelSize, double *yPixelSize, char **units)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p\n", __FUNCTION__, lHandle );
#endif
    long rCode = -1;
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;

    try
    {
        kealib::KEAImageSpatialInfo *pSpatialInfo = pLayer->pImageIO->getSpatialInfo();
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
        fprintf(stderr, "Exception in %s: %s\n", __FUNCTION__, e.what());
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
    fprintf(stderr, "%s %p\n", __FUNCTION__, lHandle );
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
keaLayerRasterNullValueRead(void  *lHandle, unsigned char  **pixel)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p\n", __FUNCTION__, lHandle );
#endif
    KEA_Layer *pLayer = (KEA_Layer*)lHandle;

    if( pLayer->bIsMask )
    {
        **pixel = 0; // according to Haiyan Qu: "IMAGINE mask layer is a binary layer, 1 is data, 0 is nodata"
    }
    else
    {
        try
        {
            // I *think* this will work - just treat it as the right type...
            pLayer->pImageIO->getNoDataValue(pLayer->nBand, (void*)(*pixel), pLayer->eKEAType);
        }
        catch (kealib::KEAIOException &e)
        {
            // throws exception when a no data has not been set so can't tell if an error
            *pixel = NULL;
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

    // set up some defaults
    pBinFn->numBins = 256;
    pBinFn->binFunctionType = EDSC_DIRECT_BINS;
    pBinFn->minLimit = 0;
    pBinFn->maxLimit = 255;
    pBinFn->binLimits = NULL;
    pBinFn->binFunctionLimitsType = EDSC_LIMITS_GIVEN;

    // now read through the metadata to get this info if present
    std::vector< std::pair<std::string, std::string> > data;
    data = pLayer->pImageIO->getImageBandMetaData(pLayer->nBand);
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

// the following don't seem to get called - RAT used instead
/*
long
keaLayerHistogramModTimeGet(void  *lHandle, time_t *modTime)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p\n", __FUNCTION__, lHandle );
#endif
    return -1;
}

long
keaLayerHistogramRead(void  *lHandle, long startRow, long numRows, long *histogram)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p %ld %ld\n", __FUNCTION__, lHandle, startRow, numRows );
#endif
    return -1;
}

long 
keaLayerColorRead(void *lHandle, char *colorName, long startRow, long numRows, double *colorTable)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p %s %ld %ld\n", __FUNCTION__, lHandle, colorName, startRow, numRows );
#endif
    return -1;
}

long
keaLayerColorModTimeGet(void *lHandle, char *colorName, time_t *modTime)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p %s\n", __FUNCTION__, lHandle, colorName);
#endif
    return -1;
}

long 
keaLayerOpacityRead(void *lHandle, long startRow, long numRows, double *opacity)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p %ld %ld\n", __FUNCTION__, lHandle, startRow, numRows);
#endif
    return -1;
}

long
keaLayerOpacityModTimeGet(void *lHandle, time_t *modTime)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p\n", __FUNCTION__, lHandle);
#endif
    return -1;
}

long 
keaLayerClassNamesRead(void *lHandle, long startRow, long numRows, char **classNames)
{
    fprintf(stderr, "%s %p\n", __FUNCTION__, lHandle);
    return -1;
}
*/
