/*
 *  keafile.cpp
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
#include "keaproj.h"

#include <sys/types.h>
#include <sys/stat.h>

bool IsSupportedDataType(kealib::KEAImageIO *pImageIO, uint32_t nBand)
{
    // Imagine does not support the 64 bit int type
    kealib::KEADataType type = pImageIO->getImageBandDataType(nBand);
    if( ( type == kealib::kea_64int ) || ( type == kealib::kea_64uint ) )
    {
        return false;
    }
    return true;
}

time_t getModifiedTime(char *fileName)
{
    time_t modTime = 0;
    // Uses stat call but syntax is slightly different 
    // Unix vs Windows
#ifdef WIN32
    struct _stat buf;
    if( _stat(fileName, &buf) == 0)
    {
        modTime = buf.st_mtime;
    }
#else
    struct stat buf;
    if( stat(filename, &buf) == 0 )
    {
        modTime = buf.st_mtime;
    }
#endif    
    return modTime;
}

void *
keaFileTitleIdentifyAndOpen(char *fileName, long *fileType, char *inFileMode)
{
#ifdef KEADEBUG
    if( inFileMode == NULL )
    {
        fprintf(stderr, "%s %s %ld NULL\n", __FUNCTION__, fileName, *fileType, inFileMode);
    }
    else
    {
        fprintf(stderr, "%s %s %ld %s\n", __FUNCTION__, fileName, *fileType, inFileMode);
    }
#endif
    Eerr_ErrorReport* err = NULL;
    // don't support writing yet
    if( ( inFileMode != NULL ) && ( EFIO_MODE_CREATE( inFileMode ) == EMSC_TRUE ) )
    {
        fprintf( stderr, "attempted to open in write mode\n" );
        return NULL;
    }

    KEA_File  *pKEAFile = NULL;
    bool isKEA = false;
    try
    {
        // is this a KEA file?
        isKEA = kealib::KEAImageIO::isKEAImage( fileName );
    }
    catch (kealib::KEAIOException &e)
    {
        fprintf(stderr, "Exception in %s: %s\n", __FUNCTION__, e.what());
        isKEA = false;
    }

    if( isKEA )
    {
        *fileType = 0;
        // I *think* they only want us to open when inFileMode != NULL....
        if( inFileMode != NULL ) 
        {
            H5::H5File *pH5File = NULL;
            kealib::KEAImageIO *pImageIO = NULL;
            try
            {
                pH5File = kealib::KEAImageIO::openKeaH5RDOnly( fileName );

                pImageIO = new kealib::KEAImageIO();
                pImageIO->openKEAImageHeader( pH5File );

                pKEAFile = new KEA_File();
                pKEAFile->pImageIO = pImageIO;
                pKEAFile->sFilePath = fileName;
                kealib::KEAImageSpatialInfo *pSpatialInfo = pImageIO->getSpatialInfo();
                // turn from WKT into someting Imagie understands
                pKEAFile->pProj = WKTToMapProj(pSpatialInfo->wktString.c_str(), pKEAFile->sProjName, pKEAFile->sUnits);
                pKEAFile->modTime = getModifiedTime(fileName); // for keaFileDataModTimeGet
                pKEAFile->nLayers = 0;
                // count how many layers in total
                uint32_t nBands = pImageIO->getNumOfImageBands();
                for( uint32_t n = 0; n < nBands; n++ )
                {
                    if( IsSupportedDataType( pImageIO, n+1 ) )
                    {
                        pKEAFile->nLayers += 2;  // one for layer, one for mask
                        pKEAFile->nLayers += pImageIO->getNumOfOverviews(n+1);
                    }
                    else
                    {
                        fprintf( stderr, "Band %d has type unsupported by Imagine\n", n );
                    }
                }
                if( pKEAFile->nLayers > 0 )
                {
                    // allocate the space
                    pKEAFile->ppLayers = (KEA_Layer**)calloc(pKEAFile->nLayers, sizeof(KEA_Layer*));
                    uint32_t nIndex = 0;
                    for( uint32_t n = 0; n < nBands; n++ )
                    {
                        uint32_t nBand = n + 1;
                        if( !IsSupportedDataType( pImageIO, nBand ) )
                            continue;
                        KEA_Layer *pLayer = new KEA_Layer();
                        pLayer->pImageIO = pImageIO;
                        pLayer->pKEAFile = pKEAFile;
                        pLayer->sName = pImageIO->getImageBandDescription(nBand);
                        pLayer->sFilePath = pKEAFile->sFilePath;
                        pLayer->nBand = nBand;
                        pLayer->bIsOverview = false;
                        pLayer->bIsMask = false;
                        pLayer->bMaskIsReal = false;
                        pLayer->nOverview = 99999;
                        pLayer->eKEAType = pImageIO->getImageBandDataType(nBand);
                        pLayer->nXSize = pSpatialInfo->xSize;
                        pLayer->nYSize = pSpatialInfo->ySize;
                        pLayer->nBlockSize = pImageIO->getImageBlockSize(nBand);
                        //fprintf( stderr, "Adding Band %s blocksize %d\n", pLayer->sName.c_str(), pLayer->nBlockSize);
                        pKEAFile->ppLayers[nIndex] = pLayer;
                        nIndex++;
                        
                        // mask
                        KEA_Layer *pMask = new KEA_Layer();
                        pMask->pImageIO = pImageIO;
                        pMask->pKEAFile = pKEAFile;
                        char *name = estr_Sprintf( NULL, (char*)"%s:Mask", &err, 
                                    pLayer->sName.c_str(), NULL );
                        pMask->sName = name;
                        pMask->sFilePath = pKEAFile->sFilePath;
                        pMask->nBand = nBand;
                        pMask->bIsOverview = false;
                        pMask->bIsMask = true;
                        pMask->bMaskIsReal = pImageIO->maskCreated(nBand);
                        pMask->nOverview = 99999;
                        pMask->eKEAType = kealib::kea_8uint;
                        pMask->nXSize = pSpatialInfo->xSize;
                        pMask->nYSize = pSpatialInfo->ySize;
                        pMask->nBlockSize = pImageIO->getImageBlockSize(nBand);
                        //fprintf( stderr, "Adding Band %s blocksize %d\n", pLayer->sName.c_str(), pLayer->nBlockSize);
                        pKEAFile->ppLayers[nIndex] = pMask;
                        nIndex++;

                        // do the overviews
                        unsigned int nOverViews = pImageIO->getNumOfOverviews(nBand);
                        for(unsigned int o = 0; o < nOverViews; o++ )
                        {
                            unsigned int nOverview = o + 1;
                            KEA_Layer *pOverview = new KEA_Layer();
                            pOverview->pImageIO = pImageIO;
                            pOverview->pKEAFile = pKEAFile;
                            char *name = estr_Sprintf( NULL, (char*)"%s:Overview_%d", &err, 
                                    pLayer->sName.c_str(), nOverview, NULL );
                            HANDLE_ERR(err, NULL);
                            pOverview->sName = name;
                            emsc_Free(name);
                            pOverview->sFilePath = pKEAFile->sFilePath;
                            pOverview->nBand = nBand;
                            pOverview->bIsOverview = true;
                            pOverview->bIsMask = false;
                            pOverview->bMaskIsReal = false;
                            pOverview->nOverview = nOverview;
                            pOverview->eKEAType = pLayer->eKEAType;
                            uint64_t xsize, ysize;
                            pImageIO->getOverviewSize(nBand, nOverview, &xsize, &ysize);
                            pOverview->nXSize = xsize;
                            pOverview->nYSize = ysize;
                            pOverview->nBlockSize = pImageIO->getOverviewBlockSize(nBand, nOverview);
                            //fprintf( stderr, "Adding overview %s %d\n", pOverview->sName.c_str(), pOverview->nBlockSize );
                            pKEAFile->ppLayers[nIndex] = pOverview;
                            nIndex++;
                        }
                    }
                }
                else
                {
                    pKEAFile->ppLayers = NULL;
                }
            }
            catch (kealib::KEAIOException &e)
            {
#ifdef KEADEBUG
                fprintf( stderr, "Error during opening %s: %s\n", fileName, e.what());
#endif
                // was a problem - can't be a valid file
                delete pImageIO;
                delete pKEAFile;
                pKEAFile = NULL;
                pImageIO = NULL;
            }
        }
    }
#ifdef KEADEBUG
    fprintf( stderr, "open returning %p\n", pKEAFile);
#endif

	return pKEAFile;
}

long
keaFileClose(void *fileHandle)
{
    KEA_File *pKEAFile = (KEA_File*)fileHandle;
#ifdef KEADEBUG
    fprintf(stderr, "%s %s %p\n", __FUNCTION__, pKEAFile->sFilePath.c_str(), pKEAFile );
#endif

    try
    {
        pKEAFile->pImageIO->close();
    }
    catch (kealib::KEAIOException &e)
    {
        fprintf(stderr, "Exception in %s: %s\n", __FUNCTION__, e.what());
    }
    delete pKEAFile->pImageIO;
    pKEAFile->pImageIO = NULL;
    for( unsigned int n = 0; n < pKEAFile->nLayers; n++ )
    {
        pKEAFile->ppLayers[n]->pKEAFile = NULL;
        delete pKEAFile->ppLayers[n];
        pKEAFile->ppLayers[n] = NULL;
    }
    free(pKEAFile->ppLayers);
    pKEAFile->ppLayers = NULL;

    if( pKEAFile->pProj != NULL )
    {
        eprj_ProjectionFree(&pKEAFile->pProj);
    }
    delete pKEAFile;
    pKEAFile = NULL;
	return 0;
}

long
keaFileLayerNamesGet(void *fileHandle, unsigned long *count, char ***layerNames)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p\n", __FUNCTION__, fileHandle );
#endif
    KEA_File *pKEAFile = (KEA_File*)fileHandle;

    long rCode = -1;
    unsigned long layerCount = 0;

    // work out how many we have
    for( unsigned int n = 0; n < pKEAFile->nLayers; n++ )
    {
        KEA_Layer *pCandidate = pKEAFile->ppLayers[n];
        if( !pCandidate->bIsOverview )
        {
            layerCount++;
        }
    }

    if( layerCount > 0 )
    {
        *count = layerCount;
        *layerNames = emsc_New(layerCount, char *);
        layerCount = 0;

        for( unsigned int n = 0; n < pKEAFile->nLayers; n++ )
        {
            KEA_Layer *pCandidate = pKEAFile->ppLayers[n];
            if( !pCandidate->bIsOverview )
            {
                (*layerNames)[layerCount] = estr_Duplicate((char*)pCandidate->sName.c_str());
                layerCount++;
            }
        }
    }
    else
    {
        *count = 0;
        *layerNames = NULL;
    }
    rCode = 0;

	return rCode;
}

long
keaFileDataRead(void *fileHandle, char *dataName, unsigned char **MIFDataObject,
        unsigned long *MIFDataSize, char **MIFDataDictionary, char **MIFDataType)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %s\n", __FUNCTION__, dataName );
#endif
    Eerr_ErrorReport *err = NULL;

    *MIFDataObject = NULL;
    *MIFDataDictionary = NULL;
    *MIFDataType = NULL;
    *MIFDataSize = 0;

    // currently we only support getting the histogram bin function in this manner
    // will be in the form:
    // :LayerName:OverviewName:Descriptor_Table:#Bin_Function#
    char *pszNameCopy = estr_Duplicate(dataName);
    char *pszLastColon = strrchr(pszNameCopy, ':');
    // is it looking for the bin function?
    if( ( pszLastColon != NULL ) &&  ( strcmp(pszLastColon+1, "#Bin_Function#") == 0 ) )
    {
        //fprintf( stderr, "Found #Bin_Function# %s\n", pszNameCopy );
        *pszLastColon = '\0'; // put a null there and look at the next one back
        pszLastColon = strrchr(pszNameCopy, ':');
        if( ( pszLastColon != NULL ) && (strcmp(pszLastColon+1, "Descriptor_Table" ) == 0 ) )
        {
            //fprintf( stderr, "Found Descriptor_Table\n" );
            // now find the second colon
            char *pszSecondColon = strchr(&pszNameCopy[1], ':');
            if( pszSecondColon != NULL )
            {
                *pszSecondColon = '\0';
                char *pszLayerName = &pszNameCopy[1];
                //fprintf( stderr, "looking for layer %s\n", pszLayerName );

                unsigned long dtype, width, height, bWidth, bHeight, compression;
                KEA_Layer *pKEALayer;
                if( keaLayerOpen(fileHandle, pszLayerName, &dtype, &width, &height, 
                            &compression, &bWidth, &bHeight, (void**)&pKEALayer) == 0 )
                {
                    //fprintf( stderr, "Found layer\n" );
                    Edsc_BinFunction *pBinFn = keaLayerGetHistoBinFunction(pKEALayer);
                    if( pBinFn != NULL )
                    {
                        //fprintf( stderr, "Found bin function\n" );
                        // make 'MIFable'
                        EMIF_CADDR *pMIFableObject;
                        Emif_Design *pDesign;
                        edsc_BinFunctionConvertToMIFable(pBinFn, (void**)&pMIFableObject, &pDesign, &err);
                        HANDLE_ERR(err, -1)
    
                        // was planning to use emif_MIFableObjectConvertToMIF, but
                        // only available post 8.4. Found "Gemetric Models Example" on developer.lggi.com
                        // which helped work out how to use the older stuff
                        //*MIFDataSize = emif_MIFableObjectConvertToMIF(pMIFableObject, pDesign, MIFDataObject,
                        //        MIFDataDictionary, MIFDataType, &err);
                        //HANDLE_ERR(err)
                        Emif_Dictionary *dictionary = emif_DictionaryCreate(&err);
                        HANDLE_ERR(err, -1)

                        emif_DictionaryAddDesign(dictionary, pDesign, &err);
                        HANDLE_ERR(err, -1)

                        *MIFDataType = estr_Duplicate(pDesign->name);
                        *MIFDataDictionary = emif_DictionaryEncodeToString(dictionary, &err);
                        HANDLE_ERR(err, -1)

                        *MIFDataSize = emif_ObjectSize(pMIFableObject, pDesign, &err);
                        HANDLE_ERR(err, -1)

                        *MIFDataObject = emsc_New(sizeof(unsigned char) * (*MIFDataSize), unsigned char);
                        emif_ConvertToMIF(NULL, pMIFableObject, pDesign, *MIFDataObject, &err);
                        HANDLE_ERR(err, -1)

                        emif_ObjectFree(&pMIFableObject, pDesign, &err);
                        HANDLE_ERR(err, -1)
                        emif_DictionaryDestroy(dictionary);
                        emif_DesignDestroy(pDesign);
                        emsc_Free(pBinFn);
                    }
                    // does nothing, but for completeness
                    keaLayerClose(pKEALayer);
                }
            }
        }
    }
    emsc_Free(pszNameCopy);

    return 0;
}

long
keaFileDataModTimeGet(void *fileHandle, char *dataName, time_t *lastModTime)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p %s\n", __FUNCTION__, fileHandle, dataName );
#endif
    /* In theory we could extract the mod time of each HDF5 object */
    /* But for now we just return the modified time of the file */
    KEA_File *pKEAFile = (KEA_File*)fileHandle;
    *lastModTime = pKEAFile->modTime;

    return 0;
}

long 
keaFileRasterDataOrderGet(void  *fileHandle, unsigned long  *order)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %p\n", __FUNCTION__, fileHandle );
#endif
    // always BSQ - see keaInstanceRasterDataOrderTypesGet
    *order = 0;
    return 0;
}

// Don't need to implement the following. Yes I know keaFileModeGet
// might be a way to get around the file size limit in the open dialog
// in early versions of Imagine - but it was only added to Imagine later

// Imagine uses System calls if these don't exist
/*
extern "C" long keaFileModeGet(char  *fileName, mode_t  *mode);

long
keaFileModeGet(char  *fileName, mode_t  *mode)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %s\n", __FUNCTION__, fileName );
#endif

    return -1;
}

long
keaFileModTimeGet(char *fileName, time_t *modTime)
{
#ifdef KEADEBUG
    fprintf(stderr, "%s %s\n", __FUNCTION__, fileName );
#endif

    return -1;
}
*/
