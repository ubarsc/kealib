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
