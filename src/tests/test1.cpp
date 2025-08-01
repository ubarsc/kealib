/*
 *  test1.cpp
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
#include "libkea/KEAImageIO.h"

#define IMG_XSIZE 100
#define IMG_YSIZE 200
#define TEST_FIELD "test"
#define RAT_SIZE 256

bool compareSpatialInfo(kealib::KEAImageSpatialInfo *p1, kealib::KEAImageSpatialInfo *p2)
{
    if( p1->tlX != p2->tlX )
    {
        std::cout << "tlX does not match" << std::endl;
        return false;
    }
    if( p1->tlY != p2->tlY )
    {
        std::cout << "tlY does not match" << std::endl;
        return false;
    }
    if( p1->xRes != p2->xRes )
    {
        std::cout << "xRes does not match" << std::endl;
        return false;
    }
    if( p1->yRes != p2->yRes )
    {
        std::cout << "yRes does not match" << std::endl;
        return false;
    }
    if( p1->xRot != p2->xRot )
    {
        std::cout << "xRot does not match" << std::endl;
        return false;
    }
    if( p1->yRot != p2->yRot )
    {
        std::cout << "yRot does not match" << std::endl;
        return false;
    }
    if( p1->tlX != p2->tlX )
    {
        std::cout << "tlX does not match" << std::endl;
        return false;
    }
    if( p1->wktString != p2->wktString )
    {
        std::cout << "wktString does not match" << std::endl;
        return false;
    }
    // TOOD: xSize and ySize?
    return true;
}

int main()
{
    try
    {
        auto spatialInfo = new kealib::KEAImageSpatialInfo();
        spatialInfo->tlX = 10.0;
        spatialInfo->tlY = 100.0;
        spatialInfo->xRes = 1.0;
        spatialInfo->yRes = -1.0;
        spatialInfo->xRot = 5.0;
        spatialInfo->yRot = 2.0;
        spatialInfo->wktString = "Hello World";

        auto bandDescrips = std::vector<std::string>(2);
        bandDescrips[0] = "Band 1 Name";
        bandDescrips[1] = "Band 2 Name";

        kealib::KEAImageIO io;

        std::string test_kea_file = "bob.kea";

        std::cout << "Creating file" << std::endl;
        HighFive::File *h5file = kealib::KEAImageIO::createKEAImage(test_kea_file,
                        kealib::kea_8uint, IMG_XSIZE, IMG_YSIZE, 2,
                        &bandDescrips, spatialInfo);
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
        if( !compareSpatialInfo(spatialInfo, readinfo))
        {
            return 1;
        }
        
        auto spatialInfo2 = new kealib::KEAImageSpatialInfo();
        spatialInfo->tlX = 11.0;
        spatialInfo->tlY = 110.0;
        spatialInfo->xRes = 1.5;
        spatialInfo->yRes = -1.5;
        spatialInfo->xRot = 5.1;
        spatialInfo->yRot = 2.1;
        spatialInfo->wktString = "Hello World2";
        io.setSpatialInfo(spatialInfo2);
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
        unsigned char *pData = (unsigned char*)calloc(subXSize * subYSize, sizeof(unsigned char));
        for( uint64_t i = 0; i < (subXSize * subYSize); i++ )
        {
            pData[i] = rand() % 255;
        }
        io.writeImageBlock2Band(1, pData, subXOff, subYOff, subXSize, subYSize,
                    subXSize, subYSize, kealib::kea_8uint);
        //free(pData);
        std::cout << "Written some image data" << std::endl;
        
        io.createMask(1, 1);

        std::cout << "Created Mask" << std::endl;
                
        // write some mask data
        io.writeImageBlock2BandMask(1, pData, subXOff, subYOff, subXSize, subYSize,
                    subXSize, subYSize, kealib::kea_8uint);
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

        // Pete: skipping the GCPs for now
        io.setGCPProjection("KmKmqw");
        std::cout << "Wrote GCP Proj" << std::endl;
        
        if ( (io.getImageBandDataType(1) != kealib::kea_8uint) ||
             (io.getImageBandDataType(2) != kealib::kea_8uint))
        {
            std::cout << "Wrong data type read" << std::endl;
            return 1;
        }
        
        if( io.getKEAImageVersion() != kealib::KEA_VERSION)
        {
            std::cout << "wrong kealib version string" << std::endl;
            return 1;
        }
        
        io.close();


        std::cout << "Opening file" << std::endl;
        h5file = kealib::KEAImageIO::openKeaH5RDOnly(test_kea_file);
        io.openKEAImageHeader(h5file);
        std::cout << "Opened file" << std::endl;

        std::cout << "Reading some image data" << std::endl;
        unsigned char *pReadData = (unsigned char*)calloc(subXSize * subYSize, sizeof(unsigned char));
        io.readImageBlock2Band(1, pReadData, subXOff, subYOff, subXSize, subYSize, subXSize, subYSize, kealib::kea_8uint);
        std::cout << "Read some image data" << std::endl;

        std::cout << "Comparing data written and read data" << std::endl;
        for (uint64_t i = 0; i < (subXSize * subYSize); i++)
        {
            if (pReadData[i] != pData[i])
            {
                std::cout << "Error at " << i << std::endl;
                return 1;
            }
        }
        std::cout << "Data compared" << std::endl;
        
        if( io.maskCreated(1) )
        {
            std::cout << "Mask created" << std::endl;
        }
        else
        {
            std::cout << "Mask NOT created" << std::endl;
            return 1;
        }
        
        io.readImageBlock2BandMask(1, pReadData, subXOff, subYOff, subXSize, subYSize, subXSize, subYSize, kealib::kea_8uint);
        std::cout << "Read some mask data" << std::endl;

        std::cout << "Comparing data written and read mask" << std::endl;
        for (uint64_t i = 0; i < (subXSize * subYSize); i++)
        {
            if (pReadData[i] != pData[i])
            {
                std::cout << "Error at " << i << std::endl;
                return 1;
            }
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
        if( u32nodata != 999 )
        {
            std::cout << "Wrong nodata value read" << std::endl;
            return 1;
        }
        // band 2 nodata is undefined
        dnodata = -1;
        io.getNoDataValue(2, &dnodata, kealib::kea_64float);
        if( dnodata != -1 )
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
        
        auto readinfo2 = io.getSpatialInfo();
        if( !compareSpatialInfo(spatialInfo2, readinfo2))
        {
            return 1;
        }
        
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
        
        if( io.getImageBlockSize(1) != 100 ) // smaller than KEA_IMAGE_CHUNK_SIZE
        {
            std::cout << "image block size does not match" << std::endl;
            return 1;
        }
        
        io.close();

        free(pData);
        free(pReadData);
        /*
        io.setImageBandLayerType(1, kealib::kea_thematic);
        kealib::KEAAttributeTable *pRat = io.getAttributeTable(kealib::kea_att_file, 1);
        pRat->addAttIntField(TEST_FIELD, 0);
        size_t colIdx = pRat->getFieldIndex(TEST_FIELD);

        int64_t *pRATData = (int64_t*)calloc(RAT_SIZE, sizeof(int64_t));
        for( int i = 0; i < RAT_SIZE; i++ )
        {
            pRATData[i] = rand() % 100;
        }

        pRat->addRows(RAT_SIZE);
        pRat->setIntFields(0, RAT_SIZE, colIdx, pRATData);

        free(pRATData);
        io.close();
        */
    }
    catch(const kealib::KEAException &e)
    {
        fprintf(stderr, "Exception raised: %s\n", e.what());
        return 1;
    }
    printf("Success\n");

    return 0;
}
