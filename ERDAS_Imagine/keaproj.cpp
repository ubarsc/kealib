/*
 *  keaproj.cpp
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
#include <stdio.h>
#include <algorithm>
#include "keaproj.h"

// from GDAL
#include "cpl_port.h"
#include "cpl_conv.h"
#include "ogr_spatialref.h"
#include "ogr_srs_api.h"

Eprj_MapProjection* WKTToMapProj(const char *pszProj, etxt::tstring &sProjName, etxt::tstring &sUnits)
{
    Eerr_ErrorReport* err = NULL;
	ETXT_CONVERSION;
	
	OGRSpatialReference sr;
	char *pszP = const_cast<char*>(pszProj);
	sr.importFromWkt(&pszP);
	sr.morphToESRI();

	char *pszPEString = NULL;
	sr.exportToWkt(&pszPEString);
	
	Eprj_MapProjection *proj = NULL;
	Etxt_Text unitName;
	eprj_MapProjectionFromPEString(ETXT_2U(pszPEString), &proj, &unitName, &err);
	HANDLE_ERR(err, NULL)
	
	CPLFree(pszPEString);
	
	sUnits = unitName;
	sProjName = eprj_MapProjectionName(proj);
	
	if( sProjName == ETXT_LTEXT("New_Zealand_Map_Grid") )
	{
		// Imagine doesn't seem to have this right...
		eprj_ProjectionFree(&proj);
		Eprj_ProParameters *proParams = eprj_ProParametersCreate(&err);
		HANDLE_ERR(err, NULL)
		
		// below taken from GDAL's hfadataset.cpp
		proParams->proType = EPRJ_EXTERNAL;
		proParams->proNumber = 0;
        eprj_CharStrCreate(ETXT_LTEXT("nzmg"), &proParams->proExeName, &err);
        HANDLE_ERR(err, NULL)
        eprj_CharStrCreate(ETXT_LTEXT("New Zealand Map Grid"), &proParams->proName, &err);
        HANDLE_ERR(err, NULL)
        proParams->proZone = 0;
		// note: hfadataset code originally had 8 params, but I now get a 
		// relative/absolute error coming up. Creating 9 params seems to fix it.
		eprj_DblArrayCreate(9, &proParams->proParams, &err);
		HANDLE_ERR(err, NULL)
        proParams->proParams.data[0] = 0;  // False easting etc not stored in ->img it seems
        proParams->proParams.data[1] = 0;  // always fixed by definition->
        proParams->proParams.data[2] = 0;
        proParams->proParams.data[3] = 0;
        proParams->proParams.data[4] = 0;
        proParams->proParams.data[5] = 0;
        proParams->proParams.data[6] = 0;
        proParams->proParams.data[7] = 0;
        proParams->proParams.data[8] = 0;
		
		// need to get this or we can't find spheroids etc
		Eint_InitToolkitData* init = eint_GetInit();
		
		eprj_SpheroidByName(init, ETXT_LTEXT("International 1909"), proParams->proSpheroid, &err);
        HANDLE_ERR(err, NULL)

		eprj_DatumByName(init, ETXT_LTEXT("International 1909"), ETXT_LTEXT("Geodetic Datum 1949"), proParams->proSpheroid->datum, &err);
        HANDLE_ERR(err, NULL)
					
        proj = eprj_ProjectionInit(init, proParams, &err);
        HANDLE_ERR(err, NULL)
		
		// seems eprj_ProjectionInit takes a copy
        eprj_ProParametersFree(&proParams);
		
		sProjName = eprj_MapProjectionName(proj);
	}
	else if( (sProjName == ETXT_LTEXT("NZGD_2000_New_Zealand_Transverse_Mercator") ) ||
				(sProjName == ETXT_LTEXT("NZGD2000_New_Zealand_Transverse_Mercator_2000")) )
	{
		// this one doesn't seem to be right either
		eprj_ProjectionFree(&proj);
		Eprj_ProParameters *proParams = eprj_ProParametersCreate(&err);
		HANDLE_ERR(err, NULL)
		
		proParams->proType = EPRJ_INTERNAL;
		proParams->proNumber = 9;
        eprj_CharStrCreate(ETXT_LTEXT("Transverse Mercator"), &proParams->proName, &err);
        HANDLE_ERR(err, NULL)
        proParams->proZone = 0;
		eprj_DblArrayCreate(8, &proParams->proParams, &err);
		HANDLE_ERR(err, NULL)
        proParams->proParams.data[0] = 0;  
        proParams->proParams.data[1] = 0;  
        proParams->proParams.data[2] = 0.99960;
        proParams->proParams.data[3] = 0;
        proParams->proParams.data[4] = 173.0;
        proParams->proParams.data[5] = 0;
        proParams->proParams.data[6] = 1600000;
        proParams->proParams.data[7] = 10000000;
		
		// need to get this or we can't find spheroids etc
		Eint_InitToolkitData* init = eint_GetInit();
		
		eprj_SpheroidByName(init, ETXT_LTEXT("GRS 1980"), proParams->proSpheroid, &err);
        HANDLE_ERR(err, NULL)

		eprj_DatumByName(init, ETXT_LTEXT("GRS 1980"), ETXT_LTEXT("NZGD2000 (NTv2)"), proParams->proSpheroid->datum, &err);
        HANDLE_ERR(err, NULL)

        proj = eprj_ProjectionInit(init, proParams, &err);
        HANDLE_ERR(err, NULL)

		// seems eprj_ProjectionInit takes a copy
        eprj_ProParametersFree(&proParams);
		
		sProjName = eprj_MapProjectionName(proj);
	}
	
    return proj;
}


std::string MapProjToWKT( Eprj_MapProjection *proj,
                   etxt::tstring &sUnits, etxt::tstring &sProjName )
{
    Eerr_ErrorReport* err = NULL;
	ETXT_CONVERSION;

	Etxt_Text pszPEString = NULL;
	eprj_MapProjectionToPEString(&pszPEString, proj, const_cast<Etxt_Text>(sUnits.c_str()), &err);
	HANDLE_ERR(err, "")
	
	OGRSpatialReference sr;
	char *pszP = ETXT_2A(pszPEString);
	sr.importFromESRI(&pszP);
	
	char *pszWKT = NULL;
	sr.exportToWkt(&pszWKT);
	std::string sWKT = pszWKT;
	
	CPLFree(pszWKT);
	emsc_Free(pszPEString);
	
	return sWKT;
}					   
