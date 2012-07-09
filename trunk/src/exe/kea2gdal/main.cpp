/*
 *  main.cpp
 *  LibKEA
 *
 *  Created by Pete Bunting on 09/07/2012.
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

#include "gdal_priv.h"

#include "libkea/KEACommon.h"
#include "libkea/KEAException.h"
#include "libkea/KEAImageIO.h"

int main (int argc, char * const argv[]) 
{
    std::string inFilename = "";
    std::string gdalFormat = "";
    std::string outFilename = "";
    if( argc == 4 )
    {
        inFilename = std::string(argv[1]);
        gdalFormat = std::string(argv[2]);
        outFilename = std::string(argv[3]);
    }
    else
    {
        std::cout << "kea2gdal [Input Image] [format] [Output Image]" << std::endl;
    }
    
    GDALAllRegister();
    
    try
    {        
        H5::H5File *keaImgFile = libkea::KEAImageIO::openKeaH5RW(inFilename);
        
        libkea::KEAImageIO *imgIO = new libkea::KEAImageIO();
        imgIO->openKEAImageHeader(keaImgFile);
        
        libkea::KEAImageSpatialInfo *keaSpatInfo = imgIO->getSpatialInfo();
                
        double *trans = new double[6];
        trans[0] = keaSpatInfo->tlX;
        trans[3] = keaSpatInfo->tlY;
        trans[1] = keaSpatInfo->xRes;
        trans[5] = keaSpatInfo->yRes;
        trans[2] = keaSpatInfo->xRot;
        trans[4] = keaSpatInfo->yRot;
        
        libkea::KEADataType keaDataType = imgIO->getImageDataType();
        
        GDALDataType gdalDataType = GDT_Unknown;
        switch( keaDataType )
        {
            case libkea::kea_8int:
            case libkea::kea_8uint:
                gdalDataType = GDT_Byte;
                break;
            case libkea::kea_16int:
                gdalDataType = GDT_Int16;
                break;
            case libkea::kea_32int:
                gdalDataType = GDT_Int32;
                break;
            case libkea::kea_16uint:
                gdalDataType = GDT_UInt16;
                break;
            case libkea::kea_32uint:
                gdalDataType = GDT_UInt32;
                break;
            case libkea::kea_32float:
                gdalDataType = GDT_Float32;
                break;
            case libkea::kea_64float:
                gdalDataType = GDT_Float64;
                break;
            default:
                gdalDataType = GDT_Unknown;
                break;
        }

        
        GDALDriver *gdalDriver = NULL;
        gdalDriver = GetGDALDriverManager()->GetDriverByName(gdalFormat.c_str());
        if(gdalDriver == NULL)
        {
            std::cout << "Driver for " << gdalFormat << " does not exist\n";
            return -1;
        }

        GDALDataset *gdalDataset = gdalDriver->Create(outFilename.c_str(), keaSpatInfo->xSize, keaSpatInfo->ySize, imgIO->getNumOfImageBands(), gdalDataType, NULL);
        if(gdalDataset == NULL)
        {
            std::cout << "Could not create GDALDataset." << outFilename << std::endl;
        }
        
        gdalDataset->SetGeoTransform(trans);
        gdalDataset->SetProjection(keaSpatInfo->wktString.c_str());
        
        float *data = new float[keaSpatInfo->xSize];
        
        int feedback = keaSpatInfo->ySize/10;
        int feedbackCounter = 0;
        
        std::string bandDescrip = "";
        GDALRasterBand *imgBand = NULL;
        for(unsigned int i = 1; i <= imgIO->getNumOfImageBands(); ++i)
        {
            imgBand = gdalDataset->GetRasterBand(i);
            bandDescrip = imgIO->getImageBandDescription(i);
            std::cout << "Processing Band: " << bandDescrip << std::endl;

            imgBand->SetDescription(bandDescrip.c_str());
            
            std::cout << "Started." << std::flush;
            feedbackCounter = 0;
            for(unsigned int n = 0; n < keaSpatInfo->ySize; ++n)
            {
                if((n % feedback) == 0)
				{
                    std::cout << "." << feedbackCounter << "." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
                
                imgIO->readImageBlock2Band(i, data, 0, n, keaSpatInfo->xSize, 1, libkea::kea_32float);
                
                imgBand->RasterIO(GF_Write, 0, n, keaSpatInfo->xSize, 1, data, keaSpatInfo->xSize, 1, GDT_Float32, 0, 0);
            }
            std::cout << ".Complete" << std::endl;
        }
        delete[] data;
        
        imgIO->close();
        GDALClose(gdalDataset);
        GDALDestroyDriverManager();
    }
    catch (libkea::KEAException &e)
    {
        std::cout << e.what() << std::endl;
        return -1;
    }
    
    return 0;
}




