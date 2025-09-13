/*
 *  testsupport.h
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
 
 #ifndef TESTSUPPORT_H
 #define TESTSUPPORT_H
 
bool compareSpatialInfo(kealib::KEAImageSpatialInfo *p1, kealib::KEAImageSpatialInfo *p2);
bool compareGCPs(std::vector<kealib::KEAImageGCP*>* g1, std::vector<kealib::KEAImageGCP*>* g2);
kealib::KEAImageSpatialInfo getSpatialInfo(double version);
kealib::KEADataType CTypeStringToKEAType(const std::string &s);
std::vector<kealib::KEAImageGCP*>* getGCPData();
void freeGCPData(std::vector<kealib::KEAImageGCP*> *pGCPS);

template <typename T>
T* createDataForType(uint64_t xSize, uint64_t ySize)
{
    T *pData = (T*)calloc(xSize * ySize, sizeof(T));
    double dMin = std::numeric_limits<uint8_t>::min();
    double dMax = std::numeric_limits<uint8_t>::max();
    double dMult = (dMax - dMin) / (xSize * ySize);
    for( uint64_t x = 0; x < xSize; x++ )
    {
        for( uint64_t y = 0; y < ySize; y++ )
        {
            pData[(y * xSize) + x] = dMin + (y * x) * dMult;
        }
    }
    
    return pData;
}

template <typename T>
bool compareData(T *p1, T *p2, uint64_t xSize, uint64_t ySize)
{
    for( uint64_t x = 0; x < xSize; x++ )
    {
        for( uint64_t y = 0; y < ySize; y++ )
        {
            uint64_t idx = (y * xSize) + x;
            if( p1[idx] != p2[idx])
            {
                std::cout << "Error comparing values at" << x << "," << y << std::endl;
                return false;
            }
        }
    }
    return true;
}

template <typename T>
bool compareDataSubset(T *p1, T *pSubset, uint64_t xOff, uint64_t yOff, 
    uint64_t xSize, uint64_t ySize, uint64_t xSubsetSize, uint64_t ySubsetSize)
{
    for( uint64_t x = 0; x < xSubsetSize; x++ )
    {
        for( uint64_t y = 0; y < ySubsetSize; y++ )
        {
            uint64_t idx_full = ((y + yOff) * xSize) + (x + xOff);
            uint64_t idx_subset = (y * xSubsetSize) + x;
            
            if( p1[idx_full] != pSubset[idx_subset])
            {
                std::cout << "Error comparing values at" << x << "," << y << std::endl;
                return false;
            }
        }
    }
    return true;
}

const uint64_t IMG_XSIZE = 600;
const uint64_t IMG_YSIZE = 700;
const uint64_t OV_XSIZE = 300;
const uint64_t OV_YSIZE = 350;
const uint64_t OV2_XSIZE = 25;
const uint64_t OV2_YSIZE = 50;
const std::string TEST_FIELD = "test";
const uint64_t RAT_SIZE = 256;
#define STRINGIFY(x) XSTRINGIFY(x)
#define XSTRINGIFY(x) #x

#endif //TESTSUPPORT_H
