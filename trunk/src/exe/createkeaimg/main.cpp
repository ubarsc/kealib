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

int main (int argc, char * const argv[]) 
{
    try 
    {
        H5::H5File *keaImgFile = libkea::KEAImageIO::createKEAImage("/Users/pete/Desktop/TestImage.kea", libkea::kea_32float, 1000, 2000, 3);
        
        libkea::KEAImageIO imageIO;
        imageIO.openKEAImage(keaImgFile);
        
                
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
        imageIO.writeImageBlock2Band(1, data, 0, 0, 100, 200, libkea::kea_32float);
        imageIO.writeImageBlock2Band(2, data, 100, 200, 100, 200, libkea::kea_32float);
        imageIO.writeImageBlock2Band(3, data, 200, 400, 100, 200, libkea::kea_32float);
        std::cout << "Written Image data\n";
        
        imageIO.setImageMetaData("TEST", "Hello World");
        imageIO.setImageBandMetaData(1, "TEST", "Hello World");
        
        std::cout << "TEST META-DATA: " << imageIO.getImageMetaData("TEST") << std::endl;
        std::cout << "DESCRIPTION 1: " << imageIO.getImageBandDescription(1) << std::endl;
        std::cout << "DESCRIPTION 2: " << imageIO.getImageBandDescription(2) << std::endl;
        std::cout << "DESCRIPTION 3: " << imageIO.getImageBandDescription(3) << std::endl;
        
        
        for(unsigned int i = 0; i < (100*200); ++i)
        {
            data[i] = 0;
        }
        
        imageIO.readImageBlock2Band(1, data, 50, 100, 100, 200, libkea::kea_32float);
        
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
        
        delete[] data;
        
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
    } 
    catch (libkea::KEAException &e) 
    {
        std::cerr << "ERROR: " << e.what() << std::endl;
    }
    
}




