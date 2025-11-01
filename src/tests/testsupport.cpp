/*
 *  testsupport.cpp
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

#include <stdlib.h>
#include <limits>
#include <iostream>
#include "libkea/KEAImageIO.h"

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

bool compareGCPs(std::vector<kealib::KEAImageGCP*>* g1, std::vector<kealib::KEAImageGCP*>* g2)
{
    if( g1->size() != g2->size() )
    {
        std::cout << "count of gcps don't match" << std::endl;
        return false;
    }
    
    for( size_t n = 0; n < g1->size(); n++ )
    {
        kealib::KEAImageGCP* e1 = g1->at(n);
        kealib::KEAImageGCP* e2 = g2->at(n);
        if(e1->pszId != e2->pszId)
        {
            std::cout << "pszId does not match" << std::endl;
            return false;
        }
        if(e1->pszInfo != e2->pszInfo)
        {
            std::cout << "pszInfo does not match" << std::endl;
            return false;
        }
        if(e1->dfGCPPixel != e2->dfGCPPixel)
        {
            std::cout << "dfGCPPixel does not match" << std::endl;
            return false;
        }
        if(e1->dfGCPLine != e2->dfGCPLine)
        {
            std::cout << "dfGCPLine does not match" << std::endl;
            return false;
        }
        if(e1->dfGCPX != e2->dfGCPX)
        {
            std::cout << "dfGCPX does not match" << std::endl;
            return false;
        }
        if(e1->dfGCPY != e2->dfGCPY)
        {
            std::cout << "dfGCPY does not match" << std::endl;
            return false;
        }
        if(e1->dfGCPZ != e2->dfGCPZ)
        {
            std::cout << "dfGCPZ does not match" << std::endl;
            return false;
        }
    }
    
    return true;
}

kealib::KEAImageSpatialInfo getSpatialInfo(double version)
{
    kealib::KEAImageSpatialInfo spatialInfo;
    spatialInfo.tlX = 10.0 + version;
    spatialInfo.tlY = 100.0 + version;
    spatialInfo.xRes = 1.0 + version;
    spatialInfo.yRes = -1.0 + version;
    spatialInfo.xRot = 5.0 + version;
    spatialInfo.yRot = 2.0 + version;
    std::ostringstream stringStream;
    stringStream << "Hello World" << version;
    spatialInfo.wktString = stringStream.str();
    return spatialInfo;
}

kealib::KEADataType CTypeStringToKEAType(const std::string &s)
{
    if( s == "uint8_t")
        return kealib::kea_8uint;
    else if( s == "int8_t")
        return kealib::kea_8int;
    else if( s == "uint16_t")
        return kealib::kea_16uint;
    else if( s == "int16_t")
        return kealib::kea_16int;
    else if( s == "uint32_t")
        return kealib::kea_32uint;
    else if( s == "int32_t")
        return kealib::kea_32int;
    else if( s == "uint64_t")
        return kealib::kea_64uint;
    else if( s == "int64_t")
        return kealib::kea_64int;
    else if( s == "float")
        return kealib::kea_32float;
    else if( s == "double")
        return kealib::kea_64float;
    else
        throw kealib::KEAIOException("Unable to find KEA type");
}

std::vector<kealib::KEAImageGCP*>* getGCPData()
{
    std::vector<kealib::KEAImageGCP*> *pGCPS = new std::vector<kealib::KEAImageGCP*>();
    kealib::KEAImageGCP *pgcp1 = new kealib::KEAImageGCP();
    pgcp1->pszId = "id1";
    pgcp1->pszInfo = "info1";
    pgcp1->dfGCPPixel = 10.1;
    pgcp1->dfGCPLine = 29.3;
    pgcp1->dfGCPX = 10090.0;
    pgcp1->dfGCPY = -231122.1;
    pgcp1->dfGCPZ = 99.1;
    pGCPS->push_back(pgcp1);
    kealib::KEAImageGCP *pgcp2 = new kealib::KEAImageGCP();
    pgcp2->pszId = "id1";
    pgcp2->pszInfo = "info1";
    pgcp2->dfGCPPixel = 11.1;
    pgcp2->dfGCPLine = 24.3;
    pgcp2->dfGCPX = 10091.0;
    pgcp2->dfGCPY = -231111.1;
    pgcp2->dfGCPZ = 90.1;
    pGCPS->push_back(pgcp2);
    return pGCPS;
}

void freeGCPData(std::vector<kealib::KEAImageGCP*> *pGCPS)
{
    for( auto itr = pGCPS->begin(); itr != pGCPS->end(); itr++)
    {
        delete (*itr);
    }
    delete pGCPS;
}

bool compareRatConstantString(std::vector<std::string> *psBuffer, const std::string &val)
{
    for( auto itr = psBuffer->begin(); itr != psBuffer->end(); itr++ )
    {
        if( (*itr) != val )
        {
            return false;
        }
    }
    return true;
}

bool compareRatDataString(std::vector<std::string> *psBuffer1, std::vector<std::string> *psBuffer2)
{
    if( psBuffer1->size() != psBuffer2->size())
    {
        std::cout << "buffers are different sizes" << std::endl;
        return false;
    }
    
    for( uint64_t i = 0; i < psBuffer1->size(); i++)
    {
        if( psBuffer1->at(i) != psBuffer2->at(i) )
        {
            std::cout << "values differ at index " << i << std::endl;
            return false;
        }
    }
    return true;
}

bool compareRatDataStringSubset(std::vector<std::string> *psBuffer1, std::vector<std::string> *psBuffer2, uint64_t subsetoffset)
{
    for( uint64_t i = 0; i < psBuffer2->size(); i++)
    {
        if( psBuffer1->at(subsetoffset + i) != psBuffer2->at(i) )
        {
            std::cout << "values differ at index " << i << std::endl;
            return false;
        }
    }
    return true;
}

void createRatDataForString(std::vector<std::string> *psBuffer)
{
    uint64_t i = 0;
    while( i < psBuffer->size() )
    {
        char c = 'a' + (i % ('z' - 'a'));
        psBuffer->at(i) = std::string(1, c);
        i++;
    }
}

