/*
 *  kea.h
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
#ifndef _KEA_H_
#define _KEA_H_

#include <limits.h>
#include <time.h>
#include <sys/types.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <algorithm>

#ifdef _MSC_VER
    // because __STDC__ is defined apparently - hdf5 was compiled without it.
	typedef _off_t off_t;
#endif

#include "libkea/KEACommon.h"
#include "libkea/KEAImageIO.h"
#include "libkea/KEAAttributeTable.h"

#include <eerr.h>
#include <efio.h>
#include <efnp.h>
#include <egda.h>
#include <eimg.h>
#include <eint.h>
#include <emif.h>
#include <emsc.h>
#include <eprj.h>
#include <estr.h>
#include <emif.h>
#include <estr_regex.h>

// needs to be defined down here as it is first set to something
// else in one of the earlier headers
#define	DLL_INSTANCE_NAME	kea

#ifdef _MSC_VER
	// hack to get the descriptor table exported on Windows
	// not sure if this affects all versions 
	#define EDSC_API DllExport
#endif

#include <eimg_RasterFormats.h>

class KEA_Layer; 

class KEA_File
{
public:
    kealib::KEAImageIO *pImageIO;
    std::string       sFilePath;
    unsigned int     nLayers;
    Eprj_MapProjection* pProj;
    std::string sUnits;
    std::string sProjName;
    time_t  modTime;    // maybe should have whole _stat struct here, not sure.
                        // Put it here for keaDataModTimeGet
    // all the layers and overviews mixed together
    KEA_Layer **ppLayers;
};

class KEA_Layer
{
public:
    kealib::KEAImageIO *pImageIO;
    KEA_File           *pKEAFile; 
    std::string         sName;
    std::string         sFilePath;
	unsigned int		nBand;

    bool                bIsOverview;
    bool                bIsMask;
    unsigned int        nOverview; // if bIsOverview == true

    unsigned int        nXSize;
    unsigned int        nYSize;
    unsigned int        nBlockSize;
    kealib::KEADataType         eKEAType;
};

class KEA_Column
{
public:
    kealib::KEAAttributeTable      *pKEATable;
    size_t nColIdx;
    kealib::KEAFieldDataType    eType;
    bool                        bTreatIntAsFloat; // for colours - Imagine expects 0-1
};

inline void keaPrintErrorReport(Eerr_ErrorReport *err, const char *fnname)
{
    if( err != NULL )
    {
        fprintf( stderr, "Error in %s: %s: %s\n", fnname, err->functionname, err->message );
    }
}

#define HANDLE_ERR(err, retval) if( err != NULL ){keaPrintErrorReport(err, __FUNCTION__); eerr_DeleteErrorReport(err); return retval;}

// so it can be accessed from keafile.cpp
Edsc_BinFunction*
keaLayerGetHistoBinFunction(KEA_Layer *pLayer);

#endif //_KEA_H_

