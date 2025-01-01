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

#define IMG_XSIZE 10
#define IMG_YSIZE 20
#define TEST_FIELD "test"
#define RAT_SIZE 256

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

        std::cout << "Opening file" << std::endl;
        h5file = kealib::KEAImageIO::openKeaH5RW(test_kea_file);
        std::cout << "Openned file" << std::endl;

        io.openKEAImageHeader(h5file);

        /*
        unsigned char *pData = (unsigned char*)calloc(IMG_XSIZE * IMG_YSIZE, sizeof(unsigned char));
        for( int i = 0; i < (IMG_XSIZE * IMG_YSIZE); i++ )
        {
            pData[i] = rand() % 255;
        }
        io.writeImageBlock2Band(1, pData, 0, 0, IMG_XSIZE, IMG_YSIZE, 
                    IMG_XSIZE, IMG_YSIZE, kealib::kea_8uint);
        free(pData);

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
