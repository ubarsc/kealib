/*
 *  main.cpp
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

#include <iostream>
#include <sstream>
#include <string>

#include "H5Cpp.h"

#include "libkea/KEACommon.h"
#include "libkea/KEAException.h"
#include "libkea/KEAImageIO.h"
#include "libkea/KEAAttributeTable.h"

int main (int argc, char * const argv[]) 
{
    std::string sFilename;
    if( argc == 2 )
    {
        sFilename = argv[1];
    }
    else
    {
        sFilename = "/Users/pete/Desktop/TestImage.kea";
    }

    try 
    {
        H5::H5File *keaImgFile = libkea::KEAImageIO::createKEAImage(sFilename, libkea::kea_32float, 1000, 2000, 3);
        
        //keaImgFile->close();
        //keaImgFile = libkea::KEAImageIO::openKeaH5RW(sFilename);
        
        
        libkea::KEAImageIO imageIO;
        imageIO.openKEAImageHeader(keaImgFile);
        
                
        float *data = new float[100*200];
        for(unsigned int i = 0; i < 200; ++i)
        {
            //data[i] = i;
            for(unsigned int j = 0; j < 100; ++j)
            {
                data[(i*100)+j] = j; 
            }
        }
        
        std::cout << "Write Image data\n";
        imageIO.writeImageBlock2Band(1, data, 0, 0, 100, 200, 100, 200, libkea::kea_32float);
        imageIO.writeImageBlock2Band(2, data, 100, 200, 100, 200, 100, 200, libkea::kea_32float);
        imageIO.writeImageBlock2Band(3, data, 200, 400, 100, 200, 100, 200, libkea::kea_32float);
        std::cout << "Written Image data\n";
        
        float *dataEdge = new float[100*100];
        for(unsigned int i = 0; i < 50; ++i)
        {
            for(unsigned int j = 0; j < 50; ++j)
            {
                dataEdge[(i*100)+j] = j; 
            }
        }
        
        imageIO.writeImageBlock2Band(1, dataEdge, 950, 1950, 50, 50, 100, 100, libkea::kea_32float);
        delete[] dataEdge;
        
        float *dataEdgeRead = new float[100*100];
        for(unsigned int i = 0; i < 100; ++i)
        {
            for(unsigned int j = 0; j < 100; ++j)
            {
                dataEdge[(i*100)+j] = 0; 
            }
        }
        
        imageIO.readImageBlock2Band(1, dataEdge, 950, 1950, 50, 50, 100, 100, libkea::kea_32float);
        
        for(unsigned int i = 0; i < 100; ++i)
        {
            for(unsigned int j = 0; j < 100; ++j)
            {
                if(j == 0)
                {
                    std::cout << dataEdgeRead[(i*100)+j]; 
                }
                else
                {
                    std::cout << "," << dataEdgeRead[(i*100)+j]; 
                }
            }
            std::cout << std::endl;
        }
        delete[] dataEdgeRead;
        
        imageIO.setImageMetaData("TEST1", "Hello World 1");
        imageIO.setImageMetaData("TEST2", "Hello World 2");
        imageIO.setImageMetaData("TEST3", "Hello World 3");
        imageIO.setImageMetaData("TEST4", "Hello World 4");
        imageIO.setImageMetaData("TEST5", "Hello World 5");
        imageIO.setImageMetaData("TEST6", "Hello World 6");
        imageIO.setImageMetaData("TEST7", "Hello World 7");
        imageIO.setImageBandMetaData(1, "TEST1", "Hello World 1 on Band");
        imageIO.setImageBandMetaData(1, "TEST2", "Hello World 2 on Band");
        imageIO.setImageBandMetaData(1, "TEST3", "Hello World 3 on Band");
        imageIO.setImageBandMetaData(1, "TEST4", "Hello World 4 on Band");
        imageIO.setImageBandMetaData(1, "TEST5", "Hello World 5 on Band");
        imageIO.setImageBandMetaData(1, "TEST6", "Hello World 6 on Band");
        
        std::cout << "TEST META-DATA Image: " << imageIO.getImageMetaData("TEST1") << std::endl;
        std::cout << "TEST META-DATA Band: " << imageIO.getImageBandMetaData(1, "TEST1") << std::endl;
        std::cout << "DESCRIPTION 1: " << imageIO.getImageBandDescription(1) << std::endl;
        std::cout << "DESCRIPTION 2: " << imageIO.getImageBandDescription(2) << std::endl;
        std::cout << "DESCRIPTION 3: " << imageIO.getImageBandDescription(3) << std::endl;
        
        std::cout << "Number of Bands: " << imageIO.getNumOfImageBands() << std::endl;
        
        std::cout << "Image Data Type: " << imageIO.getImageBandDataType(1) << std::endl;
        std::cout << "Image Data Type: " << getDataTypeAsStr(imageIO.getImageBandDataType(1)) << std::endl;
        
        int noDataVal = 13;
        
        imageIO.setNoDataValue(1, &noDataVal, libkea::kea_32int);
        std::cout << "No Data value being set to " << noDataVal << std::endl;
        
        float noDataValRead = -1;
        imageIO.getNoDataValue(1, &noDataValRead, libkea::kea_32float);
        std::cout << "No Data value read as " << noDataVal << std::endl;
        
        noDataVal = 2300;
        
        imageIO.setNoDataValue(1, &noDataVal, libkea::kea_32int);
        std::cout << "No Data value being set to " << noDataVal << std::endl;
        
        noDataValRead = -1;
        imageIO.getNoDataValue(1, &noDataValRead, libkea::kea_32float);
        std::cout << "No Data value read as " << noDataVal << std::endl;
        
        
        std::vector<std::string> bandMetaDataNames =  imageIO.getImageBandMetaDataNames(1);
        std::cout << "Image Band 1 Meta Data Names: \n";
        for(std::vector<std::string>::iterator iterNames = bandMetaDataNames.begin(); iterNames != bandMetaDataNames.end(); ++iterNames)
        {
            std::cout << "\t" << *iterNames << std::endl;
        }
        
        std::vector<std::string> imgMetaDataNames =  imageIO.getImageMetaDataNames();
        std::cout << "Image Meta Data Names: \n";
        for(std::vector<std::string>::iterator iterNames = imgMetaDataNames.begin(); iterNames != imgMetaDataNames.end(); ++iterNames)
        {
            std::cout << "\t" << *iterNames << std::endl;
        }
        
        std::vector< std::pair<std::string, std::string> > imgBandMetaData = imageIO.getImageBandMetaData(1);
        std::cout << "Image Band Meta Data: \n";
        for(std::vector< std::pair<std::string, std::string> >::iterator iterMetaData = imgBandMetaData.begin(); iterMetaData != imgBandMetaData.end(); ++iterMetaData)
        {
            std::cout << "\t" << iterMetaData->first << " = " << iterMetaData->second << std::endl;
        }
        
        std::vector< std::pair<std::string, std::string> > imgMetaData = imageIO.getImageMetaData();
        std::cout << "Image Meta Data: \n";
        for(std::vector< std::pair<std::string, std::string> >::iterator iterMetaData = imgMetaData.begin(); iterMetaData != imgMetaData.end(); ++iterMetaData)
        {
            std::cout << "\t" << iterMetaData->first << " = " << iterMetaData->second << std::endl;
        }
        
        std::vector< std::pair<std::string, std::string> > newMetaData;
        
        newMetaData.push_back(std::pair<std::string, std::string>("min1", "4"));
        newMetaData.push_back(std::pair<std::string, std::string>("max1", "4543"));
        newMetaData.push_back(std::pair<std::string, std::string>("min2", "45"));
        newMetaData.push_back(std::pair<std::string, std::string>("max2", "67546"));
        newMetaData.push_back(std::pair<std::string, std::string>("min3", "12"));
        newMetaData.push_back(std::pair<std::string, std::string>("max3", "900"));
        
        imageIO.setImageBandMetaData(2, newMetaData);
        imageIO.setImageMetaData(newMetaData);
        
        std::cout << "Creating overview images\n";
        imageIO.createOverview(1, 0, 500, 1000);
        imageIO.createOverview(1, 1, 250, 500);
        imageIO.createOverview(1, 1, 500, 500);
        
        imageIO.createOverview(2, 0, 500, 1000);
        imageIO.createOverview(2, 1, 250, 500);
        
        imageIO.createOverview(3, 0, 500, 1000);
        imageIO.createOverview(3, 1, 250, 500);
        
        std::cout << "Overview 1 Band 3 block size: " << imageIO.getOverviewBlockSize(3, 1) << std::endl;
        
        std::cout << "There are " << imageIO.getNumOfOverviews(1) << " overviews for band 1\n";
        
        unsigned long xSizeOverview = 0;
        unsigned long ySizeOverview = 0;
        
        imageIO.getOverviewSize(2, 1, &xSizeOverview, &ySizeOverview);
        
        std::cout << "Image overview 1 for band 2 has size [" << xSizeOverview << "," << ySizeOverview << "]\n";
        
        for(unsigned int i = 0; i < (100*200); ++i)
        {
            data[i] = 0;
        }
        
        imageIO.readImageBlock2Band(1, data, 50, 100, 100, 200, 100, 200, libkea::kea_32float);
        /*
        for(unsigned int i = 0; i < 200; ++i)
        {
            for(unsigned int j = 0; j < 100; ++j)
            {
                if(j == 0)
                {
                    std::cout << data[(i*100)+j];
                }
                else
                {
                    std::cout << "," << data[(i*100)+j];
                }
            }
            std::cout << std::endl;
        }
         */
        
        delete[] data;
        
        std::cout << "Creating attribute table\n";
        libkea::KEAAttributeTable *attTable = imageIO.getAttributeTable(libkea::kea_att_mem, 1);
        std::cout << "Got attribute table\n";
        std::cout << "Adding rows\n";
        attTable->addRows(2500);
        std::cout << "Rows added\n";
        attTable->addAttFloatField("Float Field 1", 0, "Red");
        attTable->addAttIntField("Int Field 1", 0);
        attTable->addAttFloatField("Float Field 2", 1, "Green");
        attTable->addAttFloatField("Float Field 3", 2, "Blue");
        attTable->addAttIntField("Int Field 2", 1);
        attTable->addAttStringField("String Field 1", "xxx");
        attTable->addAttStringField("String Field 2", "yyy");
        attTable->addAttBoolField("Bool Field 1", false);
        attTable->addAttBoolField("Bool Field 2", true);
        
        attTable->setFloatField(10, "Float Field 1", 10);
        
        attTable->setFloatField(2, "Float Field 2", 99);
        
        attTable->setStringField(99, "String Field 2", "Hello World");
        
        std::cout << "FID 45: Field \'Float Field 2\' = " << attTable->getFloatField(45, "Float Field 2") << "\n";
        
        libkea::KEAATTFeature *keaFeat = attTable->getFeature(10);
        keaFeat->neighbours->push_back(9);
        keaFeat->neighbours->push_back(3);
        keaFeat->neighbours->push_back(15);
        
        keaFeat = attTable->getFeature(40);
        keaFeat->neighbours->push_back(41);
        keaFeat->neighbours->push_back(39);
        keaFeat->neighbours->push_back(48);

        keaFeat = attTable->getFeature(1300);
        keaFeat->neighbours->push_back(1301);
        keaFeat->neighbours->push_back(1320);
        keaFeat->neighbours->push_back(1299);
        
        /*
        size_t numRows = attTable->getSize();
        libkea::KEAATTFeature *feat = NULL;
        for(size_t i = 0; i < numRows; ++i)
        {
            feat = attTable->getFeature(i);
            std::cout << "FID: " << feat->fid << std::endl;
            std::cout << "\t" << feat->intFields->at(0) << ", " << feat->intFields->at(1) << std::endl;
            std::cout << "\t" << feat->floatFields->at(0) << ", " << feat->floatFields->at(1) << ", " << feat->floatFields->at(2) << std::endl;
            std::cout << "\t" << feat->strFields->at(0) << ", " << feat->strFields->at(1) << std::endl;
            std::cout << "\t";
            if(feat->boolFields->at(0))
            {
                std::cout << "TRUE";
            }
            else
            {
                std::cout << "FALSE";
            }
            if(feat->boolFields->at(1))
            {
                std::cout << ", TRUE\n";
            }
            else
            {
                std::cout << ", FALSE\n";
            }
            std::cout << "\tNeighbours: ";
            for(size_t j = 0; j < feat->neighbours->size(); ++j)
            {
                if(j == 0)
                {
                    std::cout << feat->neighbours->at(j);
                }
                else
                {
                    std::cout << ", " << feat->neighbours->at(j);
                }
            }
            std::cout << std::endl;
        }
        */
                
        imageIO.setAttributeTable(attTable, 1);
        imageIO.setAttributeTable(attTable, 2);
        
        imageIO.close();
        
        keaImgFile->close();
        
        if(libkea::KEAImageIO::isKEAImage("/Users/pete/Desktop/TestImage.kea"))
        {
            std::cout << "FOUND KEA IMAGE - THIS IS CORRECT A KEA IMAGE WAS PROVIDED!\n";
        }
        else
        {
            std::cout << "DID NOT FIND KEA IMAGE - THIS IS **NOT** CORRECT A KEA IMAGE WAS PROVIDED!\n";
        }
        
        
        if(libkea::KEAImageIO::isKEAImage("/Users/pete/Desktop/LI080204_RAW9_1m_pmfgrd_dtm.img"))
        {
            std::cout << "FOUND KEA IMAGE - THIS IS **NOT** CORRECT A IMG IMAGE WAS PROVIDED!\n";
        }
        else
        {
            std::cout << "DID NOT FIND KEA IMAGE - THIS IS CORRECT A IMG IMAGE WAS PROVIDED!\n";
        }
        
        if(libkea::KEAImageIO::isKEAImage("/Users/pete/Desktop/LI080220_RAW17_10m_pmfgrd_h.spd"))
        {
            std::cout << "FOUND KEA IMAGE - THIS IS **NOT** CORRECT A SPD IMAGE WAS PROVIDED!\n";
        }
        else
        {
            std::cout << "DID NOT FIND KEA IMAGE - THIS IS CORRECT A SPD IMAGE WAS PROVIDED!\n";
        }
        
        keaImgFile = libkea::KEAImageIO::openKeaH5RW(sFilename);
        
        imageIO.openKEAImageHeader(keaImgFile);
        libkea::KEAAttributeTable *readAtt = imageIO.getAttributeTable(libkea::kea_att_mem, 1);
        /*
        size_t numRows1 = readAtt->getSize();
        libkea::KEAATTFeature *feat = NULL;
        for(size_t i = 0; i < numRows1; ++i)
        {
            feat = attTable->getFeature(i);
            std::cout << "FID: " << feat->fid << std::endl;
            std::cout << "\t" << feat->intFields->at(0) << ", " << feat->intFields->at(1) << std::endl;
            std::cout << "\t" << feat->floatFields->at(0) << ", " << feat->floatFields->at(1) << ", " << feat->floatFields->at(2) << std::endl;
            std::cout << "\t" << feat->strFields->at(0) << ", " << feat->strFields->at(1) << std::endl;
            std::cout << "\t";
            if(feat->boolFields->at(0))
            {
                std::cout << "TRUE";
            }
            else
            {
                std::cout << "FALSE";
            }
            if(feat->boolFields->at(1))
            {
                std::cout << ", TRUE\n";
            }
            else
            {
                std::cout << ", FALSE\n";
            }
            std::cout << "\tNeighbours: ";
            for(size_t j = 0; j < feat->neighbours->size(); ++j)
            {
                if(j == 0)
                {
                    std::cout << feat->neighbours->at(j);
                }
                else
                {
                    std::cout << ", " << feat->neighbours->at(j);
                }
            }
            std::cout << std::endl;
        }
        */
        
        readAtt->addAttStringField("third field", "new value");
        readAtt->addAttFloatField("another string field", 50.3);
        
        imageIO.setAttributeTable(readAtt, 1);
        
        std::cout << "Global Column 0 is \'" << readAtt->getField(0).name << "\'" <<  std::endl;
        std::cout << "Global Column 2 is \'" << readAtt->getField(2).name << "\'" <<  std::endl;
        std::cout << "Global Column 4 is \'" << readAtt->getField(4).name << "\'" <<  std::endl;
        std::cout << "Global Column 1 is \'" << readAtt->getField(1).name << "\'" <<  std::endl;
        std::cout << "Global Column 7 is \'" << readAtt->getField(7).name << "\'" <<  std::endl;
        std::cout << "Global Column 9 is \'" << readAtt->getField(9).name << "\'" <<  std::endl;
        
        imageIO.close();
    } 
    catch (libkea::KEAException &e) 
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
    
}




