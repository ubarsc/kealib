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
    std::string outFilename = "";
    if( argc == 3 )
    {
        inFilename = std::string(argv[1]);
        outFilename = std::string(argv[2]);
    }
    else
    {
        std::cout << "gdal2kea [Input Image] [Output Image]" << std::endl;
    }
    
    GDALAllRegister();
    GDALDataset *gdalDataset = NULL;
    
    gdalDataset = (GDALDataset *) GDALOpenShared(inFilename.c_str(), GA_ReadOnly);
    if(gdalDataset == NULL)
    {
        std::cout << "Could not open image " << inFilename << std::endl;
        return -1;
    }
    
    double *trans = new double[6];
    gdalDataset->GetGeoTransform(trans);
    
    const char *wtkSpatialRef = gdalDataset->GetProjectionRef();
    
    libkea::KEAImageSpatialInfo *keaSpatInfo = new libkea::KEAImageSpatialInfo();
    keaSpatInfo->tlX = trans[0];
    keaSpatInfo->tlY = trans[3];
    keaSpatInfo->xRes = trans[1];
    keaSpatInfo->yRes = trans[5];
    keaSpatInfo->xRot = trans[2];
    keaSpatInfo->yRot = trans[4];
    keaSpatInfo->xSize = gdalDataset->GetRasterXSize();
    keaSpatInfo->ySize = gdalDataset->GetRasterYSize();
    keaSpatInfo->wktString = std::string(wtkSpatialRef);
    
    unsigned int numImgBands = gdalDataset->GetRasterCount();
    
    GDALDataType gdalType = gdalDataset->GetRasterBand(1)->GetRasterDataType();
    libkea::KEADataType keaDataType = libkea::kea_undefined;
    switch( gdalType )
    {
        case GDT_Byte:
            keaDataType = libkea::kea_8uint;
            break;
        case GDT_Int16:
            keaDataType = libkea::kea_16int;
            break;
        case GDT_Int32:
            keaDataType = libkea::kea_32int;
            break;
        case GDT_UInt16:
            keaDataType = libkea::kea_16uint;
            break;
        case GDT_UInt32:
            keaDataType = libkea::kea_32uint;
            break;
        case GDT_Float32:
            keaDataType = libkea::kea_32float;
            break;
        case GDT_Float64:
            keaDataType = libkea::kea_64float;
            break;
        default:
            keaDataType = libkea::kea_undefined;
            break;
    }

    try
    {
        H5::H5File *keaImgFile = libkea::KEAImageIO::createKEAImage(outFilename, keaDataType, keaSpatInfo->xSize, keaSpatInfo->ySize, numImgBands, NULL, NULL);//keaSpatInfo);
        
        libkea::KEAImageIO *imgIO = new libkea::KEAImageIO();
        imgIO->openKEAImageHeader(keaImgFile);
        
        imgIO->setSpatialInfo(keaSpatInfo);
        
        int *data = new int[keaSpatInfo->xSize];
        
        int feedback = keaSpatInfo->ySize/10;
        int feedbackCounter = 0;
        
        const char *bandDescripChar = NULL;
        std::string bandDescrip = "";
        GDALRasterBand *imgBand = NULL;
        for(unsigned int i = 1; i <= numImgBands; ++i)
        {
            imgBand = gdalDataset->GetRasterBand(i);
            bandDescripChar = imgBand->GetDescription();
            if(bandDescripChar != NULL)
            {
                bandDescrip = std::string(bandDescripChar);
            }
            else
            {
                bandDescrip = "Band " + libkea::uint2Str(i);
            }
            std::cout << "Processing Band: " << bandDescrip << std::endl;
            imgIO->setImageBandDescription(i, bandDescrip);
            
            std::cout << "Started." << std::flush;
            feedbackCounter = 0;
            for(unsigned int n = 0; n < keaSpatInfo->ySize; ++n)
            {
                if((n % feedback) == 0)
				{
                    std::cout << "." << feedbackCounter << "." << std::flush;
					feedbackCounter = feedbackCounter + 10;
				}
                imgBand->RasterIO(GF_Read, 0, n, keaSpatInfo->xSize, 1, data, keaSpatInfo->xSize, 1, GDT_Int32, 0, 0);
                imgIO->writeImageBlock2Band(i, data, 0, n, keaSpatInfo->xSize, 1, libkea::kea_32int);
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




