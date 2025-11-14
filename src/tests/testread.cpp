/*
 *  testread.cpp
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
#include <algorithm>
#include "libkea/KEAImageIO.h"
#include "testsupport.h"

int main()
{
    try
    {
        kealib::KEADataType keatype = CTypeStringToKEAType(STRINGIFY(KEA_DTYPE));
    
        kealib::KEAImageIO io;

        std::string test_kea_file = "test_" STRINGIFY(KEA_DTYPE) ".kea";
        
        std::cout << "Opening file" << std::endl;
        HighFive::File *h5file = kealib::KEAImageIO::openKeaH5RDOnly(test_kea_file);
        io.openKEAImageHeader(h5file);
        std::cout << "Opened file" << std::endl;
        
        auto spatialInfo2 = getSpatialInfo(10);
        auto readinfo2 = io.getSpatialInfo();
        if( !compareSpatialInfo(&spatialInfo2, readinfo2))
        {
            return 1;
        }

        std::cout << "Reading some image data" << std::endl;
        KEA_DTYPE *pReadData = (KEA_DTYPE*)calloc(readinfo2->xSize * readinfo2->ySize, sizeof(KEA_DTYPE));
        io.readImageBlock2Band(1, pReadData, 0, 0, readinfo2->xSize, readinfo2->ySize, readinfo2->xSize, readinfo2->ySize, keatype);
        std::cout << "Read some image data" << std::endl;

        KEA_DTYPE *pCheckData = createDataForType<KEA_DTYPE>(readinfo2->xSize, readinfo2->ySize);
        std::cout << "Comparing data written and read data" << std::endl;
        if( !compareData<KEA_DTYPE>(pReadData, pCheckData, readinfo2->xSize, readinfo2->ySize))
        {
            return 1;
        }
        std::cout << "Data compared" << std::endl;
        
        // below tests check reading off the edge ok
        std::cout << "Reading a Subset" << std::endl;
        KEA_DTYPE *pSubData = (KEA_DTYPE*)calloc(100 * 100, sizeof(KEA_DTYPE));
        io.readImageBlock2Band(1, pSubData, 200, 150, 100, 100, 100, 100, keatype);
        if( !compareDataSubset<KEA_DTYPE>(pReadData, pSubData, 200, 150, readinfo2->xSize, readinfo2->ySize, 100, 100))
        {
            return 1;
        }
        std::cout << "subset compared" << std::endl;
        
        std::cout << "Reading right edge" << std::endl;
        io.readImageBlock2Band(1, pSubData, IMG_XSIZE - 50, 0, 50, 100, 100, 100, keatype);
        if( !compareDataSubsetEdge<KEA_DTYPE>(pReadData, pSubData, IMG_XSIZE - 50, 0, readinfo2->xSize, readinfo2->ySize, 100, 100, 50, 100, 99))
        {
            return 1;
        }
        
        std::cout << "right edge compared" << std::endl;

        std::cout << "Reading bottom edge" << std::endl;
        io.readImageBlock2Band(1, pSubData, 0, IMG_YSIZE - 50, 100, 50, 100, 100, keatype);
        if( !compareDataSubsetEdge<KEA_DTYPE>(pReadData, pSubData, 0, IMG_YSIZE - 50, readinfo2->xSize, readinfo2->ySize, 100, 100, 100, 50, 99))
        {
            return 1;
        }
        
        std::cout << "bottom edge compared" << std::endl;

        std::cout << "Reading bottom right edge" << std::endl;
        io.readImageBlock2Band(1, pSubData, IMG_XSIZE - 50, IMG_YSIZE - 50, 50, 50, 100, 100, keatype);
        if( !compareDataSubsetEdge<KEA_DTYPE>(pReadData, pSubData, IMG_XSIZE - 50, IMG_YSIZE - 50, readinfo2->xSize, readinfo2->ySize, 100, 100, 50, 50, 99))
        {
            return 1;
        }
        
        std::cout << "bottom right edge compared" << std::endl;
        
        std::cout << "reading out of range" << std::endl;
        bool exception = false;
        try
        {
            io.readImageBlock2Band(1, pSubData, IMG_XSIZE + 50, 0, 100, 100, 100, 100, keatype);
        }
        catch(const kealib::KEAIOException &e)
        {
            exception = true;
        }
        if( !exception )
        {
            std::cout << "exception not raised" << std::endl;
            return 1;
        }
        std::cout << "reading out of checked" << std::endl;
        
        // check band 2 which as some data re-written at as offset
        std::cout << "checking offset written data" << std::endl;
        // first the normally written block (minus the bit at the bottom and the bit at the right where it has been re-written)
        io.readImageBlock2Band(2, pReadData, 0, 0, readinfo2->xSize - 50, readinfo2->ySize - 75, readinfo2->xSize - 50, readinfo2->ySize - 75, keatype);
        if( !compareDataSubset<KEA_DTYPE>(pCheckData, pReadData, 0, 0, readinfo2->xSize, readinfo2->ySize, readinfo2->xSize - 50, readinfo2->ySize - 75))
        {
            return 1;
        }
        std::cout << "checking the right edge" << std::endl;
        // then the bit where I reset the numbers again (over the right edge)
        io.readImageBlock2Band(2, pReadData, readinfo2->xSize - 50, 0, 50, readinfo2->ySize, 50, readinfo2->ySize, keatype);
        if( !compareDataSubset<KEA_DTYPE>(pCheckData, pReadData, 0, 0, readinfo2->xSize, readinfo2->ySize, 50, readinfo2->ySize))
        {
            return 1;
        }
        std::cout << "checking the bottom edge" << std::endl;
        // and the within bounds overwrite (excluding over the edge). At the bottom
        io.readImageBlock2Band(2, pReadData, 0, readinfo2->ySize - 75, readinfo2->xSize - 50, 75, readinfo2->xSize - 50, 75, keatype);
        if( !compareDataSubset<KEA_DTYPE>(pCheckData, pReadData, 0, 0, readinfo2->xSize, readinfo2->ySize, readinfo2->xSize - 50, 75))
        {
            return 1;
        }
        
        std::cout << "Checked offset written data" << std::endl;
        
        free(pSubData);
        free(pReadData);
        free(pCheckData);
        
        if( io.maskCreated(1) )
        {
            std::cout << "Mask created" << std::endl;
        }
        else
        {
            std::cout << "Mask NOT created" << std::endl;
            return 1;
        }
        
        // note: mask always uint8_t in here
        uint8_t *pReadMask = (uint8_t*)calloc(readinfo2->xSize * readinfo2->ySize, sizeof(uint8_t));
        io.readImageBlock2BandMask(1, pReadMask, 0, 0, readinfo2->xSize, readinfo2->ySize, readinfo2->xSize, readinfo2->ySize, kealib::kea_8uint);
        std::cout << "Read some mask data" << std::endl;

        uint8_t *pMaskData = createDataForType<uint8_t>(readinfo2->xSize, readinfo2->ySize);

        std::cout << "Comparing data written and read mask" << std::endl;
        if( !compareData<uint8_t>(pReadMask, pMaskData, readinfo2->xSize, readinfo2->ySize))
        {
            return 1;
        }
        
        std::cout << "Reading right edge mask" << std::endl;
        io.readImageBlock2BandMask(1, pReadMask, readinfo2->xSize - 50, 0, 50, 100, 100, 100, kealib::kea_8uint);
        if( !compareDataSubsetEdge<uint8_t>(pMaskData, pReadMask, readinfo2->xSize - 50, 0, readinfo2->xSize, readinfo2->ySize, 100, 100, 50, 100, 255))
        {
            return 1;
        }
        
        std::cout << "Mask compared" << std::endl;

        if( io.getImageMetaData("Test1") != "Value1" )
        {
            std::cout << "Error reading metadata" << std::endl;
            return 1;
        }
        if( io.getImageMetaData("Test2") != "Value2" )
        {
            std::cout << "Error reading metadata" << std::endl;
            return 1;
        }
        
        auto names = io.getImageMetaDataNames();
        if( names.size() != 4 )
        {
            std::cout << "Wrong number of image metadata items" << std::endl;
            return 1;
        }
        if( std::find(names.begin(), names.end(), "Test1") == names.end())
        {
            std::cout << "Can't find Test1 in image metadata" << std::endl;
            return 1;
        }
        if( std::find(names.begin(), names.end(), "Test2") == names.end())
        {
            std::cout << "Can't find Test2 in image metadata" << std::endl;
            return 1;
        }
        
        auto name_values = io.getImageMetaData();
        if( name_values.size() != 4 )
        {
            std::cout << "Wrong number of image metadata items/values" << std::endl;
            return 1;
        }
        for( auto itr = name_values.begin(); itr != name_values.end(); itr++)
        {
            auto name = (*itr).first;
            if( (name != "Test1") && (name != "Test2") && (name != "Test98") && (name != "Test99"))
            {
                std::cout << "Unexpected name " << name << std::endl;
                return 1;
            }
            auto value = (*itr).second;
            if( ( value != "Value1") && (value != "Value2") && (value != "Value98") && (value != "Value99"))
            {
                std::cout << "Unexpected name " << value << std::endl;
                return 1;
            }
        }
        std::cout << "Successfully read dataset metadata" << std::endl;

        if( io.getImageBandMetaData(1, "BandTest1") != "Value1" )
        {
            std::cout << "Error reading metadata" << std::endl;
            return 1;
        }
        if( io.getImageBandMetaData(1, "BandTest2") != "Value2" )
        {
            std::cout << "Error reading metadata" << std::endl;
            return 1;
        }

        auto bandnames = io.getImageBandMetaDataNames(1);
        if( bandnames.size() != 4 )
        {
            std::cout << "Wrong number of band metadata items" << std::endl;
            return 1;
        }
        if( std::find(bandnames.begin(), bandnames.end(), "BandTest1") == names.end())
        {
            std::cout << "Can't find BandTest1 in image metadata" << std::endl;
            return 1;
        }
        if( std::find(bandnames.begin(), bandnames.end(), "BandTest2") == names.end())
        {
            std::cout << "Can't find BandTest2 in image metadata" << std::endl;
            return 1;
        }
        
        auto bandname_values = io.getImageBandMetaData(1);
        if( bandname_values.size() != 4 )
        {
            std::cout << "Wrong number of image metadata items/values" << std::endl;
            return 1;
        }
        for( auto itr = bandname_values.begin(); itr != bandname_values.end(); itr++)
        {
            auto name = (*itr).first;
            if( (name != "BandTest1") && (name != "BandTest2") && (name != "BandTest98") && (name != "BandTest99"))
            {
                std::cout << "Unexpected name " << name << std::endl;
                return 1;
            }
            auto value = (*itr).second;
            if( ( value != "Value1") && (value != "Value2") && (value != "Value98") && (value != "Value99"))
            {
                std::cout << "Unexpected value " << value << std::endl;
                return 1;
            }
        }

        std::cout << "Successfully read band metadata" << std::endl;

        auto desc1 = io.getImageBandDescription(1);
        auto desc2 = io.getImageBandDescription(2);
        if( (desc1 != "HBand1Desc") || (desc2 != "HBand2Desc"))
        {
            std::cout << "band descriptions did not match " << desc1 << " " << desc1 << std::endl;
            return 1;
        }
        std::cout << "Read band descriptions ok" << std::endl;
        
        // read nodata back as different type
        uint32_t u32nodata = 0;
        io.getNoDataValue(1, &u32nodata, kealib::kea_32uint);
        if( u32nodata != 99 )
        {
            std::cout << "Wrong nodata value read" << std::endl;
            return 1;
        }
        // band 2 nodata is undefined
        double dnodata = -1;
        bool bNoDataSet = true;
        try
        {
            io.getNoDataValue(2, &dnodata, kealib::kea_64float);
        }
        catch(const kealib::KEAIOException &e)
        {
            bNoDataSet = false;
        }
        if( bNoDataSet )
        {
            std::cout << "Nodata not properly undefined" << std::endl;
            return 1;
        }
        std::cout << "Read nodata ok" << std::endl;
        
        if( io.getGCPProjection() != "KmKmqw" )
        {
            std::cout << "GCP Projection not correctly read" << std::endl;
            return 1;
        }
        
        if( io.getGCPCount() != 2 )
        {
            std::cout << "Wrong number of GCPs read" << std::endl;
            return 1;
        }
        
        auto readgcps = io.getGCPs();
        auto pGCPs = getGCPData();
        if(!compareGCPs(pGCPs, readgcps))
        {
            std::cout << "GCPs don't match" << std::endl;
            return 1;
        }
        freeGCPData(pGCPs);
        freeGCPData(readgcps);
        
        if( io.getAttributeTableChunkSize(1) != kealib::KEA_ATT_CHUNK_SIZE )
        {
            std::cout << "Attribute chunk size does not match" << std::endl;
            return 1;
        }
        
        if( io.getNumOfImageBands() != 2)
        {
            std::cout << "Wrong number of image bands" << std::endl;
            return 1; 
        }
        
        if( io.getImageBlockSize(1) != 512 ) 
        {
            std::cout << "image block size does not match" << std::endl;
            return 1;
        }
        
        // band 2 should be the default (kea_continuous)
        // but we changed band 1
        if( (io.getImageBandLayerType(1) != kealib::kea_continuous) ||
            (io.getImageBandLayerType(2) != kealib::kea_thematic) )
        {
            std::cout << "layer types do not match" << std::endl;
            return 1;
        }
        
        // band 2 should be the default (kea_generic) but 
        // we changed band 1
        if( ( io.getImageBandClrInterp(1) != kealib::kea_generic) ||
            (io.getImageBandClrInterp(2) != kealib::kea_redband))
        {
            std::cout << "colour interp does not match" << std::endl;
            return 1;
        }

        // overviews        
        if( io.getNumOfOverviews(1) != 2 )
        {
            std::cout << "should be 2 overviews" << std::endl;
            return 1;
        }
        if(io.getNumOfOverviews(2) != 0)
        {
            std::cout << "should be 0 overviews" << std::endl;
            return 1;
        }
        if( io.getOverviewBlockSize(1, 1) != OV_XSIZE)
        {
            std::cout << "wrong overview block size" << std::endl;
            return 1;
        }
        
        uint64_t xsize, ysize;
        io.getOverviewSize(1, 1, &xsize, &ysize);
        if( (xsize != OV_XSIZE) || (ysize != OV_YSIZE))
        {
            std::cout << "wrong overview size" << std::endl;
            return 1;
        }
        io.getOverviewSize(1, 2, &xsize, &ysize);
        if( (xsize != OV2_XSIZE) || (ysize != OV2_YSIZE))
        {
            std::cout << "wrong overview size2" << std::endl;
            return 1;
        }
        
        std::cout << "Reading some overview data" << std::endl;
        KEA_DTYPE *pReadOvData = (KEA_DTYPE*)calloc(OV_XSIZE * OV_YSIZE, sizeof(KEA_DTYPE));
        io.readFromOverview(1, 1, pReadOvData, 0, 0, OV_XSIZE, OV_YSIZE, OV_XSIZE, OV_YSIZE, keatype);
        std::cout << "Read some overview data" << std::endl;

        std::cout << "Comparing overview written and read data" << std::endl;
        KEA_DTYPE *pTestOvData = createDataForType<KEA_DTYPE>(OV_XSIZE, OV_YSIZE);
        if( !compareData<KEA_DTYPE>(pReadOvData, pTestOvData, OV_XSIZE, OV_YSIZE))
        {
            return 1;
        }
        
        std::cout << "reading right edge overview" << std::endl;
        io.readFromOverview(1, 1, pReadOvData, OV_XSIZE - 50, 0, 50, 100, 100, 100, keatype);
        if( !compareDataSubsetEdge<KEA_DTYPE>(pTestOvData, pReadOvData, OV_XSIZE - 50, 0, OV_XSIZE, OV_YSIZE, 100, 100, 50, 100, 99))
        {
            return 1;
        }
        
        free(pTestOvData);
        free(pReadOvData);
        std::cout << "Overview compared" << std::endl;
        
        std::vector<uint32_t> bands = {1, 2};
        std::vector<kealib::KEAATTType> ratTypes = {kealib::kea_att_file, kealib::kea_att_mem};
        for( const uint32_t& band_num : bands )
        {
            for( const kealib::KEAATTType& rat_type : ratTypes )
            {
                // bands should have the same RAT because we copy them
                std::cout << "Testing RAT type " << rat_type << " for band " << band_num << std::endl; 
                auto *rat1 = io.getAttributeTable(rat_type, band_num);
                
                if( rat1->getMaxGlobalColIdx() != 5)
                {
                    std::cout << "wrong number of global idx" << std::endl;
                    return 1;
                }
                
                std::cout << "reading fields" << std::endl;
                kealib::KEAATTField sKEAField;
                sKEAField = rat1->getField(0);
                if( (sKEAField.name != "FirstBool") || (sKEAField.dataType != kealib::kea_att_bool) ||
                    (sKEAField.idx != 0) || (sKEAField.usage != "") || (sKEAField.colNum != 0))
                {
                    std::cout << "First field info wrong" << std::endl;
                    dumpAttField(sKEAField);
                    return 1;
                }
                sKEAField = rat1->getField(1);
                if( (sKEAField.name != "FirstInt") || (sKEAField.dataType != kealib::kea_att_int) ||
                    (sKEAField.idx != 0) || (sKEAField.usage != "first") || (sKEAField.colNum != 1))
                {
                    std::cout << "Second field info wrong" << std::endl;
                    dumpAttField(sKEAField);
                    return 1;
                }
                sKEAField = rat1->getField(2);
                if( (sKEAField.name != "SecondInt") || (sKEAField.dataType != kealib::kea_att_int) ||
                    (sKEAField.idx != 1) || (sKEAField.usage != "second") || (sKEAField.colNum != 2))
                {
                    std::cout << "third field info wrong" << std::endl;
                    dumpAttField(sKEAField);
                    return 1;
                }
                sKEAField = rat1->getField(3);
                if( (sKEAField.name != "FirstFloat") || (sKEAField.dataType != kealib::kea_att_float) ||
                    (sKEAField.idx != 0) || (sKEAField.usage != "third") || (sKEAField.colNum != 3))
                {
                    std::cout << "fourth field info wrong" << std::endl;
                    dumpAttField(sKEAField);
                    return 1;
                }
                sKEAField = rat1->getField(4);
                if( (sKEAField.name != "FirstString") || (sKEAField.dataType != kealib::kea_att_string) ||
                    (sKEAField.idx != 0) || (sKEAField.usage != "fourth") || (sKEAField.colNum != 4))
                {
                    std::cout << "fifth field info wrong" << std::endl;
                    dumpAttField(sKEAField);
                    return 1;
                }
            
                std::cout << "Reading bool col" << std::endl;
                bool boolFields[RAT_SIZE];
                bool boolFieldsTruth[RAT_SIZE];
                createRatDataForType(boolFieldsTruth, RAT_SIZE);
                
                rat1->getBoolFields(0, RAT_SIZE, 0, boolFields);
                if( !compareRat(boolFields, boolFieldsTruth, RAT_SIZE))
                {
                    return 1;
                }
        
                std::cout << "Reading bool col subset" << std::endl;
                rat1->getBoolFields(101, 21, 0, boolFields);
                if( !compareRatSubset(boolFieldsTruth, RAT_SIZE, boolFields, 101, 21))
                {
                    return 1;
                }
        
                std::cout << "Reading int col 1" << std::endl;
                int64_t intFields[RAT_SIZE];
                int64_t intFieldsTruth[RAT_SIZE];
                createRatDataForType(intFieldsTruth, RAT_SIZE);
                
                rat1->getIntFields(0, RAT_SIZE, 0, intFields);
                if( !compareRat(intFields, intFieldsTruth, RAT_SIZE))
                {
                    return 1;
                }
        
                std::cout << "Reading int col 1 subset" << std::endl;
                rat1->getIntFields(101, 21, 0, intFields);
                if( !compareRatSubset(intFieldsTruth, RAT_SIZE, intFields, 101, 21))
                {
                    return 1;
                }
        
                std::cout << "Reading int col 2 with subset" << std::endl;
                rat1->getIntFields(0, RAT_SIZE - 10, 1, intFields);
                if( !compareRat(intFields, intFieldsTruth, RAT_SIZE - 10))
                {
                    return 1;
                }
        
                rat1->getIntFields(RAT_SIZE - 10, 10, 1, intFields);
                if( !compareRat(intFields, intFieldsTruth, 10))
                {
                    return 1;
                }
                
                std::cout << "reading string col" << std::endl;
                std::vector<std::string> stringBuffer;
                std::vector<std::string> stringBufferTruth(RAT_SIZE);
                createRatDataForString(&stringBufferTruth);
                
                rat1->getStringFields(0, RAT_SIZE, 0, &stringBuffer);
                if( !compareRatDataString(&stringBuffer, &stringBufferTruth))
                {
                    return 1;
                }
        
                std::cout << "Reading string subset" << std::endl;
                rat1->getStringFields(101, 21, 0, &stringBuffer);
                if( !compareRatDataStringSubset(&stringBufferTruth, &stringBuffer, 101))
                {
                    return 1;
                }
                
                std::cout << "Reading neighbours" << std::endl;
                std::vector<std::vector<size_t>* > neighbours;
                rat1->getNeighbours(0, RAT_SIZE, &neighbours);
                std::vector<std::vector<size_t>* > neighboursTruth;
                createNeighbours(RAT_SIZE, &neighboursTruth);
                if( !compareNeighbours(&neighbours, &neighboursTruth))
                {
                    return 1;
                }
                
                std::cout << "Reading neighbours subset" << std::endl;
                rat1->getNeighbours(101, 21, &neighbours);
                if( !compareNeighboursSubset(&neighboursTruth, 101, &neighbours))
                {
                    return 1;
                }
                
                clearNeighbours(&neighbours);
                clearNeighbours(&neighboursTruth);
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
