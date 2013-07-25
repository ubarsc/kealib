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

#include <stdio.h>
#ifdef __SUNPRO_CC
    // to get strcasecmp on Solaris
    #define __EXTENSIONS__
#endif
#include <string.h>
#include <vector>
#include <string>
#include "keaproj.h"
#include "kea.h"

// my hack to have enough calls to support the GDAL
// code in hfadataset.cpp - implements some of OGRSpatialReference
class ProjParser : public std::vector<ProjParser*>
{
public:
    ProjParser(const char *pszProj, int &idx);
    ~ProjParser();

    double GetProjParm(const char *pszName, double dfDefault=0.0);

    void Print();

    std::string getKey() { return m_fKey; }
    size_t getNumValues() { return m_fValues.size(); }
    std::string getValue(size_t n) { return m_fValues[n]; }

    std::string GetAttrValue(const char *pszName);

    ProjParser* ProjParser::GetSubValue(const char *pszName);
private:
    void addValue(std::string &val);

    std::string                 m_fKey;
    std::vector<std::string>    m_fValues;
};


// adapted from some pascal by Stefan Ascher
ProjParser::ProjParser(const char *pszProj, int &idx)
{
    bool opened = false;
    bool quoteopen = false;
    int commas = 0;
    int lastcomma = idx;
    std::string val;

    while( pszProj[idx] != '\0')
    {
        //printf( "looking at %c\n", pszProj[idx]);
        switch(pszProj[idx])
        {
            case '[':
            {
                if(opened)
                {
                    addValue(val);
                    val = "";
                    if( commas != 0 )
                    {
                        // new child
                        if( m_fValues.size() != 0 )
                        {
                            // this is a sub node not a value
                            m_fValues.pop_back();
                        }
                        idx = lastcomma;
                        ProjParser *child = new ProjParser(pszProj, idx);
                        push_back(child);
                    }
                } 
                else
                {
                    // Name, Value
                    opened = true;
                    m_fKey = val;
                    val = "";
                }
            }
            break;
            case ',':
            {
                lastcomma = idx;
                addValue(val);
                val = "";
                commas++;
            }
            break;
            case ']':
            {
                addValue(val);
                val = "";
                return;
            }
            break;
            case ' ':
            case 9:
            case 10:
            case 13:
            break;
            case '"':
            {
                quoteopen = !quoteopen;
                if(quoteopen)
                {
                    val = pszProj[idx];
                    idx++;
                    while(pszProj[idx] != '\0')
                    {
                        val = val + pszProj[idx];
                        if( pszProj[idx] == '"' )
                        {
                            quoteopen = false;
                            break;
                        }
                        idx++;
                    }
                }
            }
            break;
            default:
                val = val + pszProj[idx];
                break;
        }
        idx++;
    }
}

ProjParser::~ProjParser()
{
    for( iterator itr = begin(); itr != end(); itr++ )
    {
        delete (*itr);
    }
}

void ProjParser::addValue(std::string &val)
{
    if( val != "" )
    {
        /*if( val[0] == '"' )
            val.erase(0, 1);
        size_t last = val.size() - 1;
        if( val[last] == '"' )
            val.erase(last, 1);*/
        m_fValues.push_back(val);
    }
}

double ProjParser::GetProjParm(const char *pszName, double dfDefault)
{
    double dVal = dfDefault;
    // we have left quotes in the names
    std::string search = std::string("\"") + std::string(pszName) + std::string("\"");
    for( iterator itr = begin(); itr != end(); itr++ )
    {
        if( (*itr)->getKey() == "PARAMETER" )
        {
            if( (*itr)->getNumValues() >= 2 )
            {
                if( (*itr)->getValue(0) == search )
                {
                    dVal = atof((*itr)->getValue(1).c_str());
                }
            }
        }
    }

    return dVal;
}

void ProjParser::Print()
{
    printf( "Key: %s\n", m_fKey.c_str());
    printf("Values:\n");
    for( std::vector<std::string>::iterator itr = m_fValues.begin(); itr != m_fValues.end(); itr++ )
    {
        printf( "%s\n", (*itr).c_str());
    }
    printf( "subvalues:\n");
    for( iterator itr = begin(); itr != end(); itr++ )
    {
        (*itr)->Print();
    }
}

std::string ProjParser::GetAttrValue(const char *pszName)
{
    std::string val;
    for( iterator itr = begin(); itr != end(); itr++ )
    {
        if( (*itr)->getKey() == pszName )
        {
            if( (*itr)->getNumValues() >= 1 )
            {
                val = (*itr)->getValue(0);
                if( val[0] == '"' )
                    val.erase(0, 1);
                size_t last = val.size() - 1;
                if( val[last] == '"' )
                    val.erase(last, 1);
            }
        }
    }
    return val;
}

ProjParser* ProjParser::GetSubValue(const char *pszName)
{
   for( iterator itr = begin(); itr != end(); itr++ )
    {
        if( (*itr)->getKey() == pszName )
        {
            return (*itr);
            break;
        }
    }
    return NULL;
}

// from GDAL
// ogr_srs_api.h
#define SRS_PP_CENTRAL_MERIDIAN         "central_meridian"
#define SRS_PP_SCALE_FACTOR             "scale_factor"
#define SRS_PP_STANDARD_PARALLEL_1      "standard_parallel_1"
#define SRS_PP_STANDARD_PARALLEL_2      "standard_parallel_2"
#define SRS_PP_PSEUDO_STD_PARALLEL_1    "pseudo_standard_parallel_1"
#define SRS_PP_LONGITUDE_OF_CENTER      "longitude_of_center"
#define SRS_PP_LATITUDE_OF_CENTER       "latitude_of_center"
#define SRS_PP_LONGITUDE_OF_ORIGIN      "longitude_of_origin"
#define SRS_PP_LATITUDE_OF_ORIGIN       "latitude_of_origin"
#define SRS_PP_FALSE_EASTING            "false_easting"
#define SRS_PP_FALSE_NORTHING           "false_northing"
#define SRS_PP_AZIMUTH                  "azimuth"
#define SRS_PP_LONGITUDE_OF_POINT_1     "longitude_of_point_1"
#define SRS_PP_LATITUDE_OF_POINT_1      "latitude_of_point_1"
#define SRS_PP_LONGITUDE_OF_POINT_2     "longitude_of_point_2"
#define SRS_PP_LATITUDE_OF_POINT_2      "latitude_of_point_2"
#define SRS_PP_LONGITUDE_OF_POINT_3     "longitude_of_point_3"
#define SRS_PP_LATITUDE_OF_POINT_3      "latitude_of_point_3"
#define SRS_PP_RECTIFIED_GRID_ANGLE     "rectified_grid_angle"
#define SRS_PP_LANDSAT_NUMBER           "landsat_number"
#define SRS_PP_PATH_NUMBER              "path_number"
#define SRS_PP_PERSPECTIVE_POINT_HEIGHT "perspective_point_height"
#define SRS_PP_SATELLITE_HEIGHT         "satellite_height"
#define SRS_PP_FIPSZONE                 "fipszone"
#define SRS_PP_ZONE                     "zone"
#define SRS_PP_LATITUDE_OF_1ST_POINT    "Latitude_Of_1st_Point"
#define SRS_PP_LONGITUDE_OF_1ST_POINT   "Longitude_Of_1st_Point"
#define SRS_PP_LATITUDE_OF_2ND_POINT    "Latitude_Of_2nd_Point"
#define SRS_PP_LONGITUDE_OF_2ND_POINT   "Longitude_Of_2nd_Point"

/* ==================================================================== */
/*      Some "standard" strings.                                        */
/* ==================================================================== */

#define SRS_PT_ALBERS_CONIC_EQUAL_AREA                                  \
                                "Albers_Conic_Equal_Area"
#define SRS_PT_AZIMUTHAL_EQUIDISTANT "Azimuthal_Equidistant"
#define SRS_PT_CASSINI_SOLDNER  "Cassini_Soldner"
#define SRS_PT_CYLINDRICAL_EQUAL_AREA "Cylindrical_Equal_Area"
#define SRS_PT_BONNE            "Bonne"
#define SRS_PT_ECKERT_I         "Eckert_I"
#define SRS_PT_ECKERT_II        "Eckert_II"
#define SRS_PT_ECKERT_III       "Eckert_III"
#define SRS_PT_ECKERT_IV        "Eckert_IV"
#define SRS_PT_ECKERT_V         "Eckert_V"
#define SRS_PT_ECKERT_VI        "Eckert_VI"
#define SRS_PT_EQUIDISTANT_CONIC                                        \
                                "Equidistant_Conic"
#define SRS_PT_EQUIRECTANGULAR  "Equirectangular"
#define SRS_PT_GALL_STEREOGRAPHIC                                       \
                                "Gall_Stereographic"
#define SRS_PT_GAUSSSCHREIBERTMERCATOR                                      \
                                "Gauss_Schreiber_Transverse_Mercator"
#define SRS_PT_GEOSTATIONARY_SATELLITE                                  \
                                "Geostationary_Satellite"
#define SRS_PT_GOODE_HOMOLOSINE "Goode_Homolosine"
#define SRS_PT_IGH              "Interrupted_Goode_Homolosine"
#define SRS_PT_GNOMONIC         "Gnomonic"
#define SRS_PT_HOTINE_OBLIQUE_MERCATOR_AZIMUTH_CENTER                   \
                                "Hotine_Oblique_Mercator_Azimuth_Center"
#define SRS_PT_HOTINE_OBLIQUE_MERCATOR                                  \
                                "Hotine_Oblique_Mercator"
#define SRS_PT_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_NATURAL_ORIGIN         \
                            "Hotine_Oblique_Mercator_Two_Point_Natural_Origin"
#define SRS_PT_LABORDE_OBLIQUE_MERCATOR                                 \
                                "Laborde_Oblique_Mercator"
#define SRS_PT_LAMBERT_CONFORMAL_CONIC_1SP                              \
                                "Lambert_Conformal_Conic_1SP"
#define SRS_PT_LAMBERT_CONFORMAL_CONIC_2SP                              \
                                "Lambert_Conformal_Conic_2SP"
#define SRS_PT_LAMBERT_CONFORMAL_CONIC_2SP_BELGIUM                      \
                                "Lambert_Conformal_Conic_2SP_Belgium"
#define SRS_PT_LAMBERT_AZIMUTHAL_EQUAL_AREA                     \
                                "Lambert_Azimuthal_Equal_Area"
#define SRS_PT_MERCATOR_1SP     "Mercator_1SP"
#define SRS_PT_MERCATOR_2SP     "Mercator_2SP"
#define SRS_PT_MILLER_CYLINDRICAL "Miller_Cylindrical"
#define SRS_PT_MOLLWEIDE        "Mollweide"
#define SRS_PT_NEW_ZEALAND_MAP_GRID                                     \
                                "New_Zealand_Map_Grid"
#define SRS_PT_OBLIQUE_STEREOGRAPHIC                                    \
                                "Oblique_Stereographic"
#define SRS_PT_ORTHOGRAPHIC     "Orthographic"
#define SRS_PT_POLAR_STEREOGRAPHIC                                      \
                                "Polar_Stereographic"
#define SRS_PT_POLYCONIC        "Polyconic"
#define SRS_PT_ROBINSON         "Robinson"
#define SRS_PT_SINUSOIDAL       "Sinusoidal"
#define SRS_PT_STEREOGRAPHIC    "Stereographic"
#define SRS_PT_SWISS_OBLIQUE_CYLINDRICAL                                \
                                "Swiss_Oblique_Cylindrical"
#define SRS_PT_TRANSVERSE_MERCATOR                                      \
                                "Transverse_Mercator"
#define SRS_PT_TRANSVERSE_MERCATOR_SOUTH_ORIENTED                       \
                                "Transverse_Mercator_South_Orientated"
/* special mapinfo variants on Transverse Mercator */
#define SRS_PT_TRANSVERSE_MERCATOR_MI_21 \
                                "Transverse_Mercator_MapInfo_21"
#define SRS_PT_TRANSVERSE_MERCATOR_MI_22 \
                                "Transverse_Mercator_MapInfo_22"
#define SRS_PT_TRANSVERSE_MERCATOR_MI_23 \
                                "Transverse_Mercator_MapInfo_23"
#define SRS_PT_TRANSVERSE_MERCATOR_MI_24 \
                                "Transverse_Mercator_MapInfo_24"
#define SRS_PT_TRANSVERSE_MERCATOR_MI_25 \
                                "Transverse_Mercator_MapInfo_25"

#define SRS_PT_TUNISIA_MINING_GRID                                      \
                                "Tunisia_Mining_Grid"
#define SRS_PT_TWO_POINT_EQUIDISTANT                                    \
                                "Two_Point_Equidistant"
#define SRS_PT_VANDERGRINTEN    "VanDerGrinten"
#define SRS_PT_KROVAK           "Krovak"
#define SRS_PT_IMW_POLYCONIC    "International_Map_of_the_World_Polyconic"
#define SRS_PT_WAGNER_I         "Wagner_I"
#define SRS_PT_WAGNER_II        "Wagner_II"
#define SRS_PT_WAGNER_III       "Wagner_III"
#define SRS_PT_WAGNER_IV        "Wagner_IV"
#define SRS_PT_WAGNER_V         "Wagner_V"
#define SRS_PT_WAGNER_VI        "Wagner_VI"
#define SRS_PT_WAGNER_VII       "Wagner_VII"

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

#define EPRJ_EXTERNAL_RSO           "eprj_rso"
#define EPRJ_EXTERNAL_NZMG                      "nzmg"
#define EPRJ_EXTERNAL_INTEGERIZED_SINUSOIDAL    "isin"

#ifndef PI
#  define PI 3.14159265358979323846
#endif

#ifndef R2D
#  define R2D   (180/PI)
#endif
#ifndef D2R
#  define D2R   (PI/180)
#endif

/* -------------------------------------------------------------------- */
/*      Provide macros for case insensitive string comparisons.         */
/* -------------------------------------------------------------------- */
#ifndef EQUAL
#  if defined(WIN32) || defined(WIN32CE)
#    define STRCASECMP(a,b)         (stricmp(a,b))
#    define STRNCASECMP(a,b,n)      (strnicmp(a,b,n))
#  else
#    define STRCASECMP(a,b)         (strcasecmp(a,b))
#    define STRNCASECMP(a,b,n)      (strncasecmp(a,b,n))
#  endif
#  define EQUALN(a,b,n)           (STRNCASECMP(a,b,n)==0)
#  define EQUAL(a,b)              (STRCASECMP(a,b)==0)
#endif

// this is very similar to the Imagine struct
// but Imagine needs the pointers dynamically allocated
// we have this so we can fill it in using the code 
// copied from GDAL
// we then translate it into the Imagine one.
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
    Eprj_Spheroid proSpheroid;  /* projection spheroid */
} GDALEprj_ProParameters;


Eprj_MapProjection* WKTToMapProj(const char *pszProj, std::string &sProjName, std::string &sUnits)
{
    Eerr_ErrorReport* err = NULL;
    int idx = 0;
    ProjParser oSRS(pszProj, idx);

    sProjName = "Unknown";
    sUnits = oSRS.GetAttrValue("UNIT");
    if( sUnits == "" )
        sUnits = "Unknown";

    // need to get this or we can't find spheroids etc
    Eint_InitToolkitData* init = eint_GetInit();

    std::string projName = oSRS.GetAttrValue("PROJECTION");
    const char *pszProjName = projName.c_str();

    GDALEprj_ProParameters sPro;
    memset( &sPro, 0, sizeof(sPro) );

    // following copied from GDAL's hfadataset.cpp WriteProjection()
    if( EQUAL(pszProjName,SRS_PT_ALBERS_CONIC_EQUAL_AREA) )
    {
        sPro.proNumber = EPRJ_ALBERS_CONIC_EQUAL_AREA;
        sPro.proName = (char*) "Albers Conical Equal Area";
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1)*D2R;
        sPro.proParams[3] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_2)*D2R;
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_LAMBERT_CONFORMAL_CONIC_2SP) )
    {
        sPro.proNumber = EPRJ_LAMBERT_CONFORMAL_CONIC;
        sPro.proName = (char*) "Lambert Conformal Conic";
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1)*D2R;
        sPro.proParams[3] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_2)*D2R;
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_MERCATOR_1SP) 
             && oSRS.GetProjParm(SRS_PP_SCALE_FACTOR) == 1.0 )
    {
        sPro.proNumber = EPRJ_MERCATOR;
        sPro.proName = (char*) "Mercator";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_MERCATOR_1SP) )
    {
        sPro.proNumber = EPRJ_MERCATOR_VARIANT_A;
        sPro.proName = (char*) "Mercator (Variant A)";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN)*D2R;
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR);
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_KROVAK) )
    {
        sPro.proNumber = EPRJ_KROVAK;
        sPro.proName = (char*) "Krovak";
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR);
        sPro.proParams[3] = oSRS.GetProjParm(SRS_PP_AZIMUTH)*D2R;
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
        sPro.proParams[9] = oSRS.GetProjParm(SRS_PP_PSEUDO_STD_PARALLEL_1);

        // XY plane rotation
        sPro.proParams[8] = 0.0;
        // X scale
        sPro.proParams[10] = 1.0;
        // Y scale
        sPro.proParams[11] = 1.0;
    }
    else if( EQUAL(pszProjName,SRS_PT_POLAR_STEREOGRAPHIC) )
    {
        sPro.proNumber = EPRJ_POLAR_STEREOGRAPHIC;
        sPro.proName = (char*) "Polar Stereographic";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN)*D2R;
        /* hopefully the scale factor is 1.0! */
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_POLYCONIC) )
    {
        sPro.proNumber = EPRJ_POLYCONIC;
        sPro.proName = (char*) "Polyconic";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_EQUIDISTANT_CONIC) )
    {
        sPro.proNumber = EPRJ_EQUIDISTANT_CONIC;
        sPro.proName = (char*) "Equidistant Conic";
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1)*D2R;
        sPro.proParams[3] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_2)*D2R;
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
        sPro.proParams[8] = 1.0;
    }
    else if( EQUAL(pszProjName,SRS_PT_TRANSVERSE_MERCATOR) )
    {
        sPro.proNumber = EPRJ_TRANSVERSE_MERCATOR;
        sPro.proName = (char*) "Transverse Mercator";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN)*D2R;
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR,1.0);
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_STEREOGRAPHIC) )
    {
        sPro.proNumber = EPRJ_STEREOGRAPHIC_EXTENDED;
        sPro.proName = (char*) "Stereographic (Extended)";
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR,1.0);
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_LAMBERT_AZIMUTHAL_EQUAL_AREA) )
    {
        sPro.proNumber = EPRJ_LAMBERT_AZIMUTHAL_EQUAL_AREA;
        sPro.proName = (char*) "Lambert Azimuthal Equal-area";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_AZIMUTHAL_EQUIDISTANT) )
    {
        sPro.proNumber = EPRJ_AZIMUTHAL_EQUIDISTANT;
        sPro.proName = (char*) "Azimuthal Equidistant";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_GNOMONIC) )
    {
        sPro.proNumber = EPRJ_GNOMONIC;
        sPro.proName = (char*) "Gnomonic";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_ORTHOGRAPHIC) )
    {
        sPro.proNumber = EPRJ_ORTHOGRAPHIC;
        sPro.proName = (char*) "Orthographic";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_SINUSOIDAL) )
    {
        sPro.proNumber = EPRJ_SINUSOIDAL;
        sPro.proName = (char*) "Sinusoidal";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_EQUIRECTANGULAR) )
    {
        sPro.proNumber = EPRJ_EQUIRECTANGULAR;
        sPro.proName = (char*) "Equirectangular";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_MILLER_CYLINDRICAL) )
    {
        sPro.proNumber = EPRJ_MILLER_CYLINDRICAL;
        sPro.proName = (char*) "Miller Cylindrical";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER)*D2R;
        /* hopefully the latitude is zero! */
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_VANDERGRINTEN) )
    {
        sPro.proNumber = EPRJ_VANDERGRINTEN;
        sPro.proName = (char*) "Van der Grinten";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_HOTINE_OBLIQUE_MERCATOR) )
    {
        sPro.proNumber = EPRJ_HOTINE_OBLIQUE_MERCATOR;
        sPro.proName = (char*) "Oblique Mercator (Hotine)";
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR,1.0);
        sPro.proParams[3] = oSRS.GetProjParm(SRS_PP_AZIMUTH)*D2R;
        /* hopefully the rectified grid angle is zero */
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
        sPro.proParams[12] = 1.0;
    }
    else if( EQUAL(pszProjName,SRS_PT_HOTINE_OBLIQUE_MERCATOR_AZIMUTH_CENTER) )
    {
        sPro.proNumber = EPRJ_HOTINE_OBLIQUE_MERCATOR_AZIMUTH_CENTER;
        sPro.proName = (char*) "Hotine Oblique Mercator Azimuth Center";
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR,1.0);
        sPro.proParams[3] = oSRS.GetProjParm(SRS_PP_AZIMUTH)*D2R;
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
        sPro.proParams[12] = 1.0;
    }
    else if( EQUAL(pszProjName,SRS_PT_ROBINSON) )
    {
        sPro.proNumber = EPRJ_ROBINSON;
        sPro.proName = (char*) "Robinson";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_MOLLWEIDE) )
    {
        sPro.proNumber = EPRJ_MOLLWEIDE;
        sPro.proName = (char*) "Mollweide";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_ECKERT_I) )
    {
        sPro.proNumber = EPRJ_ECKERT_I;
        sPro.proName = (char*) "Eckert I";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_ECKERT_II) )
    {
        sPro.proNumber = EPRJ_ECKERT_II;
        sPro.proName = (char*) "Eckert II";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_ECKERT_III) )
    {
        sPro.proNumber = EPRJ_ECKERT_III;
        sPro.proName = (char*) "Eckert III";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_ECKERT_IV) )
    {
        sPro.proNumber = EPRJ_ECKERT_IV;
        sPro.proName = (char*) "Eckert IV";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_ECKERT_V) )
    {
        sPro.proNumber = EPRJ_ECKERT_V;
        sPro.proName = (char*) "Eckert V";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_ECKERT_VI) )
    {
        sPro.proNumber = EPRJ_ECKERT_VI;
        sPro.proName = (char*) "Eckert VI";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_GALL_STEREOGRAPHIC) )
    {
        sPro.proNumber = EPRJ_GALL_STEREOGRAPHIC;
        sPro.proName = (char*) "Gall Stereographic";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_CASSINI_SOLDNER) )
    {
        sPro.proNumber = EPRJ_CASSINI;
        sPro.proName = (char*) "Cassini";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,SRS_PT_TWO_POINT_EQUIDISTANT) )
    {
        sPro.proNumber = EPRJ_TWO_POINT_EQUIDISTANT;
        sPro.proName = (char*) "Two_Point_Equidistant";
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
        sPro.proParams[8] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_POINT_1, 0.0)*D2R;
        sPro.proParams[9] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_POINT_1, 0.0)*D2R;
        sPro.proParams[10] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_POINT_2, 60.0)*D2R;
        sPro.proParams[11] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_POINT_2, 60.0)*D2R;
    }
    else if( EQUAL(pszProjName,SRS_PT_BONNE) )
    {
        sPro.proNumber = EPRJ_BONNE;
        sPro.proName = (char*) "Bonne";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,"Loximuthal") )
    {
        sPro.proNumber = EPRJ_LOXIMUTHAL;
        sPro.proName = (char*) "Loximuthal";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm("central_parallel")*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,"Quartic_Authalic") )
    {
        sPro.proNumber = EPRJ_QUARTIC_AUTHALIC;
        sPro.proName = (char*) "Quartic Authalic";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,"Winkel_I") )
    {
        sPro.proNumber = EPRJ_WINKEL_I;
        sPro.proName = (char*) "Winkel I";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,"Winkel_II") )
    {
        sPro.proNumber = EPRJ_WINKEL_II;
        sPro.proName = (char*) "Winkel II";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,"Behrmann") )
    {
        sPro.proNumber = EPRJ_BEHRMANN;
        sPro.proName = (char*) "Behrmann";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName,"Equidistant_Cylindrical") )
    {
        sPro.proNumber = EPRJ_EQUIDISTANT_CYLINDRICAL;
        sPro.proName = (char*) "Equidistant_Cylindrical";
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1)*D2R;
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_KROVAK) )
    {
        sPro.proNumber = EPRJ_KROVAK;
        sPro.proName = (char*) "Krovak";
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR, 1.0);
        sPro.proParams[3] = oSRS.GetProjParm(SRS_PP_AZIMUTH)*D2R;
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
        sPro.proParams[8] = oSRS.GetProjParm("XY_Plane_Rotation", 0.0)*D2R;
        sPro.proParams[9] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1)*D2R;
        sPro.proParams[10] = oSRS.GetProjParm("X_Scale", 1.0);
        sPro.proParams[11] = oSRS.GetProjParm("Y_Scale", 1.0);
    }
    else if( EQUAL(pszProjName, "Double_Stereographic") )
    {
        sPro.proNumber = EPRJ_DOUBLE_STEREOGRAPHIC;
        sPro.proName = (char*) "Double_Stereographic";
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR, 1.0);
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Aitoff") )
    {
        sPro.proNumber = EPRJ_AITOFF;
        sPro.proName = (char*) "Aitoff";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Craster_Parabolic") )
    {
        sPro.proNumber = EPRJ_CRASTER_PARABOLIC;
        sPro.proName = (char*) "Craster_Parabolic";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, SRS_PT_CYLINDRICAL_EQUAL_AREA) )
    {
        sPro.proNumber = EPRJ_CYLINDRICAL_EQUAL_AREA;
        sPro.proName = (char*) "Cylindrical_Equal_Area";
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1)*D2R;
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Flat_Polar_Quartic") )
    {
        sPro.proNumber = EPRJ_FLAT_POLAR_QUARTIC;
        sPro.proName = (char*) "Flat_Polar_Quartic";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Times") )
    {
        sPro.proNumber = EPRJ_TIMES;
        sPro.proName = (char*) "Times";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Winkel_Tripel") )
    {
        sPro.proNumber = EPRJ_WINKEL_TRIPEL;
        sPro.proName = (char*) "Winkel_Tripel";
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_STANDARD_PARALLEL_1)*D2R;
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Hammer_Aitoff") )
    {
        sPro.proNumber = EPRJ_HAMMER_AITOFF;
        sPro.proName = (char*) "Hammer_Aitoff";
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Vertical_Near_Side_Perspective") )
    {
        sPro.proNumber = EPRJ_VERTICAL_NEAR_SIDE_PERSPECTIVE;
        sPro.proName = (char*) "Vertical_Near_Side_Perspective";
        sPro.proParams[2] = oSRS.GetProjParm("Height");
        sPro.proParams[4] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_CENTER, 75.0)*D2R;
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER, 40.0)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
    }
    else if( EQUAL(pszProjName, "Hotine_Oblique_Mercator_Two_Point_Center") )
    {
        sPro.proNumber = EPRJ_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_CENTER;
        sPro.proName = (char*) "Hotine_Oblique_Mercator_Two_Point_Center";
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR, 1.0);
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER, 40.0)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
        sPro.proParams[8] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_POINT_1, 0.0)*D2R;
        sPro.proParams[9] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_POINT_1, 0.0)*D2R;
        sPro.proParams[10] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_POINT_2, 60.0)*D2R;
        sPro.proParams[11] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_POINT_2, 60.0)*D2R;
    }
    else if( EQUAL(pszProjName, SRS_PT_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_NATURAL_ORIGIN) )
    {
        sPro.proNumber = EPRJ_HOTINE_OBLIQUE_MERCATOR_TWO_POINT_NATURAL_ORIGIN;
        sPro.proName = (char*) "Hotine_Oblique_Mercator_Two_Point_Natural_Origin";
        sPro.proParams[2] = oSRS.GetProjParm(SRS_PP_SCALE_FACTOR, 1.0);
        sPro.proParams[5] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_CENTER, 40.0)*D2R;
        sPro.proParams[6] = oSRS.GetProjParm(SRS_PP_FALSE_EASTING);
        sPro.proParams[7] = oSRS.GetProjParm(SRS_PP_FALSE_NORTHING);
        sPro.proParams[8] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_POINT_1, 0.0)*D2R;
        sPro.proParams[9] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_POINT_1, 0.0)*D2R;
        sPro.proParams[10] = oSRS.GetProjParm(SRS_PP_LONGITUDE_OF_POINT_2, 60.0)*D2R;
        sPro.proParams[11] = oSRS.GetProjParm(SRS_PP_LATITUDE_OF_POINT_2, 60.0)*D2R;
    }
    else if( EQUAL(pszProjName,"New_Zealand_Map_Grid") )
    {
        sPro.proType = EPRJ_EXTERNAL;
        sPro.proNumber = 0;
        sPro.proExeName = (char*) EPRJ_EXTERNAL_NZMG;
        sPro.proName = (char*) "New Zealand Map Grid";
        sPro.proZone = 0;
        sPro.proParams[0] = 0;  // false easting etc not stored in .img it seems 
        sPro.proParams[1] = 0;  // always fixed by definition. 
        sPro.proParams[2] = 0;
        sPro.proParams[3] = 0;
        sPro.proParams[4] = 0;
        sPro.proParams[5] = 0;
        sPro.proParams[6] = 0;
        sPro.proParams[7] = 0;
    }
    // previous copied from GDAL's hfadataset.cpp WriteProjection()
    

/*    if( projName == "New_Zealand_Map_Grid" )
    {
        projparams = eprj_ProParametersCreate(&err);
        HANDLE_ERR(err, NULL)

        projparams->proType = EPRJ_EXTERNAL;
        projparams->proNumber = 0L;
        eprj_CharStrCreate((char*)"nzmg", &projparams->proExeName, &err);
        HANDLE_ERR(err, NULL)
        eprj_CharStrCreate((char*)"New Zealand Map Grid", &projparams->proName, &err);
        HANDLE_ERR(err, NULL)
        sProjName = "New Zealand Map Grid";
        projparams->proZone = 0L;
        eprj_DblArrayCreate(8L, &projparams->proParams, &err);
        HANDLE_ERR(err, NULL)
        projparams->proParams.data[0] = 0;
        projparams->proParams.data[1] = 0;
        projparams->proParams.data[2] = 0;
        projparams->proParams.data[3] = 0;
        projparams->proParams.data[4] = 0;
        projparams->proParams.data[5] = 0;
        projparams->proParams.data[6] = 0;
        projparams->proParams.data[7] = 0;
    }
    else if( projName == "Transverse_Mercator" )
    {
        projparams = eprj_ProParametersCreate(&err);
        HANDLE_ERR(err, NULL)

        projparams->proType = EPRJ_INTERNAL;
        projparams->proNumber = 9L;
        eprj_CharStrCreate((char*)"Transverse Mercator", &projparams->proName, &err);
        HANDLE_ERR(err, NULL)
        sProjName = "Transverse Mercator";
        projparams->proZone = 0L;
        eprj_DblArrayCreate(8L, &projparams->proParams, &err);
        HANDLE_ERR(err, NULL)
        projparams->proParams.data[0] = 0;
        projparams->proParams.data[1] = 0;
        projparams->proParams.data[2] = parser.GetProjParm(SRS_PP_SCALE_FACTOR,1.0);
        projparams->proParams.data[3] = 0;
        projparams->proParams.data[4] = parser.GetProjParm(SRS_PP_CENTRAL_MERIDIAN)*D2R;
        projparams->proParams.data[5] = parser.GetProjParm(SRS_PP_LATITUDE_OF_ORIGIN)*D2R;
        projparams->proParams.data[6] = parser.GetProjParm(SRS_PP_FALSE_EASTING);
        projparams->proParams.data[7] = parser.GetProjParm(SRS_PP_FALSE_NORTHING);
    }*/

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

        // find datum and spheroid info
        ProjParser *p = oSRS.GetSubValue("GEOGCS");
        if( p != NULL )
        {
            std::string datum = p->GetAttrValue("DATUM");
            p = p->GetSubValue("DATUM");
            if( p != NULL )
            {
                std::string spheroid = p->GetAttrValue("SPHEROID");
                //fprintf( stderr, "spheroid = %s datum = %s\n", spheroid.c_str(), datum.c_str());
                if( ( spheroid != "" ) && ( datum != "" ) )
                {
                    eprj_SpheroidByName(init, (char*)spheroid.c_str(), projparams->proSpheroid, &err);
                    HANDLE_ERR(err, NULL)
                    eprj_DatumByName(init, (char*)spheroid.c_str(), (char*)datum.c_str(), projparams->proSpheroid->datum, &err);
                    HANDLE_ERR(err, NULL)
                }
            }
        }
        proj = eprj_ProjectionInit(init, projparams, &err);
        HANDLE_ERR(err, NULL)

        // seems eprj_ProjectionInit takes a copy
        eprj_ProParametersFree(&projparams);
    }

    return proj;
}

