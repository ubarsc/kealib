/*
 *  KEACommon.cpp
 *  LibKEA
 *
 *  Created by Pete Bunting on 02/07/2012.
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

#ifndef KEACommon_H
#define KEACommon_H

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "H5Cpp.h"

namespace libkea{
        
    static const std::string KEA_DATASETNAME_HEADER( "/HEADER" );
    static const std::string KEA_DATASETNAME_HEADER_NUMBANDS( "/HEADER/NUMBANDS" );
    static const std::string KEA_DATASETNAME_HEADER_BLOCKSIZE( "/HEADER/BLOCKSIZE" );
    static const std::string KEA_DATASETNAME_HEADER_RES( "/HEADER/RES" );
    static const std::string KEA_DATASETNAME_HEADER_TL( "/HEADER/TL" );
    static const std::string KEA_DATASETNAME_HEADER_ROT( "/HEADER/ROT" );
    static const std::string KEA_DATASETNAME_HEADER_SIZE( "/HEADER/SIZE" );
    static const std::string KEA_DATASETNAME_HEADER_WKT( "/HEADER/WKT" );
    
    static const std::string KEA_DATASETNAME_HEADER_FILETYPE( "/HEADER/FILETYPE" );
    static const std::string KEA_DATASETNAME_HEADER_GENERATOR( "/HEADER/GENERATOR" );
    static const std::string KEA_DATASETNAME_HEADER_VERSION( "/HEADER/VERSION" );
    
	static const std::string KEA_DATASETNAME_METADATA( "/METADATA" );
    static const std::string KEA_DATASETNAME_BAND( "/BAND" );
    
    static const std::string KEA_BANDNAME_DATA( "/DATA" );
    static const std::string KEA_BANDNAME_DESCRIP( "/DESCRIPTION" );
    static const std::string KEA_BANDNAME_METADATA( "/METADATA" );
    static const std::string KEA_BANDNAME_METADATA_MIN( "/METADATA/STATISTICS_MINIMUM" );
    static const std::string KEA_BANDNAME_METADATA_MAX( "/METADATA/STATISTICS_MAXIMUM" );
    static const std::string KEA_BANDNAME_METADATA_MEAN( "/METADATA/STATISTICS_MEAN" );
    static const std::string KEA_BANDNAME_METADATA_STDDEV( "/METADATA/STATISTICS_STDDEV" );
    static const std::string KEA_BANDNAME_METADATA_MODE( "/METADATA/STATISTICS_MODE" );
    static const std::string KEA_BANDNAME_METADATA_HISTOMIN( "/METADATA/STATISTICS_HISTOMIN" );
    static const std::string KEA_BANDNAME_METADATA_HISTOMAX( "/METADATA/STATISTICS_HISTOMAX" );
    static const std::string KEA_BANDNAME_METADATA_HISTONUMBINS( "/METADATA/STATISTICS_HISTONUMBINS" );
    static const std::string KEA_BANDNAME_METADATA_HISTOBINVALUES( "/METADATA/STATISTICS_HISTOBINVALUES" );
    static const std::string KEA_BANDNAME_METADATA_HISTOBINFUNCTION( "/METADATA/STATISTICS_HISTOBINFUNCTION" );
    static const std::string KEA_BANDNAME_METADATA_WAVELENGTH( "/METADATA/WAVELENGTH" );
    static const std::string KEA_BANDNAME_METADATA_FWHM( "/METADATA/FWHM" );
    
    static const std::string KEA_BANDNAME_ATT( "/ATT" );   
    static const std::string KEA_ATT_GROUPNAME_HEADER( "/ATT/HEADER" );
	static const std::string KEA_ATT_GROUPNAME_DATA( "/ATT/DATA" );
    static const std::string KEA_ATT_GROUPNAME_NEIGHBOURS( "/ATT/NEIGHBOURS" );
    static const std::string KEA_ATT_BOOL_DATA( "/ATT/DATA/BOOL" );
    static const std::string KEA_ATT_INT_DATA( "/ATT/DATA/INT" );
    static const std::string KEA_ATT_FLOAT_DATA( "/ATT/DATA/FLOAT" );
    static const std::string KEA_ATT_STRING_DATA( "/ATT/DATA/STRING" );
    static const std::string KEA_ATT_NEIGHBOURS_DATA( "/ATT/NEIGHBOURS/NEIGHBOURS" );
    static const std::string KEA_ATT_BOOL_FIELDS_HEADER( "/ATT/HEADER/BOOL_FIELDS" );
    static const std::string KEA_ATT_INT_FIELDS_HEADER( "/ATT/HEADER/INT_FIELDS" );
    static const std::string KEA_ATT_FLOAT_FIELDS_HEADER( "/ATT/HEADER/FLOAT_FIELDS" );
    static const std::string KEA_ATT_STRING_FIELDS_HEADER( "/ATT/HEADER/STRING_FIELDS" );
    static const std::string KEA_ATT_SIZE_HEADER( "/ATT/HEADER/SIZE" );
    
    static const std::string KEA_ATT_NAME_FIELD( "NAME" );
    static const std::string KEA_ATT_INDEX_FIELD( "INDEX" );
    
    static const std::string KEA_BANDNAME_OVERVIEWS( "/OVERVIEWS" );
    static const std::string KEA_OVERVIEWSNAME_OVERVIEW( "/OVERVIEWS/OVERVIEW" );
    
    static const std::string KEA_ATTRIBUTENAME_CLASS( "CLASS" );
	static const std::string KEA_ATTRIBUTENAME_IMAGE_VERSION( "IMAGE_VERSION" );
    
    static const int KEA_READ_MDC_NELMTS( 0 ); // 0
    static const hsize_t KEA_READ_RDCC_NELMTS( 512 ); // 512
    static const hsize_t KEA_READ_RDCC_NBYTES( 1048576 ); // 1048576
    static const double KEA_READ_RDCC_W0( 0.75 ); // 0.75
    static const hsize_t KEA_READ_SIEVE_BUF( 65536 ); // 65536
    static const hsize_t KEA_READ_META_BLOCKSIZE( 2048 ); // 2048
    
    static const int KEA_WRITE_MDC_NELMTS( 0 ); // 0
    static const hsize_t  KEA_WRITE_RDCC_NELMTS( 512 ); // 512
    static const hsize_t  KEA_WRITE_RDCC_NBYTES( 1048576 ); // 1048576
    static const double KEA_WRITE_RDCC_W0( 0.75 ); // 0.75
    static const hsize_t  KEA_WRITE_SIEVE_BUF( 65536 ); // 65536
    static const hsize_t  KEA_WRITE_META_BLOCKSIZE( 2048 ); // 2048
    static const unsigned int KEA_DEFLATE( 1 ); // 1
    static const hsize_t KEA_WRITE_CHUNK_SIZE( 200 ); //100
    
    
    enum KEADataType
    {
        kea_undefined = 0,
        kea_8int = 1,
        kea_16int = 2,
        kea_32int = 3,
        kea_64int = 4,        
        kea_8uint = 5,
        kea_16uint = 6,
        kea_32uint = 7,
        kea_64uint = 8,
        kea_32float = 9,
        kea_64float = 10,
    };
    
    struct KEAImageSpatialInfo
    {
        std::string wktString;
        double tlX;
        double tlY;
        float xRes;
        float yRes;
        float xRot;
        float yRot;
        unsigned int xSize;
        unsigned int ySize;
    };
    
    
    struct KEAATTFeature
    {
        size_t fid;
        std::vector<bool> *boolFields;
        std::vector<long> *intFields;
        std::vector<double> *floatFields;
        std::vector<size_t> *neighbours;
    };
    
    enum KEAAttributeDataType
    {
        rsgis_na = 0,
        rsgis_bool = 1,
        rsgis_int = 2,
        rsgis_float = 3,
    };
    
    struct KEAATTAttribute
    {
        std::string name;
        KEAAttributeDataType dataType;
        unsigned int idx;
    };
    
    inline std::string int2Str(int num)
    {
        std::ostringstream convert;
        convert << num;
        return convert.str();
    }
    
    inline std::string uint2Str(unsigned int num)
    {
        std::ostringstream convert;
        convert << num;
        return convert.str();
    }
    
    inline std::string long2Str(long num)
    {
        std::ostringstream convert;
        convert << num;
        return convert.str();
    }
    
    inline std::string ulong2Str(unsigned long num)
    {
        std::ostringstream convert;
        convert << num;
        return convert.str();
    }
    
    
}


#endif

