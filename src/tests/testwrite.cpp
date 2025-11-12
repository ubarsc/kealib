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

int main()
{
    try
    {
        kealib::KEADataType keatype = CTypeStringToKEAType(STRINGIFY(KEA_DTYPE));
    
        auto spatialInfo = getSpatialInfo(0);
    
        auto bandDescrips = std::vector<std::string>(2);
        bandDescrips[0] = "Band 1 Name";
        bandDescrips[1] = "Band 2 Name";

        kealib::KEAImageIO io;

        std::string test_kea_file = "test_" STRINGIFY(KEA_DTYPE) ".kea";

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
        
        // read image data (nothing written yet)
        // is it all zero?
        KEA_DTYPE *pUnsetData = (KEA_DTYPE*)calloc(100 * 100, sizeof(KEA_DTYPE));
        io.readImageBlock2Band(1, pUnsetData, 0, 0, 100, 100, 100, 100, keatype);
        if(!compareDataConstant<KEA_DTYPE>(pUnsetData, 0, 100, 100))
        {
            return 1;
        }

        uint64_t subXSize = IMG_XSIZE;
        uint64_t subYSize = IMG_YSIZE;

        uint64_t subXOff = 0;
        uint64_t subYOff = 0;

        std::cout << "Writing some image data" << std::endl;
        KEA_DTYPE *pData = createDataForType<KEA_DTYPE>(subXSize, subYSize);
        io.writeImageBlock2Band(1, pData, subXOff, subYOff, subXSize, subYSize,
                    subXSize, subYSize, keatype);
        io.writeImageBlock2Band(2, pData, subXOff, subYOff, subXSize, subYSize,
                    subXSize, subYSize, keatype);
        // over write subset within the file
        io.writeImageBlock2Band(2, pData, 0, IMG_YSIZE - 75, IMG_XSIZE, 75, subXSize, 75, keatype);
        // over the edge of the file
        io.writeImageBlock2Band(2, pData, IMG_XSIZE - 50, subYOff, 50, subYSize,
                    subXSize, subYSize, keatype);
        
        free(pData);
        std::cout << "Written some image data" << std::endl;
        
        io.createMask(1, 1);

        std::cout << "Created Mask" << std::endl;

        // is the mask band all 255?
        io.readImageBlock2BandMask(1, pUnsetData, 0, 0, 100, 100, 100, 100, keatype);
        KEA_DTYPE expected = 255;
        if( strcmp(STRINGIFY(KEA_DTYPE), "int8_t") == 0 )
        {
            expected = 127; // gets truncated
        }
        if(!compareDataConstant<KEA_DTYPE>(pUnsetData, expected, 100, 100))
        {
            return 1;
        }
        
        // write some mask data - note always uint8_t here
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
        bool bNoDataSet = true;
        uint16_t u16nodata = 99;
        try
        {
            // check reading is empty
            io.getNoDataValue(1, &u16nodata, kealib::kea_16uint);
        }
        catch(const kealib::KEAIOException &e)
        {
            bNoDataSet = false;
        }
        if( bNoDataSet )
        {
            std::cout << "Nodata appears set" << std::endl;
            return 1;
        }
        if( u16nodata != 99 )
        {
            std::cout << "Nodata changed when not set" << std::endl;
            return 1;
        }
        
        io.setNoDataValue(1, &u16nodata, kealib::kea_16uint);
        double dnodata = -123.89;
        KEA_DTYPE expected_nodata = dnodata; // should cast to type of image
        io.setNoDataValue(2, &dnodata, kealib::kea_64float);
        io.getNoDataValue(2, &dnodata, kealib::kea_64float);
        if( dnodata != expected_nodata)
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
        
        io.createOverview(1, 1, OV_XSIZE, OV_YSIZE);

        // set to zero?
        io.readFromOverview(1, 1, pUnsetData, 0, 0, 100, 100, 100, 100, keatype);
        if(!compareDataConstant<KEA_DTYPE>(pUnsetData, 0, 100, 100))
        {
            return 1;
        }
        
        free(pUnsetData);
        
        
        io.createOverview(1, 2, OV2_XSIZE, OV2_YSIZE);
        if( io.getNumOfOverviews(1) != 2 )
        {
            std::cout << "should be 2 overviews" << std::endl;
            return 1;
        }
       
        io.createOverview(2, 1, 50, 100);
        io.removeOverview(2, 1);
        if( io.getNumOfOverviews(2) != 0 )
        {
            std::cout << "should be 2 overviews" << std::endl;
            return 1;
        }
        
        // write to overview
        KEA_DTYPE *pOvData = createDataForType<KEA_DTYPE>(OV_XSIZE, OV_YSIZE);
        std::cout << "write to overview" << std::endl;
        io.writeToOverview(1, 1, pOvData, 0, 0, OV_XSIZE, OV_YSIZE, OV_XSIZE, OV_YSIZE, keatype);
        free(pOvData);
        
        if( io.attributeTablePresent(1) || io.attributeTablePresent(2) )
        {
            std::cout << "Attribute table present when it shouldn't be" << std::endl;
            return 1;
        }
        
        std::cout << "create RAT" << std::endl;
        auto *rat1 = io.getAttributeTable(kealib::kea_att_file, 1);
        rat1->addRows(RAT_SIZE);
        rat1->addAttBoolField("FirstBool", false);
        rat1->addAttIntField("FirstInt", 3);
        rat1->addAttIntField("SecondInt", 4);  // 4 will get ignored as you can only set fill value once...
        rat1->addAttFloatField("FirstFloat", 3.1);
        rat1->addAttStringField("FirstString", "hello");
        rat1->printAttributeTableHeaderInfo();
        
        std::cout << "created columns" << std::endl;
        std::cout << "checking bool fill" << std::endl;
        bool boolBuffer[RAT_SIZE]; 
        rat1->getBoolFields(0, RAT_SIZE, 0, boolBuffer);
        if( !compareRatConstant<bool>(boolBuffer, false, RAT_SIZE) )
        {
            return 1;
        }
        std::cout << "checking int fill" << std::endl;
        int64_t intBuffer[RAT_SIZE];
        rat1->getIntFields(0, RAT_SIZE, 0, intBuffer);
        if( !compareRatConstant<int64_t>(intBuffer, 3, RAT_SIZE))
        {
            return 1;
        }
        std::cout << "checking float fill" << std::endl;
        double floatBuffer[RAT_SIZE];
        rat1->getFloatFields(0, RAT_SIZE, 0, floatBuffer);
        if( !compareRatConstant<double>(floatBuffer, 3.1, RAT_SIZE))
        {
            return 1;
        }
        
        std::cout << "checking string fill" << std::endl;
        std::vector<std::string> stringBuffer;
        rat1->getStringFields(0, RAT_SIZE, 0, &stringBuffer);
        if( stringBuffer.size() != RAT_SIZE )
        {
            std::cout << "wrong number of strings read" << std::endl;
            return 1;
        }
        if( !compareRatConstantString(&stringBuffer, "hello"))
        {
            return 1;
        }
        
        std::cout << "writing bool col" << std::endl;
        createRatDataForType<bool>(boolBuffer, RAT_SIZE);
        rat1->setBoolFields(0, RAT_SIZE, 0, boolBuffer);
        
        std::cout << "writing int col 1" << std::endl;
        createRatDataForType<int64_t>(intBuffer, RAT_SIZE);
        rat1->setIntFields(0, RAT_SIZE, 0, intBuffer);

        std::cout << "writing int col 2" << std::endl;
        createRatDataForType<int64_t>(intBuffer, RAT_SIZE);
        rat1->setIntFields(0, RAT_SIZE, 1, intBuffer);

        std::cout << "writing int col 2 subset" << std::endl;
        rat1->setIntFields(RAT_SIZE - 10, 10, 1, intBuffer);
        
        std::cout << "writing float col" << std::endl;
        createRatDataForType<double>(floatBuffer, RAT_SIZE);
        rat1->setFloatFields(0, 10, 0, floatBuffer);
        
        std::cout << "writing string col" << std::endl;
        createRatDataForString(&stringBuffer);
        rat1->setStringFields(0, RAT_SIZE, 0, &stringBuffer);
        
        std::cout << "writing neighbours" << std::endl;
        std::vector<std::vector<size_t>* > neighbours;
        createNeighbours(RAT_SIZE, &neighbours);
        rat1->setNeighbours(0, RAT_SIZE, &neighbours);
        clearNeighbours(&neighbours);
        
        // now copy to the other band
        std::cout << "copying RAT" << std::endl;
        io.setAttributeTable(rat1, 2);

        std::cout << "testing single element RAT functions" << std::endl;
        std::vector<uint32_t> bands = {1, 2};
        std::vector<kealib::KEAATTType> ratTypes = {kealib::kea_att_file, kealib::kea_att_mem};
        for( const uint32_t& band_num : bands )
        {
            for( const kealib::KEAATTType& rat_type : ratTypes )
            {
                // bands should have the same RAT because we copy them
                std::cout << "Testing RAT type " << rat_type << " for band " << band_num << std::endl; 
                auto *rat = io.getAttributeTable(rat_type, band_num);
                // some testing of the individual fields code
                if( rat->getBoolField(3, 0) != boolBuffer[3] )
                {
                    std::cout << "bool field not read correctly" << std::endl;
                    return 1;
                }
                rat->setBoolField(3, 0, !boolBuffer[3]);
                if( rat->getBoolField(3, 0) == boolBuffer[3])
                {
                    std::cout << "updated bool field not read correctly" << std::endl;
                    return 1;
                }
                rat->setBoolField(3, 0, boolBuffer[3]);
                
                if( rat->getIntField(10, 0) != intBuffer[10])
                {
                    std::cout << "int field not read correctly" << std::endl;
                    return 1;
                }
                rat->setIntField(10, 0, 99);
                if( rat->getIntField(10, 0) != 99)
                {
                    std::cout << "updated int field not read correctly" << std::endl;
                    return 1;
                }
                rat->setIntField(10, 0, intBuffer[10]);
                
                if( rat->getStringField(8, 0) != stringBuffer[8])
                {
                    std::cout << "string field not read correctly" << std::endl;
                    return 1;
                }
                rat->setStringField(8, 0, "abcdef");
                if( rat->getStringField(8, 0) != "abcdef")
                {
                    std::cout << "updated string field not read correctly" << std::endl;
                    return 1;
                }
                rat->setStringField(8, 0, stringBuffer[8]);
            }
        }
        
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
     