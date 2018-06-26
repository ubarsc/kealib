/*
 *  keainstance.cpp
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
/*
********************************************************************************
Support for KEA file format within Imagine
********************************************************************************
*/

#include "kea.h"
#include "../include/libkea/kea-config.h"

extern "C" 
{
	// For some reason these aren't in the Imagine headers but provided 
	// by other Imagine DLL's.
	IFDExport long IFD_NAME(InstanceSupportsUnicode)(void);
	IFDExport long IFD_NAME(InstanceIsThreadsafe)(void);
}

long
keaInstanceTitleListGet(unsigned long *count, Etxt_Text **titleList )
{
#ifdef KEADEBUG
   keaDebugOut( "%s\n", __FUNCTION__ );
#endif
    *titleList = emsc_New(1, Etxt_Text );
    (*titleList)[0] = estr_Duplicate(ETXT_LTEXT("KEA File Format"));
    *count = 1;

	return 0;
}

long
keaInstanceTemplateListGet(Etxt_Text **templateList, Etxt_Text *templateListPseudoFlags)
{
#ifdef KEADEBUG
    keaDebugOut( "%s\n", __FUNCTION__ );
#endif
    *templateList = emsc_New(1, Etxt_Text );
    (*templateList)[0] = estr_Duplicate(ETXT_LTEXT("*.kea"));

	/*
	** None of these extensions are pseudo extensions, so we do not
	** need to set any flags
	*/
	*templateListPseudoFlags = (Etxt_Text)NULL;

	return 0;
}

long
keaInstanceExtListGet(Etxt_Text **extList)
{
#ifdef KEADEBUG
    keaDebugOut( "%s\n", __FUNCTION__ );
#endif
    *extList = emsc_New(1, Etxt_Text );
    (*extList)[0] = estr_Duplicate(ETXT_LTEXT(".kea"));

	return 0;
}

long
keaInstanceShortNameListGet(Etxt_Text **shortNameList)
{
#ifdef KEADEBUG
    keaDebugOut( "%s\n", __FUNCTION__ );
#endif
    *shortNameList = emsc_New(1, Etxt_Text );
    (*shortNameList)[0] = estr_Duplicate(ETXT_LTEXT("kea"));

	return 0;
}

long
keaInstanceFilterFlagsGet( char **flags )
{
#ifdef KEADEBUG
    keaDebugOut( "%s\n", __FUNCTION__ );
#endif
    *flags = emsc_New(1, char);
    (*flags)[0] = 1;
    return 0;
}

long
keaInstanceIsCreatableFlagsGet(char **flagsList)
{
#ifdef KEADEBUG
    keaDebugOut( "%s\n", __FUNCTION__ );
#endif
    *flagsList = emsc_New(1, char);
    (*flagsList)[0] = 1;
    return 0;
}

long
keaInstanceIsDirFlagsGet( char **flags )
{
#ifdef KEADEBUG
    keaDebugOut( "%s\n", __FUNCTION__ );
#endif
    *flags = emsc_New(1, char);
    (*flags)[0] = 0;
    return 0;
}

long
keaInstancePixelTypesGet(unsigned long *count, Etxt_Text **pixelTypes)
{
#ifdef KEADEBUG
    keaDebugOut( "%s\n", __FUNCTION__ );
#endif

    // just make a list where the indices match the KEADataType enum
    // because the first one is 'undefined' just pad it with something
    // we are in trouble if the enum ever changes...
    // also do the same for kea_64int and kea_64uint
    *count = kealib::kea_64float + 1;
    *pixelTypes = emsc_New(kealib::kea_64float + 1, Etxt_Text);
    (*pixelTypes)[0] = estr_Duplicate(ETXT_LTEXT("s8")); // kea_undefined
    (*pixelTypes)[1] = estr_Duplicate(ETXT_LTEXT("s8")); // kea_8int
    (*pixelTypes)[2] = estr_Duplicate(ETXT_LTEXT("s16")); // kea_16int
    (*pixelTypes)[3] = estr_Duplicate(ETXT_LTEXT("s32")); // kea_32int
    (*pixelTypes)[4] = estr_Duplicate(ETXT_LTEXT("s32")); // kea_64int - not supported
    (*pixelTypes)[5] = estr_Duplicate(ETXT_LTEXT("u8")); // kea_8uint
    (*pixelTypes)[6] = estr_Duplicate(ETXT_LTEXT("u16")); // kea_16uint
    (*pixelTypes)[7] = estr_Duplicate(ETXT_LTEXT("u32")); // kea_32uint
    (*pixelTypes)[8] = estr_Duplicate(ETXT_LTEXT("u32")); // kea_64uint - not supported
    (*pixelTypes)[9] = estr_Duplicate(ETXT_LTEXT("f32")); //  kea_32float
    (*pixelTypes)[10] = estr_Duplicate(ETXT_LTEXT("f64")); // kea_64float

    return 0;
}

long
keaInstanceCompressionTypesGet(unsigned long *count, Etxt_Text  **compressionTypes)
{
#ifdef KEADEBUG
    keaDebugOut( "%s\n", __FUNCTION__ );
#endif
    // we just support one type - zlib. Note that this is just for display
    // data is expected to be uncompressed by the time Imagine sees it
    *count = 1;
    *compressionTypes = emsc_New(1, Etxt_Text);
    (*compressionTypes)[0] = estr_Duplicate(ETXT_LTEXT("zlib"));

    return 0;
}

long
keaInstanceLayerTypesGet( unsigned long  *count,  Etxt_Text  **layerTypes)
{
#ifdef KEADEBUG
    keaDebugOut( "%s\n", __FUNCTION__ );
#endif
    // make it match the KEALayerType enum
    *count = 2;
    *layerTypes = emsc_New(2, Etxt_Text);
    (*layerTypes)[0] = estr_Duplicate(ETXT_LTEXT("athematic"));
    (*layerTypes)[1] = estr_Duplicate(ETXT_LTEXT("thematic"));

    return 0;
}

long
keaInstanceColumnTypesGet(unsigned long *count, Etxt_Text **columnTypes)
{
#ifdef KEADEBUG
    keaDebugOut( "%s\n", __FUNCTION__ );
#endif
    // make it match KEAFieldDataType enum
    *count = kealib::kea_att_string + 1;
    *columnTypes = emsc_New(kealib::kea_att_string + 1, Etxt_Text);
    (*columnTypes)[0] = estr_Duplicate(ETXT_LTEXT("integer")); // kea_att_na
    (*columnTypes)[1] = estr_Duplicate(ETXT_LTEXT("integer")); // kea_att_bool - treat as int
    (*columnTypes)[2] = estr_Duplicate(ETXT_LTEXT("integer")); // kea_att_int
    (*columnTypes)[3] = estr_Duplicate(ETXT_LTEXT("real")); // kea_att_float
    (*columnTypes)[4] = estr_Duplicate(ETXT_LTEXT("string")); // kea_att_string

    return 0;
}

long
keaInstanceRasterDataOrderTypesGet(unsigned long  *count, Etxt_Text  **rdoTypes, unsigned char  **rdoWriteFlags)
{     
#ifdef KEADEBUG
    keaDebugOut( "%s\n", __FUNCTION__ );
#endif
    /* If this isn't implemented we end up with BIK (??) in the file info */
    *count = 1;
    *rdoTypes = emsc_New(1, Etxt_Text);
    (*rdoTypes)[0] = estr_Duplicate(ETXT_LTEXT("BSQ"));
    *rdoWriteFlags = NULL;
    
    return 0;
}

long
keaInstanceDescriptionGet(Etxt_Text *description)
{
#ifdef KEADEBUG
    keaDebugOut( "%s\n", __FUNCTION__ );
#endif
    Eerr_ErrorReport* err = NULL;
    *description = estr_Sprintf(NULL, ETXT_LTEXT("KEA Raster Support Library Version %s"), &err,
            ETXT_LTEXT(LIBKEA_VERSION), NULL);
    HANDLE_ERR(err, -1);
    return 0;
}

// This is a weird one - the doco says this isn't required, yet with 
// Imagine 2015 is still asks for the :Mask of each band and gives
// and error when it fails.

// Only alternative was to implement the Mask band as we have done.

long 
keaInstanceSupportsMasks(unsigned char **flags)
{
#ifdef KEADEBUG
    keaDebugOut( "%s\n", __FUNCTION__ );
#endif
    *flags = emsc_New(1, unsigned char);
    (*flags)[0] = 1;
    return 0; /* success in Imagine land usually *
                /* The doco says this is a void function which doesn't match the header */
}

// not strictly necessary I guess but the Imagine format DLL's export this
// for some reason we have to mark as exportable since they aren't in the headers
long keaInstanceSupportsUnicode(void)
{
#ifdef KEADEBUG
    keaDebugOut( "%s\n", __FUNCTION__ );
#endif

#ifdef UNICODE
	return 1;
#else
    return 0;
#endif
}

long keaInstanceIsThreadsafe(void)
{
#ifdef KEADEBUG
    keaDebugOut( "%s\n", __FUNCTION__ );
#endif
	return 0;
}

long
keaInstanceMapProjectionIsSupported(long  rfTitle, Etxt_Text  projTitle, unsigned char  *MIFproj,
unsigned long  MIFprojSize, Etxt_Text  MIFprojDictionary, Etxt_Text  MIFprojName, unsigned char  *MIFearthModel, 
unsigned long  MIFearthModelSize, Etxt_Text MIFearthModelDictionary, Etxt_Text MIFearthModelName )
{
#ifdef KEADEBUG
    keaDebugOut( "%s %s\n", __FUNCTION__, projTitle );
#endif
    return 1;
}

#ifdef WIN32
    #define snprintf _snprintf
#endif

void keaDebugOut(char *fmt, ...)
{
  va_list args;
    FILE *fh;
    char *tmpDir, *fName;
    int nSize, nPid;
    
    tmpDir = getenv("TMP");
    if( tmpDir == NULL )
    {
#ifdef WIN32
        tmpDir = "C:";
#else
        tmpDir = "/tmp";
#endif    
    }

#ifdef WIN32
    nPid = GetCurrentProcessId();
#else
    nPid = getpid();
#endif    
    
    nSize = snprintf(NULL, 0, "%s/kea_%d.log", tmpDir, nPid);
    fName = (char*)malloc(nSize+1);
    snprintf(fName, nSize+1, "%s/kea_%d.log", tmpDir, nPid);
    fh = fopen(fName, "a");
    if( fh == NULL )
    {
        fprintf(stderr, "Failed to open %s for writing\n", fName);
    }
    else
    {
        va_start(args, fmt);
        vfprintf(fh, fmt, args);
        va_end(args);
    }
    fflush(fh);
    fclose(fh);
    free(fName);
}
