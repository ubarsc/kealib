/*
 *  testseg.cpp
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

#define IMG_XSIZE 20
#define IMG_YSIZE 20
#define HIST_SIZE 100

int main()
{
    try
    {
        kealib::KEAImageIO io;
        H5::H5File *h5file = kealib::KEAImageIO::createKEAImage("segs.kea", 
                        kealib::kea_8uint, IMG_XSIZE, IMG_YSIZE, 1);

        io.openKEAImageHeader(h5file);

        unsigned char *pData = (unsigned char*)calloc(IMG_XSIZE * IMG_YSIZE, sizeof(unsigned char));
        int64_t *pHist = (int64_t*)calloc(HIST_SIZE, sizeof(int64_t));
        
        // make some square segments
        for( int y = 0; y < IMG_YSIZE; y++)
        {
            for( int x = 0; x < IMG_XSIZE; x++)
            {
                unsigned char val = ((y / 10) * 10) + (x / 10);
                pData[(y * IMG_XSIZE) + x] = val; 
                pHist[val]++;
            }
        }
        io.writeImageBlock2Band(1, pData, 0, 0, IMG_XSIZE, IMG_YSIZE, 
                    IMG_XSIZE, IMG_YSIZE, kealib::kea_8uint);
        free(pData);

        io.setImageBandLayerType(1, kealib::kea_thematic);
        kealib::KEAAttributeTable *pRat = io.getAttributeTable(kealib::kea_att_file, 1);
        pRat->addAttIntField("Histogram", 0);
        size_t colIdx = pRat->getFieldIndex("Histogram");

        pRat->addRows(HIST_SIZE);
        pRat->setIntFields(0, HIST_SIZE, colIdx, pHist);

        free(pHist);
        
        uint8_t nodata = 90;
        io.setNoDataValue(1, &nodata, kealib::kea_8uint);
        
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
