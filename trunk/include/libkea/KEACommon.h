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

#include "H5Cpp.h"

namespace libkea{
        
    static const std::string KEA_DATASETNAME_HEADER( "/HEADER" );
    static const std::string KEA_DATASETNAME_HEADER_NUMBANDS( "/HEADER/NUMBANDS" );
    static const std::string KEA_DATASETNAME_HEADER_BLOCKSIZE( "/HEADER/BLOCKSIZE" );
    static const std::string KEA_DATASETNAME_HEADER_RES( "/HEADER/RES" );
    static const std::string KEA_DATASETNAME_HEADER_TL( "/HEADER/TL" );
    static const std::string KEA_DATASETNAME_HEADER_ROT( "/HEADER/ROT" );
    static const std::string KEA_DATASETNAME_HEADER_WKT( "/HEADER/WKT" );
	static const std::string KEA_DATASETNAME_METADATA( "/METADATA" );
    static const std::string KEA_DATASETNAME_BAND( "/BAND" );
    
    static const std::string KEA_BANDNAME_DATA( "/DATA" );
    static const std::string KEA_BANDNAME_DESCRIP( "/DESCRIPTION" );
    static const std::string KEA_BANDNAME_METADATA( "/METADATA" );
    static const std::string KEA_BANDNAME_METADATA_MIN( "/METADATA/MIN" );
    static const std::string KEA_BANDNAME_METADATA_MAX( "/METADATA/MAX" );
    static const std::string KEA_BANDNAME_METADATA_MEAN( "/METADATA/MEAN" );
    static const std::string KEA_BANDNAME_METADATA_STDDEV( "/METADATA/STDDEV" );
    static const std::string KEA_BANDNAME_METADATA_WAVELENGTH( "/METADATA/WAVELENGTH" );
    static const std::string KEA_BANDNAME_METADATA_FWHM( "/METADATA/FWHM" );
    
    static const std::string KEA_BANDNAME_ATT( "/ATT" );   
    static const std::string KEA_ATT_GROUPNAME_HEADER( "/ATT/HEADER" );
	static const std::string KEA_ATT_GROUPNAME_DATA( "/ATT/DATA" );
    static const std::string KEA_ATT_GROUPNAME_NEIGHBOURS( "/ATT/NEIGHBOURS" );
    static const std::string KEA_ATT_BOOL_DATA( "/ATT/DATA/BOOL" );
    static const std::string KEA_ATT_INT_DATA( "/ATT/DATA/INT" );
    static const std::string KEA_ATT_FLOAT_DATA( "/ATT/DATA/FLOAT" );
    static const std::string KEA_ATT_NEIGHBOURS_DATA( "/ATT/NEIGHBOURS/NEIGHBOURS" );
    static const std::string KEA_ATT_BOOL_FIELDS_HEADER( "/ATT/HEADER/BOOL_FIELDS" );
    static const std::string KEA_ATT_INT_FIELDS_HEADER( "/ATT/HEADER/INT_FIELDS" );
    static const std::string KEA_ATT_FLOAT_FIELDS_HEADER( "/ATT/HEADER/FLOAT_FIELDS" );
    static const std::string KEA_ATT_SIZE_HEADER( "/ATT/HEADER/SIZE" );
    
    static const std::string KEA_ATT_NAME_FIELD( "NAME" );
    static const std::string KEA_ATT_INDEX_FIELD( "INDEX" );
    
    static const std::string KEA_BANDNAME_OVERVIEWS( "/OVERVIEWS" );
    static const std::string KEA_OVERVIEWSNAME_OVERVIEW( "/OVERVIEWS/OVERVIEW" );
    
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
    };
    
    std::string int2Str(int num)
    {
        std::ostringstream convert;
        convert << num;
        return convert.str();
    }
    
    std::string uint2Str(unsigned int num)
    {
        std::ostringstream convert;
        convert << num;
        return convert.str();
    }
    
    std::string long2Str(long num)
    {
        std::ostringstream convert;
        convert << num;
        return convert.str();
    }
    
    std::string ulong2Str(unsigned long num)
    {
        std::ostringstream convert;
        convert << num;
        return convert.str();
    }
    
    
}


#endif
