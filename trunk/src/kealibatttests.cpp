/*
 *  kealibatttests.cpp
 *  LibKEA
 *
 *  Created by Pete Bunting on 08/11/2013.
 *  Copyright 2013 LibKEA. All rights reserved.
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
#include <string>
#include <string.h>

#include "libkea/KEACommon.h"
#include "libkea/KEAImageIO.h"
#include "libkea/KEAAttributeTable.h"
#include "libkea/KEAAttributeTableFile.h"

#include "kealibatttests.h"

namespace keatest{
    
    
    KEATests::KEATests()
    {
        
    }
    
    std::string KEATests::getAttColTypeStr(kealib::KEAFieldDataType dataType)
    {
        std::string strDT = "";
        if(dataType == kealib::kea_att_na)
        {
            strDT = "NA";
        }
        else if(dataType == kealib::kea_att_bool)
        {
            strDT = "Bool";
        }
        else if(dataType == kealib::kea_att_int)
        {
            strDT = "Int";
        }
        else if(dataType == kealib::kea_att_float)
        {
            strDT = "Float";
        }
        else if(dataType == kealib::kea_att_string)
        {
            strDT = "String";
        }
        else
        {
            strDT = "???????????";
        }
        
        
        return strDT;
    }
    
    void KEATests::readIntColumns(std::string filename, std::string columnName, bool inMem)
    {
        H5::H5File *keaFile = kealib::KEAImageIO::openKeaH5RDOnly(filename);
        kealib::KEAImageIO keaIO;
        keaIO.openKEAImageHeader(keaFile);
        std::cout << "Image has " << keaIO.getNumOfImageBands() << " image bands\n";
        std::cout << "Image is a KEA file of version " << keaIO.getKEAImageVersion() << std::endl;
        
        if(keaIO.attributeTablePresent(1))
        {
            std::cout << "File has an attribute table\n";
            kealib::KEAAttributeTable *att = NULL;
            if(inMem)
            {
                att = keaIO.getAttributeTable(kealib::kea_att_mem, 1);
            }
            else
            {
                att = keaIO.getAttributeTable(kealib::kea_att_file, 1);
            }
            
            size_t sizeOfAtt = att->getSize();
            std::cout << "There are " << sizeOfAtt << " rows\n";
            
            std::cout << "There are " << att->getNumBoolFields() << " Bool fields\n";
            std::cout << "There are " << att->getNumIntFields() << " Int fields\n";
            std::cout << "There are " << att->getNumFloatFields() << " Float fields\n";
            std::cout << "There are " << att->getNumStringFields() << " String fields\n";
            
            kealib::KEAATTField field = att->getField(columnName);
            
            std::cout << "Field Name = " << field.name << std::endl;
            std::cout << "Field Idx = " << field.idx << std::endl;
            std::cout << "Field Usage = " << field.usage << std::endl;
            std::cout << "Field Col Num = " << field.colNum << std::endl;
            std::cout << "Field Type = " << this->getAttColTypeStr(field.dataType) << std::endl;
            
            unsigned int colIdx = field.idx;
            if(inMem)
            {
                for(size_t i = 0; i < sizeOfAtt; ++i)
                {
                    std::cout << i << " = " << att->getIntField(i, colIdx) << std::endl;
                }
            }
            else
            {
                for(int n = 0; n < 10000; ++n)
                {
                    int64_t *pnBuffer = new int64_t[sizeOfAtt];
                    att->getIntFields(0, sizeOfAtt, colIdx, pnBuffer);
                    
                    for(size_t i = 0; i < sizeOfAtt; ++i)
                    {
                        //std::cout << i << " = " << pnBuffer[i] << std::endl;
                    }
                    
                    delete[] pnBuffer;
                }
            }
            
            int64_t startIdx = 1500;
            int64_t numRead = 3000;
            if(inMem)
            {
                for(size_t i = 0; i < numRead; ++i)
                {
                    std::cout << (i+startIdx) << " = " << att->getIntField((i+startIdx), colIdx) << std::endl;
                }
            }
            else
            {
                for(int n = 0; n < 10000; ++n)
                {
                    int64_t *pnBuffer = new int64_t[numRead];
                    att->getIntFields(startIdx, numRead, colIdx, pnBuffer);
                    
                    for(size_t i = 0; i < numRead; ++i)
                    {
                        //std::cout << (i+startIdx) << " = " << pnBuffer[i] << std::endl;
                    }
                    
                    delete[] pnBuffer;
                }
            }
            
        }
        else
        {
            std::cout << "There is no attribute table present\n";
        }
    }
    
    void KEATests::readFloatColumns(std::string filename, std::string columnName, bool inMem)
    {
        H5::H5File *keaFile = kealib::KEAImageIO::openKeaH5RDOnly(filename);
        kealib::KEAImageIO keaIO;
        keaIO.openKEAImageHeader(keaFile);
        std::cout << "Image has " << keaIO.getNumOfImageBands() << " image bands\n";
        std::cout << "Image is a KEA file of version " << keaIO.getKEAImageVersion() << std::endl;
        
        if(keaIO.attributeTablePresent(1))
        {
            std::cout << "File has an attribute table\n";
            kealib::KEAAttributeTable *att = NULL;
            if(inMem)
            {
                att = keaIO.getAttributeTable(kealib::kea_att_mem, 1);
            }
            else
            {
                att = keaIO.getAttributeTable(kealib::kea_att_file, 1);
            }
            
            size_t sizeOfAtt = att->getSize();
            std::cout << "There are " << sizeOfAtt << " rows\n";
            
            std::cout << "There are " << att->getNumBoolFields() << " Bool fields\n";
            std::cout << "There are " << att->getNumIntFields() << " Int fields\n";
            std::cout << "There are " << att->getNumFloatFields() << " Float fields\n";
            std::cout << "There are " << att->getNumStringFields() << " String fields\n";
            
            kealib::KEAATTField field = att->getField(columnName);
            
            std::cout << "Field Name = " << field.name << std::endl;
            std::cout << "Field Idx = " << field.idx << std::endl;
            std::cout << "Field Usage = " << field.usage << std::endl;
            std::cout << "Field Col Num = " << field.colNum << std::endl;
            std::cout << "Field Type = " << this->getAttColTypeStr(field.dataType) << std::endl;
            
            unsigned int colIdx = field.idx;
            if(inMem)
            {
                for(size_t i = 0; i < sizeOfAtt; ++i)
                {
                    std::cout << i << " = " << att->getFloatField(i, colIdx) << std::endl;
                }
            }
            else
            {
                for(int n = 0; n < 10000; ++n)
                {
                    double *pnBuffer = new double[sizeOfAtt];
                    att->getFloatFields(0, sizeOfAtt, colIdx, pnBuffer);
                    
                    for(size_t i = 0; i < sizeOfAtt; ++i)
                    {
                        //std::cout << i << " = " << pnBuffer[i] << std::endl;
                    }
                    
                    delete[] pnBuffer;
                }
            }
            
            int64_t startIdx = 1500;
            int64_t numRead = 3000;
            if(inMem)
            {
                for(size_t i = 0; i < numRead; ++i)
                {
                    std::cout << (i+startIdx) << " = " << att->getFloatField((i+startIdx), colIdx) << std::endl;
                }
            }
            else
            {
                for(int n = 0; n < 10000; ++n)
                {
                    double *pnBuffer = new double[numRead];
                    att->getFloatFields(startIdx, numRead, colIdx, pnBuffer);
                    
                    for(size_t i = 0; i < numRead; ++i)
                    {
                        //std::cout << (i+startIdx) << " = " << pnBuffer[i] << std::endl;
                    }
                    
                    delete[] pnBuffer;
                }
            }
            
        }
        else
        {
            std::cout << "There is no attribute table present\n";
        }
    }

    void KEATests::readStringColumns(std::string filename, std::string columnName, bool inMem)
    {
        H5::H5File *keaFile = kealib::KEAImageIO::openKeaH5RDOnly(filename);
        kealib::KEAImageIO keaIO;
        keaIO.openKEAImageHeader(keaFile);
        std::cout << "Image has " << keaIO.getNumOfImageBands() << " image bands\n";
        std::cout << "Image is a KEA file of version " << keaIO.getKEAImageVersion() << std::endl;
        
        if(keaIO.attributeTablePresent(1))
        {
            std::cout << "File has an attribute table\n";
            kealib::KEAAttributeTable *att = NULL;
            if(inMem)
            {
                att = keaIO.getAttributeTable(kealib::kea_att_mem, 1);
            }
            else
            {
                att = keaIO.getAttributeTable(kealib::kea_att_file, 1);
            }
            
            size_t sizeOfAtt = att->getSize();
            std::cout << "There are " << sizeOfAtt << " rows\n";
            
            std::cout << "There are " << att->getNumBoolFields() << " Bool fields\n";
            std::cout << "There are " << att->getNumIntFields() << " Int fields\n";
            std::cout << "There are " << att->getNumFloatFields() << " Float fields\n";
            std::cout << "There are " << att->getNumStringFields() << " String fields\n";
            
            kealib::KEAATTField field = att->getField(columnName);
            
            std::cout << "Field Name = " << field.name << std::endl;
            std::cout << "Field Idx = " << field.idx << std::endl;
            std::cout << "Field Usage = " << field.usage << std::endl;
            std::cout << "Field Col Num = " << field.colNum << std::endl;
            std::cout << "Field Type = " << this->getAttColTypeStr(field.dataType) << std::endl;
            
            unsigned int colIdx = field.idx;
            if(inMem)
            {
                for(size_t i = 0; i < sizeOfAtt; ++i)
                {
                    std::cout << i << " = " << att->getStringField(i, colIdx) << std::endl;
                }
            }
            else
            {
                for(int n = 0; n < 1000; ++n)
                {
                    std::vector<std::string> *pnBuffer = new std::vector<std::string>();
                    att->getStringFields(0, sizeOfAtt, colIdx, pnBuffer);
                    int i = 0;
                    for(std::vector<std::string>::iterator iterStr = pnBuffer->begin(); iterStr != pnBuffer->end(); ++iterStr)
                    {
                        //std::cout << i++ << " = " << *iterStr << std::endl;
                    }
                    delete pnBuffer;
                }
            }
            
            int64_t startIdx = 1500;
            int64_t numRead = 3000;
            if(inMem)
            {
                for(size_t i = 0; i < numRead; ++i)
                {
                    std::cout << (i+startIdx) << " = " << att->getStringField((i+startIdx), colIdx) << std::endl;
                }
            }
            else
            {
                for(int n = 0; n < 1000; ++n)
                {
                    std::vector<std::string> *pnBuffer = new std::vector<std::string>();
                    att->getStringFields(startIdx, numRead, colIdx, pnBuffer);
                    int i = 0;
                    for(std::vector<std::string>::iterator iterStr = pnBuffer->begin(); iterStr != pnBuffer->end(); ++iterStr)
                    {
                        //std::cout << i++ << " = " << *iterStr << std::endl;
                    }
                    delete pnBuffer;
                }
            }
            
        }
        else
        {
            std::cout << "There is no attribute table present\n";
        }
    }
        
        
    KEATests::~KEATests()
    {
        
    }
    
}


int main(int argc, char **argv)
{
	std::cout << "Hello World\n";
    
    keatest::KEATests runTests;
    
    std::string inFile = "";
    std::string colName = "";
    bool inMem = false;
    if(argc == 4)
    {
        inFile = std::string(argv[1]);
        colName = std::string(argv[2]);
        std::string memoption = std::string(argv[3]);
        if(memoption == "mem")
        {
            inMem = true;
        }
        else if(memoption == "file")
        {
            inMem = false;
        }
        else
        {
            std::cout << "Option needs to be either \'mem\' or \'file\'.\n";
            return -1;
        }
        runTests.readStringColumns(inFile, colName, inMem);
        //runTests.readIntColumns(inFile, colName, inMem);
        //runTests.readFloatColumns(inFile, colName, inMem);
    }
    else
    {
        std::cout << "Need 3 input parameters: filename, column name, [mem | file]\n";
    }
    
    
    
    
}