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
#ifdef __SUNPRO_CC
    // to get strcasecmp on Solaris
    #define __EXTENSIONS__
#endif
#include <string.h>
#include <vector>
#include <string>
#include <algorithm>
#include "keaproj.h"

// from GDAL
#include "cpl_port.h"
#include "ogr_core.h"
#include "cpl_conv.h"

#include "ogr_spatialref.h"
#include "ogr_srs_api.h"

#define stricmp _stricmp
#define strnicmp _strnicmp

// from hfa.h
/* -------------------------------------------------------------------- */
/*      Projection codes.                                               */
/* -------------------------------------------------------------------- */
#define EPRJ_LATLONG                0
#define EPRJ_UTM                1
#define EPRJ_STATE_PLANE            2
#define EPRJ_ALBERS_CONIC_EQUAL_AREA        3
#define EPRJ_LAMBERT_CONFORMAL_CONIC            4
#define EPRJ_MERCATOR                           5
#define EPRJ_POLAR_STEREOGRAPHIC                6
#define EPRJ_POLYCONIC                          7
#define EPRJ_EQUIDISTANT_CONIC                  8
#define EPRJ_TRANSVERSE_MERCATOR                9
#define EPRJ_STEREOGRAPHIC                      10
#define EPRJ_LAMBERT_AZIMUTHAL_EQUAL_AREA       11
#define EPRJ_AZIMUTHAL_EQUIDISTANT              12
#define EPRJ_GNOMONIC                           13
#define EPRJ_ORTHOGRAPHIC                       14
#define EPRJ_GENERAL_VERTICAL_NEAR_SIDE_PERSPECTIVE 15
#define EPRJ_SINUSOIDAL                         16
#define EPRJ_EQUIRECTANGULAR                    17
#define EPRJ_MILLER_CYLINDRICAL                 18
#define EPRJ_VANDERGRINTEN                      19
#define EPRJ_HOTINE_OBLIQUE_MERCATOR            20
#define EPRJ_SPACE_OBLIQUE_MERCATOR             21
#define EPRJ_MODIFIED_TRANSVERSE_MERCATOR       22
#define EPRJ_EOSAT_SOM                          23
#define EPRJ_ROBINSON                           24
#define EPRJ_SOM_A_AND_B                        25
#define EPRJ_ALASKA_CONFORMAL                   26
#define EPRJ_INTERRUPTED_GOODE_HOMOLOSINE       27
#define EPRJ_MOLLWEIDE                          28
#define EPRJ_INTERRUPTED_MOLLWEIDE              29
#define EPRJ_HAMMER                             30
#define EPRJ_WAGNER_IV                          31
#define EPRJ_WAGNER_VII                         32
#define EPRJ_OBLATED_EQUAL_AREA                 33
#define EPRJ_PLATE_CARREE                       34
#define EPRJ_EQUIDISTANT_CYLINDRICAL            35
#define EPRJ_GAUSS_KRUGER                       36
#define EPRJ_ECKERT_VI                          37
#define EPRJ_ECKERT_V                           38
#define EPRJ_ECKERT_IV                          39
#define EPRJ_ECKERT_III                         40
#define EPRJ_ECKERT_II                          41
#define EPRJ_ECKERT_I                           42
#define EPRJ_GALL_STEREOGRAPHIC                 43
#define EPRJ_BEHRMANN                           44
#define EPRJ_WINKEL_I                           45
#define EPRJ_WINKEL_II                          46
#define EPRJ_QUARTIC_AUTHALIC                   47
#define EPRJ_LOXIMUTHAL                         48
#define EPRJ_BONNE                              49
#define EPRJ_STEREOGRAPHIC_EXTENDED             50
#define EPRJ_CASSINI                            51
#define EPRJ_TWO_POINT_EQUIDISTANT              52
#define EPRJ_ANCHORED_LSR                       53
#define EPRJ_KROVAK                             54
#define EPRJ_DOUBLE_STEREOGRAPHIC               55
#define EPRJ_AITOFF                             56
#define EPRJ_CRASTER_PARABOLIC                  57
#define EPRJ_CYLINDRICAL_EQUAL_AREA             58
#define EPRJ_FLAT_POLAR_QUARTIC                 59
#define EPRJ_TIMES                              60
#define EPRJ_WINKEL_TRIPEL                      61
#define EPRJ_HAMMER_AITOFF                      62
#define EPRJ_VERTICAL_NEAR_SIDE_PERSPECTIVE     63
#define EPRJ_HOTINE_OBLIQUE_MERCATOR_AZIMUTH_CENTER           64
#define EPRJ_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_CENTER         65
#define EPRJ_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_NATURAL_ORIGIN 66
#define EPRJ_LAMBERT_CONFORMAL_CONIC_1SP        67
#define EPRJ_PSEUDO_MERCATOR                    68
#define EPRJ_MERCATOR_VARIANT_A                 69
#define EPRJ_HOTINE_OBLIQUE_MERCATOR_VARIANT_A                70
#define EPRJ_TRANSVERSE_MERCATOR_SOUTH_ORIENTATED             71

#define EPRJ_EXTERNAL_RSO           "eprj_rso"
#define EPRJ_EXTERNAL_NZMG                      "nzmg"
#define EPRJ_EXTERNAL_INTEGERIZED_SINUSOIDAL    "isin"

static const char *const apszDatumMap[] = {
    // Imagine name, WKT name.
    "NAD27", "North_American_Datum_1927",
    "NAD83", "North_American_Datum_1983",
    "WGS 84", "WGS_1984",
    "WGS 1972", "WGS_1972",
    "GDA94", "Geocentric_Datum_of_Australia_1994",
    NULL, NULL
};

static const char *const apszUnitMap[] = {
    "meters", "1.0",
    "meter", "1.0",
    "m", "1.0",
    "centimeters", "0.01",
    "centimeter", "0.01",
    "cm", "0.01",
    "millimeters", "0.001",
    "millimeter", "0.001",
    "mm", "0.001",
    "kilometers", "1000.0",
    "kilometer", "1000.0",
    "km", "1000.0",
    "us_survey_feet", "0.3048006096012192",
    "us_survey_foot", "0.3048006096012192",
    "feet", "0.3048006096012192",
    "foot", "0.3048006096012192",
    "ft", "0.3048006096012192",
    "international_feet", "0.3048",
    "international_foot", "0.3048",
    "inches", "0.0254000508001",
    "inch", "0.0254000508001",
    "in", "0.0254000508001",
    "yards", "0.9144",
    "yard", "0.9144",
    "yd", "0.9144",
    "miles", "1304.544",
    "mile", "1304.544",
    "mi", "1304.544",
    "modified_american_feet", "0.3048122530",
    "modified_american_foot", "0.3048122530",
    "clarke_feet", "0.3047972651",
    "clarke_foot", "0.3047972651",
    "indian_feet", "0.3047995142",
    "indian_foot", "0.3047995142",
    NULL, NULL
};

/* ==================================================================== */
/*      Table relating USGS and ESRI state plane zones.                 */
/* ==================================================================== */
static const int anUsgsEsriZones[] =
{
  101, 3101,
  102, 3126,
  201, 3151,
  202, 3176,
  203, 3201,
  301, 3226,
  302, 3251,
  401, 3276,
  402, 3301,
  403, 3326,
  404, 3351,
  405, 3376,
  406, 3401,
  407, 3426,
  501, 3451,
  502, 3476,
  503, 3501,
  600, 3526,
  700, 3551,
  901, 3601,
  902, 3626,
  903, 3576,
 1001, 3651,
 1002, 3676,
 1101, 3701,
 1102, 3726,
 1103, 3751,
 1201, 3776,
 1202, 3801,
 1301, 3826,
 1302, 3851,
 1401, 3876,
 1402, 3901,
 1501, 3926,
 1502, 3951,
 1601, 3976,
 1602, 4001,
 1701, 4026,
 1702, 4051,
 1703, 6426,
 1801, 4076,
 1802, 4101,
 1900, 4126,
 2001, 4151,
 2002, 4176,
 2101, 4201,
 2102, 4226,
 2103, 4251,
 2111, 6351,
 2112, 6376,
 2113, 6401,
 2201, 4276,
 2202, 4301,
 2203, 4326,
 2301, 4351,
 2302, 4376,
 2401, 4401,
 2402, 4426,
 2403, 4451,
 2500,    0,
 2501, 4476,
 2502, 4501,
 2503, 4526,
 2600,    0,
 2601, 4551,
 2602, 4576,
 2701, 4601,
 2702, 4626,
 2703, 4651,
 2800, 4676,
 2900, 4701,
 3001, 4726,
 3002, 4751,
 3003, 4776,
 3101, 4801,
 3102, 4826,
 3103, 4851,
 3104, 4876,
 3200, 4901,
 3301, 4926,
 3302, 4951,
 3401, 4976,
 3402, 5001,
 3501, 5026,
 3502, 5051,
 3601, 5076,
 3602, 5101,
 3701, 5126,
 3702, 5151,
 3800, 5176,
 3900,    0,
 3901, 5201,
 3902, 5226,
 4001, 5251,
 4002, 5276,
 4100, 5301,
 4201, 5326,
 4202, 5351,
 4203, 5376,
 4204, 5401,
 4205, 5426,
 4301, 5451,
 4302, 5476,
 4303, 5501,
 4400, 5526,
 4501, 5551,
 4502, 5576,
 4601, 5601,
 4602, 5626,
 4701, 5651,
 4702, 5676,
 4801, 5701,
 4802, 5726,
 4803, 5751,
 4901, 5776,
 4902, 5801,
 4903, 5826,
 4904, 5851,
 5001, 6101,
 5002, 6126,
 5003, 6151,
 5004, 6176,
 5005, 6201,
 5006, 6226,
 5007, 6251,
 5008, 6276,
 5009, 6301,
 5010, 6326,
 5101, 5876,
 5102, 5901,
 5103, 5926,
 5104, 5951,
 5105, 5976,
 5201, 6001,
 5200, 6026,
 5200, 6076,
 5201, 6051,
 5202, 6051,
 5300,    0,
 5400,    0
};


static const double R2D = 180.0 / M_PI;
static const double D2R = M_PI / 180.0;

static const double ARCSEC2RAD = M_PI / 648000.0;
static const double RAD2ARCSEC = 648000.0 / M_PI;

// this is very similar to the Imagine struct
// but Imagine needs the pointers dynamically allocated
// we have this so we can fill it in using the code 
// copied from GDAL
// we then translate it into the Imagine one.
typedef struct {
        char * sphereName;              /* name of the ellipsoid */
        double a;                       /* semi-major axis of ellipsoid */
        double b;                       /* semi-minor axis of ellipsoid */
        double eSquared;                /* eccentricity-squared */
        double radius;                  /* radius of the sphere */
} GDALEprj_Spheroid;

typedef struct {
    Eprj_ProType proType;       /* projection type */
    long proNumber;         /* projection number for internal 
                       projections */
    char * proExeName;  /* projection executable name for
                       EXTERNal projections */
    char * proName; /* projection name */
    long proZone;           /* projection zone (UTM, SP only) */
    double proParams[15];   /* projection parameters array in the
                       GCTP form */
    GDALEprj_Spheroid proSpheroid;  /* projection spheroid */
} GDALEprj_ProParameters;

typedef struct {
        char *datumname;                /* name of the datum */
        Eprj_DatumType type;            /* The datum type */
        double  params[7];              /* The parameters for type
                                           EPRJ_DATUM_PARAMETRIC */
        char *gridname;                 /* name of the grid file */
} GDALEprj_Datum;

Eprj_MapProjection* WKTToMapProj(const char *pszProj, std::string &sProjName, std::string &sUnits)
{
    Eerr_ErrorReport* err = NULL;
    int idx = 0;
    OGRSpatialReference oSRS;
	char *pszP = const_cast<char *>(pszProj);
	bool bIgnoreUTM = true; // to match IGNORE_UTM creation option in GDAL/HFA

	const bool bHaveSRS =
        pszProj != NULL && strlen(pszProj) > 0 &&
        oSRS.importFromWkt(&pszP) == OGRERR_NONE;

	char *pszUnits = NULL;
	oSRS.GetLinearUnits(&pszUnits);
	sUnits = pszUnits;
		
	// hack for Imagine
	if( (sUnits == "metre" ) || (sUnits == "Meter" ) )
		sUnits = "meters";

	GDALEprj_Datum sDatum;
    GDALEprj_ProParameters sPro;
    memset( &sPro, 0, sizeof(sPro) );
	memset( &sDatum, 0, sizeof(sDatum) );

    // following copied from GDAL's hfadataset.cpp WriteProjection()
    // Collect datum information.
    OGRSpatialReference *poGeogSRS = bHaveSRS ? oSRS.CloneGeogCS() : NULL;

    if( poGeogSRS )
    {
        sDatum.datumname =
            const_cast<char *>(poGeogSRS->GetAttrValue("GEOGCS|DATUM"));
        if( sDatum.datumname == NULL )
            sDatum.datumname = const_cast<char *>("");

        // WKT to Imagine translation.
        for( int i = 0; apszDatumMap[i] != NULL; i += 2 )
        {
            if( EQUAL(sDatum.datumname, apszDatumMap[i+1]) )
            {
                sDatum.datumname = (char *)apszDatumMap[i];
                break;
            }
        }

        // Map some EPSG datum codes directly to Imagine names.
        const int nGCS = poGeogSRS->GetEPSGGeogCS();

        if( nGCS == 4326 )
            sDatum.datumname = const_cast<char *>("WGS 84");
        if( nGCS == 4322 )
            sDatum.datumname = const_cast<char*>("WGS 1972");
        if( nGCS == 4267 )
            sDatum.datumname = const_cast<char *>("NAD27");
        if( nGCS == 4269 )
            sDatum.datumname = const_cast<char *>("NAD83");
        if( nGCS == 4283 )
            sDatum.datumname = const_cast<char *>("GDA94");

        if( poGeogSRS->GetTOWGS84(sDatum.params) == OGRERR_NONE )
        {
            sDatum.type = EPRJ_DATUM_PARAMETRIC;
            sDatum.params[3] *= -ARCSEC2RAD;
            sDatum.params[4] *= -ARCSEC2RAD;
            sDatum.params[5] *= -ARCSEC2RAD;
            sDatum.params[6] *= 1e-6;
        }
        else if( EQUAL(sDatum.datumname, "NAD27") )
        {
            sDatum.type = EPRJ_DATUM_GRID;
            sDatum.gridname = const_cast<char *>("nadcon.dat");
        }
        else
        {
            // We will default to this (effectively WGS84) for now.
            sDatum.type = EPRJ_DATUM_PARAMETRIC;
        }

        sPro.proSpheroid.sphereName =
            (char *)poGeogSRS->GetAttrValue("GEOGCS|DATUM|SPHEROID");
        sPro.proSpheroid.a = poGeogSRS->GetSemiMajor();
        sPro.proSpheroid.b = poGeogSRS->GetSemiMinor();
        sPro.proSpheroid.radius = sPro.proSpheroid.a;

        const double a2 = sPro.proSpheroid.a * sPro.proSpheroid.a;
        const double b2 = sPro.proSpheroid.b * sPro.proSpheroid.b;

        sPro.proSpheroid.eSquared = (a2 - b2) / a2;
    }

    if( sDatum.datumname == NULL )
        sDatum.datumname = const_cast<char *>("");
    if( sPro.proSpheroid.sphereName == NULL )
        sPro.proSpheroid.sphereName = const_cast<char *>("");

    // Recognise various projections.
    const char *pszProjName = NULL;

    if( bHaveSRS )
        pszProjName = oSRS.GetAttrValue("PROJCS|PROJECTION");

    if( pszProjName == NULL )
    {
        if( bHaveSRS && oSRS.IsGeographic() )
        {
            sPro.proNumber = EPRJ_LATLONG;
            sPro.proName = const_cast<char *>("Geographic (Lat/Lon)");
        }
    }
    // TODO: Add State Plane.
    else if( !bIgnoreUTM && oSRS.GetUTMZone(NULL) != 0 )
    {
        int bNorth = FALSE;
        const int nZone = oSRS.GetUTMZone(&bNorth);
        sPro.proNumber = EPRJ_UTM;
        sPro.proName = const_cast<char *>("UTM");
        sPro.proZone = nZone;
        if( bNorth )
            sPro.proParams[3] = 1.0;
        else
            sPro.proParams[3] = -1.0;
    }
    else if( EQUAL(pszProjName, SRS_PT_ALBERS_CONIC_EQUAL_AREA) )
    {
        sPro.proNumber = EPRJ_ALBERS_CONIC_EQUAL_AREA;
        sPro.proName = const_cast<char *>("Albers Conical Equal Area");
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1) * D2R;
        sPro.proParams[3] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_2) * D2R;
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_LAMBERT_CONFORMAL_CONIC_2SP) )
    {
        sPro.proNumber = EPRJ_LAMBERT_CONFORMAL_CONIC;
        sPro.proName = const_cast<char *>("Lambert Conformal Conic");
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1) * D2R;
        sPro.proParams[3] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_2) * D2R;
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_MERCATOR_1SP) &&
             oSRS.GetProjParm(SRS_PP_SCALE_FACTOR) == 1.0 )
    {
        sPro.proNumber = EPRJ_MERCATOR;
        sPro.proName = const_cast<char *>("Mercator");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_MERCATOR_1SP) )
    {
        sPro.proNumber = EPRJ_MERCATOR_VARIANT_A;
        sPro.proName = const_cast<char *>("Mercator (Variant A)");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN)*D2R;
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR);
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_KROVAK) )
    {
        sPro.proNumber = EPRJ_KROVAK;
        sPro.proName = const_cast<char *>("Krovak");
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR);
        sPro.proParams[3] = oSRS.GetProjParm(SRS_PP_AZIMUTH) * D2R;
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
        sPro.proParams[9] = oSRS.GetProjParm(SRS_PP_PSEUDO_STD_PARALLEL_1);

        sPro.proParams[8] = 0.0;  // XY plane rotation
        sPro.proParams[10] = 1.0;  // X scale
        sPro.proParams[11] = 1.0;  // Y scale
    }
    else if( EQUAL(pszProjName, SRS_PT_POLAR_STEREOGRAPHIC) )
    {
        sPro.proNumber = EPRJ_POLAR_STEREOGRAPHIC;
        sPro.proName = const_cast<char *>("Polar Stereographic");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN) * D2R;
        // Hopefully the scale factor is 1.0!
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_POLYCONIC) )
    {
        sPro.proNumber = EPRJ_POLYCONIC;
        sPro.proName = const_cast<char *>("Polyconic");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_EQUIDISTANT_CONIC) )
    {
        sPro.proNumber = EPRJ_EQUIDISTANT_CONIC;
        sPro.proName = const_cast<char *>("Equidistant Conic");
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1) * D2R;
        sPro.proParams[3] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_2) * D2R;
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
        sPro.proParams[8] = 1.0;
    }
    else if( EQUAL(pszProjName, SRS_PT_TRANSVERSE_MERCATOR) )
    {
        sPro.proNumber = EPRJ_TRANSVERSE_MERCATOR;
        sPro.proName = const_cast<char *>("Transverse Mercator");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN) * D2R;
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR, 1.0);
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_STEREOGRAPHIC) )
    {
        sPro.proNumber = EPRJ_STEREOGRAPHIC_EXTENDED;
        sPro.proName = const_cast<char *>("Stereographic (Extended)");
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR, 1.0);
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_LAMBERT_AZIMUTHAL_EQUAL_AREA) )
    {
        sPro.proNumber = EPRJ_LAMBERT_AZIMUTHAL_EQUAL_AREA;
        sPro.proName = const_cast<char *>("Lambert Azimuthal Equal-area");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_AZIMUTHAL_EQUIDISTANT) )
    {
        sPro.proNumber = EPRJ_AZIMUTHAL_EQUIDISTANT;
        sPro.proName = const_cast<char *>("Azimuthal Equidistant");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_GNOMONIC) )
    {
        sPro.proNumber = EPRJ_GNOMONIC;
        sPro.proName = const_cast<char *>("Gnomonic");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_ORTHOGRAPHIC) )
    {
        sPro.proNumber = EPRJ_ORTHOGRAPHIC;
        sPro.proName = const_cast<char *>("Orthographic");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_SINUSOIDAL) )
    {
        sPro.proNumber = EPRJ_SINUSOIDAL;
        sPro.proName = const_cast<char *>("Sinusoidal");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_EQUIRECTANGULAR) )
    {
        sPro.proNumber = EPRJ_EQUIRECTANGULAR;
        sPro.proName = const_cast<char *>("Equirectangular");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_MILLER_CYLINDRICAL) )
    {
        sPro.proNumber = EPRJ_MILLER_CYLINDRICAL;
        sPro.proName = const_cast<char *>("Miller Cylindrical");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER) * D2R;
        // Hopefully the latitude is zero!
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_VANDERGRINTEN) )
    {
        sPro.proNumber = EPRJ_VANDERGRINTEN;
        sPro.proName = const_cast<char *>("Van der Grinten");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_HOTINE_OBLIQUE_MERCATOR) )
    {
        if( oSRS.GetProjParm(SRS_PP_RECTIFIED_GRID_ANGLE) == 0.0 )
        {
            sPro.proNumber = EPRJ_HOTINE_OBLIQUE_MERCATOR;
            sPro.proName = const_cast<char *>("Oblique Mercator (Hotine)");
            sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR, 1.0);
            sPro.proParams[3] = oSRS.GetProjParm(SRS_PP_AZIMUTH) * D2R;
            sPro.proParams[4] =
                oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER) * D2R;
            sPro.proParams[5] =
                oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER) * D2R;
            sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
            sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
            sPro.proParams[12] = 1.0;
        }
        else
        {
            sPro.proNumber = EPRJ_HOTINE_OBLIQUE_MERCATOR_VARIANT_A;
            sPro.proName =
                const_cast<char *>("Hotine Oblique Mercator (Variant A)");
            sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR, 1.0);
            sPro.proParams[3] = oSRS.GetProjParm(SRS_PP_AZIMUTH) * D2R;
            sPro.proParams[4] =
                oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER) * D2R;
            sPro.proParams[5] =
                oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER) * D2R;
            sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
            sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
            sPro.proParams[8] =
                oSRS.GetProjParm(SRS_PP_RECTIFIED_GRID_ANGLE) * D2R;
        }
    }
    else if( EQUAL(pszProjName, SRS_PT_HOTINE_OBLIQUE_MERCATOR_AZIMUTH_CENTER) )
    {
        sPro.proNumber = EPRJ_HOTINE_OBLIQUE_MERCATOR_AZIMUTH_CENTER;
        sPro.proName =
            const_cast<char *>("Hotine Oblique Mercator Azimuth Center");
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR, 1.0);
        sPro.proParams[3] = oSRS.GetProjParm(SRS_PP_AZIMUTH) * D2R;
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
        sPro.proParams[12] = 1.0;
    }
    else if( EQUAL(pszProjName, SRS_PT_ROBINSON) )
    {
        sPro.proNumber = EPRJ_ROBINSON;
        sPro.proName = const_cast<char *>("Robinson");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_MOLLWEIDE) )
    {
        sPro.proNumber = EPRJ_MOLLWEIDE;
        sPro.proName = const_cast<char *>("Mollweide");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_ECKERT_I) )
    {
        sPro.proNumber = EPRJ_ECKERT_I;
        sPro.proName = const_cast<char *>("Eckert I");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_ECKERT_II) )
    {
        sPro.proNumber = EPRJ_ECKERT_II;
        sPro.proName = const_cast<char *>("Eckert II");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_ECKERT_III) )
    {
        sPro.proNumber = EPRJ_ECKERT_III;
        sPro.proName = const_cast<char *>("Eckert III");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_ECKERT_IV) )
    {
        sPro.proNumber = EPRJ_ECKERT_IV;
        sPro.proName = const_cast<char *>("Eckert IV");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_ECKERT_V) )
    {
        sPro.proNumber = EPRJ_ECKERT_V;
        sPro.proName = const_cast<char *>("Eckert V");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_ECKERT_VI) )
    {
        sPro.proNumber = EPRJ_ECKERT_VI;
        sPro.proName = const_cast<char *>("Eckert VI");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_GALL_STEREOGRAPHIC) )
    {
        sPro.proNumber = EPRJ_GALL_STEREOGRAPHIC;
        sPro.proName = const_cast<char *>("Gall Stereographic");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_CASSINI_SOLDNER) )
    {
        sPro.proNumber = EPRJ_CASSINI;
        sPro.proName = const_cast<char *>("Cassini");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_TWO_POINT_EQUIDISTANT) )
    {
        sPro.proNumber = EPRJ_TWO_POINT_EQUIDISTANT;
        sPro.proName = const_cast<char *>("Two_Point_Equidistant");
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
        sPro.proParams[8] =
            oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_POINT_1, 0.0) * D2R;
        sPro.proParams[9] =
            oSRS.GetProjParm(SRS_PP_LATITUDE_OF_POINT_1, 0.0) * D2R;
        sPro.proParams[10] =
            oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_POINT_2, 60.0) * D2R;
        sPro.proParams[11] =
            oSRS.GetProjParm(SRS_PP_LATITUDE_OF_POINT_2, 60.0) * D2R;
    }
    else if( EQUAL(pszProjName, SRS_PT_BONNE) )
    {
        sPro.proNumber = EPRJ_BONNE;
        sPro.proName = const_cast<char *>("Bonne");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Loximuthal") )
    {
        sPro.proNumber = EPRJ_LOXIMUTHAL;
        sPro.proName = const_cast<char *>("Loximuthal");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm("central_parallel") * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Quartic_Authalic") )
    {
        sPro.proNumber = EPRJ_QUARTIC_AUTHALIC;
        sPro.proName = const_cast<char *>("Quartic Authalic");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Winkel_I") )
    {
        sPro.proNumber = EPRJ_WINKEL_I;
        sPro.proName = const_cast<char *>("Winkel I");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Winkel_II") )
    {
        sPro.proNumber = EPRJ_WINKEL_II;
        sPro.proName = const_cast<char *>("Winkel II");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Behrmann") )
    {
        sPro.proNumber = EPRJ_BEHRMANN;
        sPro.proName = const_cast<char *>("Behrmann");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Equidistant_Cylindrical") )
    {
        sPro.proNumber = EPRJ_EQUIDISTANT_CYLINDRICAL;
        sPro.proName = const_cast<char *>("Equidistant_Cylindrical");
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1) * D2R;
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_KROVAK) )
    {
        sPro.proNumber = EPRJ_KROVAK;
        sPro.proName = const_cast<char *>("Krovak");
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR, 1.0);
        sPro.proParams[3] = oSRS.GetProjParm(SRS_PP_AZIMUTH) * D2R;
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
        sPro.proParams[8] = oSRS.GetProjParm("XY_Plane_Rotation", 0.0) * D2R;
        sPro.proParams[9] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1) * D2R;
        sPro.proParams[10] = oSRS.GetProjParm("X_Scale", 1.0);
        sPro.proParams[11] = oSRS.GetProjParm("Y_Scale", 1.0);
    }
    else if( EQUAL(pszProjName, "Double_Stereographic") )
    {
        sPro.proNumber = EPRJ_DOUBLE_STEREOGRAPHIC;
        sPro.proName = const_cast<char *>("Double_Stereographic");
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR, 1.0);
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Aitoff") )
    {
        sPro.proNumber = EPRJ_AITOFF;
        sPro.proName = const_cast<char *>("Aitoff");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Craster_Parabolic") )
    {
        sPro.proNumber = EPRJ_CRASTER_PARABOLIC;
        sPro.proName = const_cast<char *>("Craster_Parabolic");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_CYLINDRICAL_EQUAL_AREA) )
    {
        sPro.proNumber = EPRJ_CYLINDRICAL_EQUAL_AREA;
        sPro.proName = const_cast<char *>("Cylindrical_Equal_Area");
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1) * D2R;
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Flat_Polar_Quartic") )
    {
        sPro.proNumber = EPRJ_FLAT_POLAR_QUARTIC;
        sPro.proName = const_cast<char *>("Flat_Polar_Quartic");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Times") )
    {
        sPro.proNumber = EPRJ_TIMES;
        sPro.proName = const_cast<char *>("Times");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Winkel_Tripel") )
    {
        sPro.proNumber = EPRJ_WINKEL_TRIPEL;
        sPro.proName = const_cast<char *>("Winkel_Tripel");
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1) * D2R;
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Hammer_Aitoff") )
    {
        sPro.proNumber = EPRJ_HAMMER_AITOFF;
        sPro.proName = const_cast<char *>("Hammer_Aitoff");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Vertical_Near_Side_Perspective") )
    {
        sPro.proNumber = EPRJ_VERTICAL_NEAR_SIDE_PERSPECTIVE;
        sPro.proName = const_cast<char *>("Vertical_Near_Side_Perspective");
        sPro.proParams[2] = oSRS.GetProjParm("Height");
        sPro.proParams[4] =
            oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER, 75.0) * D2R;
        sPro.proParams[5] =
            oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER, 40.0) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Hotine_Oblique_Mercator_Two_Point_Center") )
    {
        sPro.proNumber = EPRJ_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_CENTER;
        sPro.proName =
            const_cast<char *>("Hotine_Oblique_Mercator_Two_Point_Center");
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR, 1.0);
        sPro.proParams[5] =
            oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER, 40.0) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
        sPro.proParams[8] =
            oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_POINT_1, 0.0) * D2R;
        sPro.proParams[9] =
            oSRS.GetProjParm(SRS_PP_LATITUDE_OF_POINT_1, 0.0) * D2R;
        sPro.proParams[10] =
            oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_POINT_2, 60.0) * D2R;
        sPro.proParams[11] =
            oSRS.GetProjParm(SRS_PP_LATITUDE_OF_POINT_2, 60.0) * D2R;
    }
    else if( EQUAL(pszProjName,
                   SRS_PT_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_NATURAL_ORIGIN) )
    {
        sPro.proNumber = EPRJ_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_NATURAL_ORIGIN;
        sPro.proName = const_cast<char *>(
            "Hotine_Oblique_Mercator_Two_Point_Natural_Origin");
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR, 1.0);
        sPro.proParams[5] =
            oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER, 40.0) * D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
        sPro.proParams[8] =
            oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_POINT_1, 0.0) * D2R;
        sPro.proParams[9] =
            oSRS.GetProjParm(SRS_PP_LATITUDE_OF_POINT_1, 0.0) * D2R;
        sPro.proParams[10] =
            oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_POINT_2, 60.0) * D2R;
        sPro.proParams[11] =
            oSRS.GetProjParm(SRS_PP_LATITUDE_OF_POINT_2, 60.0) * D2R;
    }
    else if( EQUAL(pszProjName, "New_Zealand_Map_Grid") )
    {
        sPro.proType = EPRJ_EXTERNAL;
        sPro.proNumber = 0;
        sPro.proExeName = const_cast<char *>(EPRJ_EXTERNAL_NZMG);
        sPro.proName = const_cast<char *>("New Zealand Map Grid");
        sPro.proZone = 0;
        sPro.proParams[0] = 0;  // False easting etc not stored in .img it seems
        sPro.proParams[1] = 0;  // always fixed by definition.
        sPro.proParams[2] = 0;
        sPro.proParams[3] = 0;
        sPro.proParams[4] = 0;
        sPro.proParams[5] = 0;
        sPro.proParams[6] = 0;
        sPro.proParams[7] = 0;
    }
    else if( EQUAL(pszProjName, SRS_PT_TRANSVERSE_MERCATOR_SOUTH_ORIENTED) )
    {
        sPro.proNumber = EPRJ_TRANSVERSE_MERCATOR_SOUTH_ORIENTATED;
        sPro.proName =
            const_cast<char *>("Transverse Mercator (South Orientated)");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN) * D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN) * D2R;
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR, 1.0);
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }

    else
    {
#ifdef KEADEBUG                        
		keaDebugOut("Projection %s not supported for translation to Imagine.\n", pszProjName );
#endif
		return NULL;
    }

	
    Eprj_MapProjection* proj = NULL;

    if( sPro.proName != NULL )
    {
        // return to caller
        sProjName = sPro.proName;

        Eprj_ProParameters *projparams = eprj_ProParametersCreate(&err);
        HANDLE_ERR(err, NULL)

        // copy relevant fields
        projparams->proType = sPro.proType;
        projparams->proNumber = sPro.proNumber;
        projparams->proZone = sPro.proZone;
        if( sPro.proExeName != NULL )
        {
            eprj_CharStrCreate(sPro.proExeName, &projparams->proExeName, &err);
            HANDLE_ERR(err, NULL)
        }
        eprj_CharStrCreate(sPro.proName, &projparams->proName, &err);
        HANDLE_ERR(err, NULL)

        int nParams = sizeof(sPro.proParams) / sizeof(double);
        eprj_DblArrayCreate(nParams, &projparams->proParams, &err);
        HANDLE_ERR(err, NULL)
        for( int n = 0; n < nParams; n++ )
        {
            projparams->proParams.data[n] = sPro.proParams[n];
            //fprintf( stderr, "proj %d %f\n", n, sPro.proParams[n]);
        }

		if( sPro.proSpheroid.sphereName != NULL )
		{
			eprj_CharStrCreate(sPro.proSpheroid.sphereName, &projparams->proSpheroid->sphereName, &err);
			HANDLE_ERR(err, NULL)
		}
		projparams->proSpheroid->a = sPro.proSpheroid.a;
		projparams->proSpheroid->b = sPro.proSpheroid.b;
		projparams->proSpheroid->eSquared = sPro.proSpheroid.eSquared;
		projparams->proSpheroid->radius = sPro.proSpheroid.radius;
		
		if( sDatum.datumname != NULL )
		{
			eprj_CharStrCreate(sDatum.datumname, &projparams->proSpheroid->datum->datumname, &err);
			HANDLE_ERR(err, NULL)
		}
		if( sDatum.gridname != NULL )
		{
			eprj_CharStrCreate(sDatum.gridname, &projparams->proSpheroid->datum->gridname, &err);
			HANDLE_ERR(err, NULL)
		}
		projparams->proSpheroid->datum->type = sDatum.type;
		
		nParams = sizeof(sDatum.params) / sizeof(double);
		eprj_DblArrayCreate(nParams, &projparams->proSpheroid->datum->params, &err);
        HANDLE_ERR(err, NULL)
        for( int n = 0; n < nParams; n++ )
        {
            projparams->proSpheroid->datum->params.data[n] = sDatum.params[n];
        }
		
		// need to get this or we can't find spheroids etc
		Eint_InitToolkitData* init = eint_GetInit();

        proj = eprj_ProjectionInit(init, projparams, &err);
        HANDLE_ERR(err, NULL)

        // seems eprj_ProjectionInit takes a copy
        eprj_ProParametersFree(&projparams);

    }

    return proj;
}

/************************************************************************/
/*                           ESRIToUSGSZone()                           */
/*                                                                      */
/*      Convert ESRI style state plane zones to USGS style state        */
/*      plane zones.                                                    */
/************************************************************************/

static int ESRIToUSGSZone( int nESRIZone )

{
    if( nESRIZone == INT_MIN )
        return 0;
    if( nESRIZone < 0 )
        return std::abs(nESRIZone);

    const int nPairs = sizeof(anUsgsEsriZones) / (2 * sizeof(int));
    for( int i = 0; i < nPairs; i++ )
    {
        if( anUsgsEsriZones[i * 2 + 1] == nESRIZone )
            return anUsgsEsriZones[i * 2];
    }

    return 0;
}

// taken from hfadataset.cpp
char *
HFAPCSStructToWKT( const GDALEprj_Datum *psDatum,
                   const GDALEprj_ProParameters *psPro,
                   std::string &sUnits, std::string &sProjName )

{
    // General case for Erdas style projections.

    // We make a particular effort to adapt the mapinfo->proname as
    // the PROJCS[] name per #2422.
    OGRSpatialReference oSRS;

    if( psPro == NULL && sProjName != "" )
    {
        oSRS.SetLocalCS(sProjName.c_str());
    }
    else if( psPro == NULL )
    {
        return NULL;
    }
    else if( psPro->proType == EPRJ_EXTERNAL )
    {
        if( EQUALN(psPro->proExeName, EPRJ_EXTERNAL_NZMG, 4) )
        {
            // Handle New Zealand Map Grid (NZMG) external projection.  See:
            // http://www.linz.govt.nz/
            //
            // Is there a better way that doesn't require hardcoding
            // of these numbers?
            oSRS.SetNZMG(-41.0, 173.0, 2510000, 6023150);
        }
        else
        {
            oSRS.SetLocalCS(sProjName.c_str());
        }
    }
    else if( psPro->proNumber != EPRJ_LATLONG &&
             sProjName != "" )
    {
        oSRS.SetProjCS(sProjName.c_str());
    }
    else if( psPro->proNumber != EPRJ_LATLONG )
    {
        oSRS.SetProjCS(sProjName.c_str());
    }

    // Handle units.  It is important to deal with this first so
    // that the projection Set methods will automatically do
    // translation of linear values (like false easting) to PROJCS
    // units from meters.  Erdas linear projection values are
    // always in meters.
    if( oSRS.IsProjected() || oSRS.IsLocal() )
    {
        const char *pszUnits = NULL;

        if( sUnits != "" )
        {
            int iUnitIndex = 0;  // Used after for.
            for( ;
                 apszUnitMap[iUnitIndex] != NULL;
                 iUnitIndex += 2 )
            {
                if( EQUAL(apszUnitMap[iUnitIndex], sUnits.c_str()) )
                    break;
            }

            if( apszUnitMap[iUnitIndex] == NULL )
                iUnitIndex = 0;

            oSRS.SetLinearUnits(pszUnits, CPLAtof(apszUnitMap[iUnitIndex + 1]));
        }
        else
        {
            oSRS.SetLinearUnits(SRS_UL_METER, 1.0);
        }
    }

    char *pszNewProj = NULL;
    if( psPro == NULL )
    {
        if( oSRS.IsLocal() )
        {
            if( oSRS.exportToWkt(&pszNewProj) == OGRERR_NONE )
            {
                return pszNewProj;
            }
            else
            {
                pszNewProj = NULL;
                return NULL;
            }
        }
        else
            return NULL;
    }

    // Try to work out ellipsoid and datum information.
    const char *pszDatumName = psPro->proSpheroid.sphereName;
    const char *pszEllipsoidName = psPro->proSpheroid.sphereName;

    if( psDatum != NULL )
    {
        pszDatumName = psDatum->datumname;

        // Imagine to WKT translation.
        for( int i = 0;
             pszDatumName != NULL && apszDatumMap[i] != NULL;
             i += 2 )
        {
            if( EQUAL(pszDatumName, apszDatumMap[i]) )
            {
                pszDatumName = apszDatumMap[i+1];
                break;
            }
        }
    }

    if( psPro->proSpheroid.a == 0.0 )
        ((GDALEprj_ProParameters *)psPro)->proSpheroid.a = 6378137.0;
    if( psPro->proSpheroid.b == 0.0 )
        ((GDALEprj_ProParameters *)psPro)->proSpheroid.b = 6356752.3;

    const double dfInvFlattening =
        OSRCalcInvFlattening(psPro->proSpheroid.a, psPro->proSpheroid.b);

    // Handle different projection methods.
    switch( psPro->proNumber )
    {
      case EPRJ_LATLONG:
        break;

      case EPRJ_UTM:
        // We change this to unnamed so that SetUTM will set the long
        // UTM description.
        oSRS.SetProjCS("unnamed");
        oSRS.SetUTM(psPro->proZone, psPro->proParams[3] >= 0.0);

        // The PCS name from the above function may be different with the input
        // name.  If there is a PCS name in psMapInfo that is different with the
        // one in psPro, just use it as the PCS name. This case happens if the
        // dataset's SR was written by the new GDAL.
        if( strlen(sProjName.c_str()) > 0 &&
            strlen(psPro->proName) > 0 &&
            !EQUAL(sProjName.c_str(), psPro->proName) )
            oSRS.SetProjCS(sProjName.c_str());
        break;

      case EPRJ_STATE_PLANE:
      {
          char *pszUnitsName = NULL;
          double dfLinearUnits = oSRS.GetLinearUnits(&pszUnitsName);

          pszUnitsName = CPLStrdup(pszUnitsName);

          // Historically, hfa used esri state plane zone code. Try esri pe
          // string first.
          const int zoneCode = ESRIToUSGSZone(psPro->proZone);
          const char *pszDatum;
          if( psDatum )
              pszDatum = psDatum->datumname;
          else
              pszDatum = "HARN";
          const char *pszUnits;
          if( sUnits != "" )
              pszUnits = sUnits.c_str();
          else if( pszUnitsName && strlen(pszUnitsName) > 0 )
              pszUnits = pszUnitsName;
          else
              pszUnits = "meters";
          const int proNu = psPro->proNumber;
          if( oSRS.ImportFromESRIStatePlaneWKT(zoneCode, pszDatum,
                                               pszUnits, proNu) == OGRERR_NONE )
          {
              CPLFree(pszUnitsName);
              oSRS.morphFromESRI();
              oSRS.AutoIdentifyEPSG();
              oSRS.Fixup();
              if( oSRS.exportToWkt(&pszNewProj) == OGRERR_NONE )
                  return pszNewProj;
              else
                  return NULL;
          }

          // Set state plane zone.  Set NAD83/27 on basis of spheroid.
          oSRS.SetStatePlane(ESRIToUSGSZone(psPro->proZone),
                             fabs(psPro->proSpheroid.a - 6378137.0)< 1.0,
                             pszUnitsName, dfLinearUnits);

          CPLFree(pszUnitsName);

          // Same as the UTM, The following is needed.
          if( strlen(sProjName.c_str()) > 0 &&
              strlen(psPro->proName) > 0 &&
              !EQUAL(sProjName.c_str(), psPro->proName) )
              oSRS.SetProjCS(sProjName.c_str());
      }
      break;

      case EPRJ_ALBERS_CONIC_EQUAL_AREA:
        oSRS.SetACEA(psPro->proParams[2] * R2D, psPro->proParams[3] * R2D,
                     psPro->proParams[5] * R2D, psPro->proParams[4] * R2D,
                     psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_LAMBERT_CONFORMAL_CONIC:
        // Check the possible Wisconsin first.
        if( psDatum && EQUAL(psDatum->datumname, "HARN") )
        {
            if( oSRS.ImportFromESRIWisconsinWKT(
                    "Lambert_Conformal_Conic", psPro->proParams[4] * R2D,
                    psPro->proParams[5] * R2D,
                    sUnits.c_str()) == OGRERR_NONE )
            {
                oSRS.morphFromESRI();
                oSRS.AutoIdentifyEPSG();
                oSRS.Fixup();
                if( oSRS.exportToWkt(&pszNewProj) == OGRERR_NONE )
                    return pszNewProj;
            }
        }
        oSRS.SetLCC(psPro->proParams[2] * R2D, psPro->proParams[3] * R2D,
                    psPro->proParams[5] * R2D, psPro->proParams[4] * R2D,
                    psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_MERCATOR:
        oSRS.SetMercator(psPro->proParams[5]*R2D, psPro->proParams[4]*R2D,
                         1.0,
                         psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_POLAR_STEREOGRAPHIC:
        oSRS.SetPS(psPro->proParams[5] * R2D, psPro->proParams[4] * R2D,
                   1.0,
                   psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_POLYCONIC:
        oSRS.SetPolyconic(psPro->proParams[5] * R2D, psPro->proParams[4] * R2D,
                          psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_EQUIDISTANT_CONIC:
      {
          const double dfStdParallel2 =
              psPro->proParams[8] != 0.0
              ? psPro->proParams[3] * R2D
              : psPro->proParams[2] * R2D;
          oSRS.SetEC(psPro->proParams[2] * R2D, dfStdParallel2,
                     psPro->proParams[5] * R2D, psPro->proParams[4] * R2D,
                     psPro->proParams[6], psPro->proParams[7]);
          break;
      }
      case EPRJ_TRANSVERSE_MERCATOR:
      case EPRJ_GAUSS_KRUGER:
        // Check the possible Wisconsin first.
        if( psDatum && EQUAL(psDatum->datumname, "HARN") )
        {
            if( oSRS.ImportFromESRIWisconsinWKT(
                    "Transverse_Mercator",
                    psPro->proParams[4] * R2D,
                    psPro->proParams[5] * R2D,
                    sUnits.c_str()) == OGRERR_NONE )
            {
                oSRS.morphFromESRI();
                oSRS.AutoIdentifyEPSG();
                oSRS.Fixup();
                if( oSRS.exportToWkt(&pszNewProj) == OGRERR_NONE )
                    return pszNewProj;
            }
        }
        oSRS.SetTM(psPro->proParams[5] * R2D, psPro->proParams[4] * R2D,
                   psPro->proParams[2],
                   psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_STEREOGRAPHIC:
        oSRS.SetStereographic(psPro->proParams[5] * R2D,
                              psPro->proParams[4] * R2D,
                              1.0,
                              psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_LAMBERT_AZIMUTHAL_EQUAL_AREA:
        oSRS.SetLAEA(psPro->proParams[5] * R2D, psPro->proParams[4] * R2D,
                     psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_AZIMUTHAL_EQUIDISTANT:
        oSRS.SetAE(psPro->proParams[5] * R2D, psPro->proParams[4] * R2D,
                   psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_GNOMONIC:
        oSRS.SetGnomonic(psPro->proParams[5] * R2D, psPro->proParams[4] * R2D,
                         psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_ORTHOGRAPHIC:
        oSRS.SetOrthographic(psPro->proParams[5] * R2D,
                             psPro->proParams[4] * R2D,
                             psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_SINUSOIDAL:
        oSRS.SetSinusoidal(psPro->proParams[4] * R2D,
                           psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_PLATE_CARREE:
      case EPRJ_EQUIRECTANGULAR:
        oSRS.SetEquirectangular2(0.0,
                                 psPro->proParams[4] * R2D,
                                 psPro->proParams[5] * R2D,
                                 psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_EQUIDISTANT_CYLINDRICAL:
        oSRS.SetEquirectangular2(0.0,
                                 psPro->proParams[4] * R2D,
                                 psPro->proParams[2] * R2D,
                                 psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_MILLER_CYLINDRICAL:
        oSRS.SetMC(0.0, psPro->proParams[4] * R2D,
                   psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_VANDERGRINTEN:
        oSRS.SetVDG(psPro->proParams[4] * R2D,
                    psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_HOTINE_OBLIQUE_MERCATOR:
        if( psPro->proParams[12] > 0.0 )
            oSRS.SetHOM(psPro->proParams[5] * R2D, psPro->proParams[4] * R2D,
                        psPro->proParams[3] * R2D, 0.0,
                        psPro->proParams[2],
                        psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_HOTINE_OBLIQUE_MERCATOR_AZIMUTH_CENTER:
        oSRS.SetHOMAC(psPro->proParams[5] * R2D, psPro->proParams[4] * R2D,
                      psPro->proParams[3] * R2D, 0.0,
                      psPro->proParams[2],
                      psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_ROBINSON:
        oSRS.SetRobinson(psPro->proParams[4] * R2D,
                         psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_MOLLWEIDE:
        oSRS.SetMollweide(psPro->proParams[4] * R2D,
                          psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_GALL_STEREOGRAPHIC:
        oSRS.SetGS(psPro->proParams[4] * R2D,
                   psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_ECKERT_I:
        oSRS.SetEckert(1, psPro->proParams[4] * R2D,
                       psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_ECKERT_II:
        oSRS.SetEckert(2, psPro->proParams[4] * R2D,
                       psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_ECKERT_III:
        oSRS.SetEckert(3, psPro->proParams[4] * R2D,
                       psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_ECKERT_IV:
        oSRS.SetEckert(4, psPro->proParams[4] * R2D,
                       psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_ECKERT_V:
        oSRS.SetEckert(5, psPro->proParams[4] * R2D,
                       psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_ECKERT_VI:
        oSRS.SetEckert(6, psPro->proParams[4] * R2D,
                       psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_CASSINI:
        oSRS.SetCS(psPro->proParams[5] * R2D, psPro->proParams[4] * R2D,
                   psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_TWO_POINT_EQUIDISTANT:
        oSRS.SetTPED(psPro->proParams[9] * R2D,
                     psPro->proParams[8] * R2D,
                     psPro->proParams[11] * R2D,
                     psPro->proParams[10] * R2D,
                     psPro->proParams[6], psPro->proParams[7]);
      break;

      case EPRJ_STEREOGRAPHIC_EXTENDED:
        oSRS.SetStereographic(psPro->proParams[5] * R2D,
                              psPro->proParams[4] * R2D,
                              psPro->proParams[2],
                              psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_BONNE:
        oSRS.SetBonne(psPro->proParams[2] * R2D, psPro->proParams[4] * R2D,
                      psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_LOXIMUTHAL:
      {
          oSRS.SetProjection("Loximuthal");
          oSRS.SetNormProjParm(SRS_PP_CENTRAL_MERIDIAN,
                           psPro->proParams[4] * R2D);
          oSRS.SetNormProjParm("central_parallel",
                           psPro->proParams[5] * R2D);
          oSRS.SetNormProjParm(SRS_PP_FALSE_EASTING, psPro->proParams[6]);
          oSRS.SetNormProjParm(SRS_PP_FALSE_NORTHING, psPro->proParams[7]);
      }
      break;

      case EPRJ_QUARTIC_AUTHALIC:
      {
          oSRS.SetProjection("Quartic_Authalic");
          oSRS.SetNormProjParm(SRS_PP_CENTRAL_MERIDIAN,
                           psPro->proParams[4] * R2D);
          oSRS.SetNormProjParm(SRS_PP_FALSE_EASTING, psPro->proParams[6]);
          oSRS.SetNormProjParm(SRS_PP_FALSE_NORTHING, psPro->proParams[7]);
      }
      break;

      case EPRJ_WINKEL_I:
      {
          oSRS.SetProjection("Winkel_I");
          oSRS.SetNormProjParm(SRS_PP_CENTRAL_MERIDIAN,
                           psPro->proParams[4] * R2D);
          oSRS.SetNormProjParm(SRS_PP_STANDARD_PARALLEL_1,
                           psPro->proParams[2] * R2D);
          oSRS.SetNormProjParm(SRS_PP_FALSE_EASTING, psPro->proParams[6]);
          oSRS.SetNormProjParm(SRS_PP_FALSE_NORTHING, psPro->proParams[7]);
      }
      break;

      case EPRJ_WINKEL_II:
      {
          oSRS.SetProjection("Winkel_II");
          oSRS.SetNormProjParm(SRS_PP_CENTRAL_MERIDIAN,
                           psPro->proParams[4] * R2D);
          oSRS.SetNormProjParm(SRS_PP_STANDARD_PARALLEL_1,
                           psPro->proParams[2] * R2D);
          oSRS.SetNormProjParm(SRS_PP_FALSE_EASTING, psPro->proParams[6]);
          oSRS.SetNormProjParm(SRS_PP_FALSE_NORTHING, psPro->proParams[7]);
      }
      break;

      case EPRJ_BEHRMANN:
      {
          oSRS.SetProjection("Behrmann");
          oSRS.SetNormProjParm(SRS_PP_CENTRAL_MERIDIAN,
                           psPro->proParams[4] * R2D);
          oSRS.SetNormProjParm(SRS_PP_FALSE_EASTING, psPro->proParams[6]);
          oSRS.SetNormProjParm(SRS_PP_FALSE_NORTHING, psPro->proParams[7]);
      }
      break;

      case EPRJ_KROVAK:
        oSRS.SetKrovak(psPro->proParams[4] * R2D, psPro->proParams[5] * R2D,
                       psPro->proParams[3] * R2D, psPro->proParams[9] * R2D,
                       psPro->proParams[2],
                       psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_DOUBLE_STEREOGRAPHIC:
      {
          oSRS.SetProjection("Double_Stereographic");
          oSRS.SetNormProjParm(SRS_PP_LATITUDE_OF_ORIGIN,
                           psPro->proParams[5] * R2D);
          oSRS.SetNormProjParm(SRS_PP_CENTRAL_MERIDIAN,
                           psPro->proParams[4] * R2D);
          oSRS.SetNormProjParm(SRS_PP_SCALE_FACTOR, psPro->proParams[2]);
          oSRS.SetNormProjParm(SRS_PP_FALSE_EASTING, psPro->proParams[6]);
          oSRS.SetNormProjParm(SRS_PP_FALSE_NORTHING, psPro->proParams[7]);
      }
      break;

      case EPRJ_AITOFF:
      {
          oSRS.SetProjection("Aitoff");
          oSRS.SetNormProjParm(SRS_PP_CENTRAL_MERIDIAN,
                           psPro->proParams[4] * R2D);
          oSRS.SetNormProjParm(SRS_PP_FALSE_EASTING, psPro->proParams[6]);
          oSRS.SetNormProjParm(SRS_PP_FALSE_NORTHING, psPro->proParams[7]);
      }
      break;

      case EPRJ_CRASTER_PARABOLIC:
      {
          oSRS.SetProjection("Craster_Parabolic");
          oSRS.SetNormProjParm(SRS_PP_CENTRAL_MERIDIAN,
                           psPro->proParams[4] * R2D);
          oSRS.SetNormProjParm(SRS_PP_FALSE_EASTING, psPro->proParams[6]);
          oSRS.SetNormProjParm(SRS_PP_FALSE_NORTHING, psPro->proParams[7]);
      }
      break;

      case EPRJ_CYLINDRICAL_EQUAL_AREA:
          oSRS.SetCEA(psPro->proParams[2] * R2D, psPro->proParams[4] * R2D,
                      psPro->proParams[6], psPro->proParams[7]);
      break;

      case EPRJ_FLAT_POLAR_QUARTIC:
      {
          oSRS.SetProjection("Flat_Polar_Quartic");
          oSRS.SetNormProjParm(SRS_PP_CENTRAL_MERIDIAN,
                           psPro->proParams[4] * R2D);
          oSRS.SetNormProjParm(SRS_PP_FALSE_EASTING, psPro->proParams[6]);
          oSRS.SetNormProjParm(SRS_PP_FALSE_NORTHING, psPro->proParams[7]);
      }
      break;

      case EPRJ_TIMES:
      {
          oSRS.SetProjection("Times");
          oSRS.SetNormProjParm(SRS_PP_CENTRAL_MERIDIAN,
                           psPro->proParams[4] * R2D);
          oSRS.SetNormProjParm(SRS_PP_FALSE_EASTING, psPro->proParams[6]);
          oSRS.SetNormProjParm(SRS_PP_FALSE_NORTHING, psPro->proParams[7]);
      }
      break;

      case EPRJ_WINKEL_TRIPEL:
      {
          oSRS.SetProjection("Winkel_Tripel");
          oSRS.SetNormProjParm(SRS_PP_STANDARD_PARALLEL_1,
                           psPro->proParams[2] * R2D);
          oSRS.SetNormProjParm(SRS_PP_CENTRAL_MERIDIAN,
                           psPro->proParams[4] * R2D);
          oSRS.SetNormProjParm(SRS_PP_FALSE_EASTING, psPro->proParams[6]);
          oSRS.SetNormProjParm(SRS_PP_FALSE_NORTHING, psPro->proParams[7]);
      }
      break;

      case EPRJ_HAMMER_AITOFF:
      {
          oSRS.SetProjection("Hammer_Aitoff");
          oSRS.SetNormProjParm(SRS_PP_CENTRAL_MERIDIAN,
                           psPro->proParams[4] * R2D);
          oSRS.SetNormProjParm(SRS_PP_FALSE_EASTING, psPro->proParams[6]);
          oSRS.SetNormProjParm(SRS_PP_FALSE_NORTHING, psPro->proParams[7]);
      }
      break;

      case EPRJ_VERTICAL_NEAR_SIDE_PERSPECTIVE:
      {
          oSRS.SetProjection("Vertical_Near_Side_Perspective");
          oSRS.SetNormProjParm(SRS_PP_LATITUDE_OF_CENTER,
                           psPro->proParams[5] * R2D);
          oSRS.SetNormProjParm(SRS_PP_LONGITUDE_OF_CENTER,
                           psPro->proParams[4] * R2D);
          oSRS.SetNormProjParm("height",
                           psPro->proParams[2]);
          oSRS.SetNormProjParm(SRS_PP_FALSE_EASTING, psPro->proParams[6]);
          oSRS.SetNormProjParm(SRS_PP_FALSE_NORTHING, psPro->proParams[7]);
      }
      break;

      case EPRJ_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_CENTER:
      {
          oSRS.SetProjection("Hotine_Oblique_Mercator_Twp_Point_Center");
          oSRS.SetNormProjParm(SRS_PP_LATITUDE_OF_CENTER,
                           psPro->proParams[5] * R2D);
          oSRS.SetNormProjParm(SRS_PP_LATITUDE_OF_1ST_POINT,
                           psPro->proParams[9] * R2D);
          oSRS.SetNormProjParm(SRS_PP_LONGITUDE_OF_1ST_POINT,
                           psPro->proParams[8] * R2D);
          oSRS.SetNormProjParm(SRS_PP_LATITUDE_OF_2ND_POINT,
                           psPro->proParams[11] * R2D);
          oSRS.SetNormProjParm(SRS_PP_LONGITUDE_OF_2ND_POINT,
                           psPro->proParams[10] * R2D);
          oSRS.SetNormProjParm(SRS_PP_SCALE_FACTOR,
                           psPro->proParams[2]);
          oSRS.SetNormProjParm(SRS_PP_FALSE_EASTING, psPro->proParams[6]);
          oSRS.SetNormProjParm(SRS_PP_FALSE_NORTHING, psPro->proParams[7]);
      }
      break;

      case EPRJ_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_NATURAL_ORIGIN:
        oSRS.SetHOM2PNO(psPro->proParams[5] * R2D,
                        psPro->proParams[8] * R2D,
                        psPro->proParams[9] * R2D,
                        psPro->proParams[10] * R2D,
                        psPro->proParams[11] * R2D,
                        psPro->proParams[2],
                        psPro->proParams[6], psPro->proParams[7]);
      break;

      case EPRJ_LAMBERT_CONFORMAL_CONIC_1SP:
        oSRS.SetLCC1SP(psPro->proParams[3] * R2D, psPro->proParams[2] * R2D,
                       psPro->proParams[4],
                       psPro->proParams[5], psPro->proParams[6]);
        break;

      case EPRJ_MERCATOR_VARIANT_A:
        oSRS.SetMercator(psPro->proParams[5]*R2D, psPro->proParams[4]*R2D,
                         psPro->proParams[2],
                         psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_PSEUDO_MERCATOR:  // Likely this is google mercator?
        oSRS.SetMercator(psPro->proParams[5] * R2D, psPro->proParams[4] * R2D,
                         1.0,
                         psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_HOTINE_OBLIQUE_MERCATOR_VARIANT_A:
        oSRS.SetHOM(psPro->proParams[5] * R2D, psPro->proParams[4] * R2D,
            psPro->proParams[3] * R2D, psPro->proParams[8] * R2D,
            psPro->proParams[2],
            psPro->proParams[6], psPro->proParams[7]);
        break;

      case EPRJ_TRANSVERSE_MERCATOR_SOUTH_ORIENTATED:
        oSRS.SetTMSO(psPro->proParams[5] * R2D, psPro->proParams[4] * R2D,
            psPro->proParams[2],
            psPro->proParams[6], psPro->proParams[7]);
        break;

      default:
        if( oSRS.IsProjected() )
            oSRS.GetRoot()->SetValue("LOCAL_CS");
        else
            oSRS.SetLocalCS(psPro->proName);
        break;
    }

    // Try and set the GeogCS information.
    if( oSRS.GetAttrNode("GEOGCS") == NULL &&
        oSRS.GetAttrNode("LOCAL_CS") == NULL )
    {
        if( pszDatumName == NULL)
            oSRS.SetGeogCS(pszDatumName, pszDatumName, pszEllipsoidName,
                           psPro->proSpheroid.a, dfInvFlattening);
        else if( EQUAL(pszDatumName, "WGS 84")
            || EQUAL(pszDatumName,"WGS_1984") )
            oSRS.SetWellKnownGeogCS("WGS84" );
        else if( strstr(pszDatumName, "NAD27") != NULL
                 || EQUAL(pszDatumName,"North_American_Datum_1927") )
            oSRS.SetWellKnownGeogCS("NAD27");
        else if( strstr(pszDatumName, "NAD83") != NULL
                 || EQUAL(pszDatumName, "North_American_Datum_1983"))
            oSRS.SetWellKnownGeogCS("NAD83");
        else
            oSRS.SetGeogCS(pszDatumName, pszDatumName, pszEllipsoidName,
                           psPro->proSpheroid.a, dfInvFlattening);

        if( psDatum != NULL && psDatum->type == EPRJ_DATUM_PARAMETRIC )
        {
            oSRS.SetTOWGS84(psDatum->params[0],
                            psDatum->params[1],
                            psDatum->params[2],
                            -psDatum->params[3] * RAD2ARCSEC,
                            -psDatum->params[4] * RAD2ARCSEC,
                            -psDatum->params[5] * RAD2ARCSEC,
                            psDatum->params[6] * 1e+6);
        }
    }

    // Try to insert authority information if possible.  Fixup any
    // ordering oddities.
    oSRS.AutoIdentifyEPSG();
    oSRS.Fixup();

    // Get the WKT representation of the coordinate system.
    if( oSRS.exportToWkt(&pszNewProj) == OGRERR_NONE )
        return pszNewProj;

    return NULL;
}

std::string MapProjToWKT( Eprj_MapProjection *proj,
                   std::string &sUnits, std::string &sProjName )
{
	// first convert Imagine data structures to GDAL ones
	GDALEprj_Datum sDatum;
	GDALEprj_ProParameters sPro;
	memset( &sPro, 0, sizeof(sPro) );
	memset( &sDatum, 0, sizeof(sDatum) );
	
	keaDebugOut("About to get params %p\n", proj);
	Eprj_ProParameters *projparams = EPRJ_PROPARAMETERS_GET(proj);
	keaDebugOut("got params\n");

	// copy relevant fields
	sPro.proType = projparams->proType;
	keaDebugOut("MapProjToWKT 00\n");
	sPro.proNumber = projparams->proNumber;
	sPro.proZone = projparams->proZone;
	keaDebugOut("MapProjToWKT 0 %d %d %d\n", (int)sPro.proType, (int)sPro.proNumber, (int)sPro.proZone);
	keaDebugOut("MapProjToWKT 0 %p %p\n", projparams->proName, projparams->proName.data);
	sPro.proName = projparams->proName.data;
	sPro.proExeName = projparams->proExeName.data;
keaDebugOut("MapProjToWKT 1");
	
	int nParams = sizeof(sPro.proParams) / sizeof(double);
	for( int n = 0; n < nParams; n++ )
	{
		sPro.proParams[n] = projparams->proParams.data[n];
	}
keaDebugOut("MapProjToWKT 2");

	sPro.proSpheroid.sphereName = projparams->proSpheroid->sphereName.data;
	sPro.proSpheroid.a = projparams->proSpheroid->a;
	sPro.proSpheroid.b = projparams->proSpheroid->b;
	sPro.proSpheroid.eSquared = projparams->proSpheroid->eSquared;
	sPro.proSpheroid.radius = projparams->proSpheroid->radius;
	sDatum.datumname = projparams->proSpheroid->datum->datumname.data;
	sDatum.gridname = projparams->proSpheroid->datum->gridname.data;
	sDatum.type = projparams->proSpheroid->datum->type;
keaDebugOut("MapProjToWKT 3");
	
	nParams = sizeof(sDatum.params) / sizeof(double);
	for( int n = 0; n < nParams; n++ )
	{
		sDatum.params[n] = projparams->proSpheroid->datum->params.data[n];
	}
keaDebugOut("MapProjToWKT 4");
		
	std::string sWKT;
	char *pszWKT = HFAPCSStructToWKT( &sDatum, &sPro, sUnits, sProjName);
	if( pszWKT != NULL )
	{
		sWKT = pszWKT;
		CPLFree(pszWKT);
	}
	keaDebugOut("finished function\n");
	return sWKT;
}
					   
