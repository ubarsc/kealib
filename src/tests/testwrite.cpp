/*
 *  testwrite.cpp
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
 
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "libkea/KEAImageIO.h"
#include "testsupport.h"

#define IMG_XSIZE 600
#define IMG_YSIZE 700
#define OV_XSIZE 300
#define OV_YSIZE 350
#define TEST_FIELD "test"
#define RAT_SIZE 256

int main()
{
    try
    {
        kealib::KEADataType keatype = CTypeStringToKEAType("uint8_t");
    
        auto spatialInfo = getSpatialInfo(0);
    
        auto bandDescrips = std::vector<std::string>(2);
        bandDescrips[0] = "Band 1 Name";
        bandDescrips[1] = "Band 2 Name";

        kealib::KEAImageIO io;

        std::string test_kea_file = "test_uint8_t.kea";

        std::cout << "Creating file" << std::endl;
        HighFive::File *h5file = kealib::KEAImageIO::createKEAImage(test_kea_file,
                        keatype, IMG_XSIZE, IMG_YSIZE, 2,
                        &bandDescrips, &spatialInfo);
        std::cout << "Created file" << std::endl;

        delete h5file;


        if (kealib::KEAImageIO::isKEAImage(test_kea_file))
        {
            std::cout << "File is a KEA image" << std::endl;
        }
        else
        {
            std::cout << "File is NOT a KEA image" << std::endl;
            return 1;  
        }

        std::cout << "Opening file" << std::endl;
        h5file = kealib::KEAImageIO::openKeaH5RW(test_kea_file);
        std::cout << "Opened file" << std::endl;

        io.openKEAImageHeader(h5file);
        
        auto readinfo = io.getSpatialInfo();
        if( !compareSpatialInfo(&spatialInfo, readinfo))
        {
            return 1;
        }
        
        auto spatialInfo2 = getSpatialInfo(10);
        io.setSpatialInfo(&spatialInfo2);
        /*
        uint64_t subXSize = 50;
        uint64_t subYSize = 100;

        uint64_t subXOff = 25;
        uint64_t subYOff = 50;
        */

        uint64_t subXSize = IMG_XSIZE;
        uint64_t subYSize = IMG_YSIZE;

        uint64_t subXOff = 0;
        uint64_t subYOff = 0;

        std::cout << "Writing some image data" << std::endl;
        uint8_t *pData = createDataForType<uint8_t>(subXSize, subYSize);
        io.writeImageBlock2Band(1, pData, subXOff, subYOff, subXSize, subYSize,
                    subXSize, subYSize, keatype);
        free(pData);
        std::cout << "Written some image data" << std::endl;
        
        io.createMask(1, 1);

        std::cout << "Created Mask" << std::endl;
                
        // write some mask data
        uint8_t *pMaskData = createDataForType<uint8_t>(subXSize, subYSize);
        io.writeImageBlock2BandMask(1, pMaskData, subXOff, subYOff, subXSize, subYSize,
                    subXSize, subYSize, kealib::kea_8uint);
        free(pMaskData);
        std::cout << "Wrote mask" << std::endl;
                    
        // dataset metadata
        io.setImageMetaData("Test1", "Value1");
        io.setImageMetaData("Test2", "Value2");
        
        std::vector< std::pair<std::string, std::string> > metaData;
        metaData.push_back(std::pair<std::string, std::string>("Test98","Value98"));
        metaData.push_back(std::pair<std::string, std::string>("Test99","Value99"));
        io.setImageMetaData(metaData);
        std::cout << "Wrote dataset metadata" << std::endl;
        
        // band metadata
        io.setImageBandMetaData(1, "BandTest1", "Value1");
        io.setImageBandMetaData(1, "BandTest2", "Value2");

        std::vector< std::pair<std::string, std::string> > bandmetaData;
        bandmetaData.push_back(std::pair<std::string, std::string>("BandTest98","Value98"));
        bandmetaData.push_back(std::pair<std::string, std::string>("BandTest99","Value99"));
        io.setImageBandMetaData(1, bandmetaData);
        std::cout << "Wrote band metadata" << std::endl;
        
        // description
        // test what was set on create
        if( (io.getImageBandDescription(1) != "Band 1 Name") ||
            (io.getImageBandDescription(2) != "Band 2 Name") )
        {
            std::cout << "Band names set on creation not retriived" << std::endl;
            return 1;
        }
        io.setImageBandDescription(1, "HBand1Desc");
        io.setImageBandDescription(2, "HBand2Desc");
        std::cout << "Wrote descriptions" << std::endl;
        
        // nodata
        uint16_t u16nodata = 999;
        // check reading is empty
        io.getNoDataValue(1, &u16nodata, kealib::kea_16uint);
        if( u16nodata != 999 )
        {
            std::cout << "Nodata appears set" << std::endl;
            return 1;
        }
        
        io.setNoDataValue(1, &u16nodata, kealib::kea_16uint);
        double dnodata = -123.89;
        io.setNoDataValue(2, &dnodata, kealib::kea_64float);
        io.getNoDataValue(2, &dnodata, kealib::kea_64float);
        if( dnodata !=  -123.89)
        {
            std::cout << "Nodata not written correctly" << std::endl;
            return 1;
        }
        io.undefineNoDataValue(2);
        std::cout << "Wrote nodata" << std::endl;

        auto pGCPs = getGCPData();

        io.setGCPs(pGCPs, "WKT1");
        std::cout << "wrote gcps" << std::endl;
        
        if( io.getGCPProjection() != "WKT1" )
        {
            std::cout << "GCP Projection not correctly read" << std::endl;
            return 1;
        }
        
        if( io.getGCPCount() != pGCPs->size() )
        {
            std::cout << "Wrong number of GCPs read" << std::endl;
            return 1;
        }
        freeGCPData(pGCPs);
        
        io.setGCPProjection("KmKmqw");
        std::cout << "Wrote GCP Proj" << std::endl;
        
        if ( (io.getImageBandDataType(1) != keatype) ||
             (io.getImageBandDataType(2) != keatype))
        {
            std::cout << "Wrong data type read" << std::endl;
            return 1;
        }
        
        if( io.getKEAImageVersion() != kealib::KEA_VERSION)
        {
            std::cout << "wrong kealib version string" << std::endl;
            return 1;
        }
        
        io.setImageBandLayerType(2, kealib::kea_thematic);
        std::cout << "set layer type" << std::endl;
        
        io.setImageBandClrInterp(2, kealib::kea_redband);
        std::cout << "set layer colour interp" << std::endl;
        
        if( (io.getNumOfOverviews(1) != 0) || (io.getNumOfOverviews(2) != 0))
        {
            std::cout << "should be zero overviews" << std::endl;
            return 1; 
        }
        
        io.createOverview(1, 0, OV_XSIZE, OV_YSIZE);
        io.createOverview(1, 1, 25, 50);
        if( io.getNumOfOverviews(1) != 2 )
        {
            std::cout << "should be 2 overviews" << std::endl;
            return 1;
        }
       
        io.createOverview(2, 0, 50, 100);
        io.removeOverview(2, 0);
        if( io.getNumOfOverviews(2) != 0 )
        {
            std::cout << "should be 2 overviews" << std::endl;
            return 1;
        }
        
        // write to overview
        uint8_t *pOvData = createDataForType<uint8_t>(OV_XSIZE, OV_YSIZE);
        std::cout << "write to overview" << std::endl;
        io.writeToOverview(1, 0, pOvData, 0, 0, OV_XSIZE, OV_YSIZE, OV_XSIZE, OV_YSIZE, keatype);
        free(pOvData);
        
        io.close();
        
    }
    catch(const kealib::KEAException &e)
    {
        fprintf(stderr, "Exception raised: %s\n", e.what());
        return 1;
    }
    printf("Success\n");

    return 0;
}
     