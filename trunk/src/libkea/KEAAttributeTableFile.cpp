/*
 *  KEAAttributeTableFile.h
 *  LibKEA
 *
 *  Created by Pete Bunting on 20/07/2012.
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

#include "libkea/KEAAttributeTableFile.h"
#include <stdlib.h>

namespace kealib{
    
    KEAAttributeTableFile::KEAAttributeTableFile(H5::H5File *keaImgIn, const std::string &bandPathBaseIn, size_t numRowsIn, size_t chunkSizeIn) : KEAAttributeTable(kea_att_file)
    {
        numRows = numRowsIn;
        chunkSize = chunkSizeIn;
        keaImg = keaImgIn;
        bandPathBase = bandPathBaseIn;
    }
    
    bool KEAAttributeTableFile::getBoolField(size_t fid, const std::string &name) const throw(KEAATTException)
    {
        bool value = false;
        try 
        {
            KEAATTField field = this->getField(name);
            if(field.dataType != kea_att_bool)
            {
                std::string message = std::string("Field \'") + name + std::string("\' is not of type boolean.");
                throw KEAATTException(message);
            }
            
            value = this->getBoolField(fid, field.idx);
        }
        catch (KEAATTException &e)
        {
            throw e;
        }
        return value;
    }
    
    int64_t KEAAttributeTableFile::getIntField(size_t fid, const std::string &name) const throw(KEAATTException)
    {
        int64_t value = 0;
        try 
        {
            KEAATTField field = this->getField(name);
            if(field.dataType != kea_att_int)
            {
                std::string message = std::string("Field \'") + name + std::string("\' is not of type integer.");
                throw KEAATTException(message);
            }
            
            value = this->getIntField(fid, field.idx);
        }
        catch (KEAATTException &e)
        {
            throw e;
        }
        return value;
    }
    
    double KEAAttributeTableFile::getFloatField(size_t fid, const std::string &name) const throw(KEAATTException)
    {
        double value = 0.;
        try 
        {
            KEAATTField field = this->getField(name);
            if(field.dataType != kea_att_float)
            {
                std::string message = std::string("Field \'") + name + std::string("\' is not of type double.");
                throw KEAATTException(message);
            }
            
            value = this->getFloatField(fid, field.idx);
        }
        catch (KEAATTException &e)
        {
            throw e;
        }
        return value;
    }
    
    std::string KEAAttributeTableFile::getStringField(size_t fid, const std::string &name) const throw(KEAATTException)
    {
        std::string value = "";
        try 
        {
            KEAATTField field = this->getField(name);
            if(field.dataType != kea_att_string)
            {
                std::string message = std::string("Field \'") + name + std::string("\' is not of type string.");
                throw KEAATTException(message);
            }
            
            value = this->getStringField(fid, field.idx);
        }
        catch (KEAATTException &e)
        {
            throw e;
        }
        return value;
    }
    
    void KEAAttributeTableFile::setBoolField(size_t fid, const std::string &name, bool value) throw(KEAATTException)
    {
        try 
        {
            KEAATTField field = this->getField(name);
            if(field.dataType != kea_att_bool)
            {
                std::string message = std::string("Field \'") + name + std::string("\' is not of type boolean.");
                throw KEAATTException(message);
            }
            
            this->setBoolField(fid, field.idx, value);
        }
        catch (KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTableFile::setIntField(size_t fid, const std::string &name, int64_t value) throw(KEAATTException)
    {
        try 
        {
            KEAATTField field = this->getField(name);
            if(field.dataType != kea_att_int)
            {
                std::string message = std::string("Field \'") + name + std::string("\' is not of type integer.");
                throw KEAATTException(message);
            }
            
            this->setIntField(fid, field.idx, value);
        }
        catch (KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTableFile::setFloatField(size_t fid, const std::string &name, double value) throw(KEAATTException)
    {
        try 
        {
            KEAATTField field = this->getField(name);
            if(field.dataType != kea_att_float)
            {
                std::string message = std::string("Field \'") + name + std::string("\' is not of type float.");
                throw KEAATTException(message);
            }
            
            this->setFloatField(fid, field.idx, value);
        }
        catch (KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTableFile::setStringField(size_t fid, const std::string &name, const std::string &value) throw(KEAATTException)
    {
        try 
        {
            KEAATTField field = this->getField(name);
            if(field.dataType != kea_att_string)
            {
                std::string message = std::string("Field \'") + name + std::string("\' is not of type string.");
                throw KEAATTException(message);
            }
            
            this->setStringField(fid, field.idx, value);
        }
        catch (KEAATTException &e)
        {
            throw e;
        }
    }
    
    bool KEAAttributeTableFile::getBoolField(size_t fid, size_t colIdx) const throw(KEAATTException)
    {
        if(fid >= numRows)
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= numBoolFields)
        {
            std::string message = std::string("Requested boolean column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        bool value;
        this->getBoolFields(fid, 1, colIdx, &value);
        return value;
    }
    
    int64_t KEAAttributeTableFile::getIntField(size_t fid, size_t colIdx) const throw(KEAATTException)
    {
        if(fid >= numRows)
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= numIntFields)
        {
            std::string message = std::string("Requested integer column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        int64_t value;
        this->getIntFields(fid, 1, colIdx, &value);
        return value;
    }
    
    double KEAAttributeTableFile::getFloatField(size_t fid, size_t colIdx) const throw(KEAATTException)
    {
        if(fid >= numRows)
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= numFloatFields)
        {
            std::string message = std::string("Requested float column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        double value;
        this->getFloatFields(fid, 1, colIdx, &value);
        return value;
    }
    
    std::string KEAAttributeTableFile::getStringField(size_t fid, size_t colIdx) const throw(KEAATTException)
    {
        if(fid >= numRows)
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= numStringFields)
        {
            std::string message = std::string("Requested string column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        char *apszValue[1];
        std::string sValue;
        this->getStringFields(fid, 1, colIdx, apszValue);
        sValue = apszValue[0];
        free(apszValue[0]);
        return sValue;
    }
    
    // RFC40
    void KEAAttributeTableFile::getBoolFields(size_t startfid, size_t len, size_t colIdx, bool *pbBuffer) const throw(KEAATTException)
    {
        if((startfid+len) > numRows)
        {
            std::string message = std::string("Requested feature (") + sizet2Str(startfid+len) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= numBoolFields)
        {
            std::string message = std::string("Requested boolean column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }

        try
        {        
            size_t preRows = (ceil((double)startfid / chunkSize) * chunkSize) - startfid;
            size_t numOfBlocks = floor(((double)(len - preRows)/chunkSize));
            size_t remainRows = len - preRows - (numOfBlocks * chunkSize);
            fprintf( stderr, "start = %ld len = %ld preRows = %ld numOfBlocks = %ld remanRows = %ld\n", startfid, len, preRows, numOfBlocks, remainRows);
            size_t rowOff = 0;
                
            H5::DataSet boolDataset;
            H5::DataSpace boolDataspace;
            H5::DataSpace boolFieldsMemspace;
            int *boolVals = new int[chunkSize];
            hsize_t boolFieldsOffset[2];
            hsize_t boolFieldsCount[2];
            hsize_t boolFieldsDimsRead[2];
            hsize_t boolFieldsOffset_out[2];
            hsize_t boolFieldsCount_out[2];
            boolDataset = keaImg->openDataSet( (bandPathBase + KEA_ATT_BOOL_DATA) );
            boolDataspace = boolDataset.getSpace();
                    
            int boolNDims = boolDataspace.getSimpleExtentNdims();

            if(boolNDims != 2)
            {
                throw KEAIOException("The boolean datasets needs to have 2 dimensions.");
            }
                    
            hsize_t *boolDims = new hsize_t[boolNDims];
            boolDataspace.getSimpleExtentDims(boolDims);
                    
            if(numRows > boolDims[0])
            {
                throw KEAIOException("The number of features in boolean dataset is smaller than expected.");
            }
                    
            if(numBoolFields > boolDims[1])
            {
                throw KEAIOException("The number of boolean fields is smaller than expected.");
            }
            delete[] boolDims;

            if(preRows > 0)
            {
                boolFieldsOffset[0] = startfid;
                boolFieldsOffset[1] = colIdx;

                boolFieldsCount[0] = preRows;
                boolFieldsCount[1] = 1;

                boolDataspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount, boolFieldsOffset );

                boolFieldsDimsRead[0] = preRows;
                boolFieldsDimsRead[1] = 1;
                boolFieldsMemspace = H5::DataSpace( 2, boolFieldsDimsRead );
                        
                boolFieldsOffset_out[0] = 0;
                boolFieldsOffset_out[1] = 0;
                boolFieldsCount_out[0] = preRows;
                boolFieldsCount_out[1] = 1;
                boolFieldsMemspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount_out, boolFieldsOffset_out );
                        
                boolDataset.read(boolVals, H5::PredType::NATIVE_INT, boolFieldsMemspace, boolDataspace);
                for( size_t i = 0; i < preRows; i++ )
                {
                    pbBuffer[i] = (boolVals[i] != 0);
                }
            }
                
                
            if(numOfBlocks > 0)
            {                   
                boolFieldsOffset[0] = 0;
                boolFieldsOffset[1] = colIdx;
                    
                boolFieldsCount[0] = chunkSize;
                boolFieldsCount[1] = 1;
                    
                boolDataspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount, boolFieldsOffset );
                    
                boolFieldsDimsRead[0] = chunkSize;
                boolFieldsDimsRead[1] = 1;
                boolFieldsMemspace = H5::DataSpace( 2, boolFieldsDimsRead );
                    
                boolFieldsOffset_out[0] = 0;
                boolFieldsOffset_out[1] = 0;
                    
                boolFieldsCount_out[0] = chunkSize;
                boolFieldsCount_out[1] = 1;
                    
                boolFieldsMemspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount_out, boolFieldsOffset_out );
                for(size_t n = 0; n < numOfBlocks; ++n)
                {
                    rowOff = (n*chunkSize);
                    boolFieldsOffset[0] = rowOff;
                    boolDataspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount, boolFieldsOffset );
                    boolDataset.read(boolVals, H5::PredType::NATIVE_INT, boolFieldsMemspace, boolDataspace);
                    for( size_t i = 0; i < chunkSize; i++ )
                    {
                        pbBuffer[i+rowOff] = (boolVals[i] != 0);
                    }
                }
            }
                
            if(remainRows > 0)
            {
                rowOff = (numOfBlocks*chunkSize);
                boolFieldsOffset[0] = rowOff;
                boolFieldsOffset[1] = 0;
                boolFieldsCount[0] = remainRows;
                boolFieldsCount[1] = 1;
                boolDataspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount, boolFieldsOffset );
                        
                boolFieldsDimsRead[0] = remainRows;
                boolFieldsDimsRead[1] = 1;
                boolFieldsMemspace = H5::DataSpace( 2, boolFieldsDimsRead );
                        
                boolFieldsOffset_out[0] = 0;
                boolFieldsOffset_out[1] = 0;
                boolFieldsCount_out[0] = remainRows;
                boolFieldsCount_out[1] = 1;
                boolFieldsMemspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount_out, boolFieldsOffset_out );
                        
                boolDataset.read(boolVals, H5::PredType::NATIVE_INT, boolFieldsMemspace, boolDataspace);
                for( size_t i = 0; i < remainRows; i++ )
                {
                    pbBuffer[i+rowOff] = (boolVals[i] != 0);
                }
                    
            }
                
            boolDataset.close();
            boolDataspace.close();
            boolFieldsMemspace.close();
                
            delete[] boolVals;
        }
        catch(H5::Exception &e)
        {
            throw KEAIOException(e.getDetailMsg());
        }
        catch (KEAATTException &e)
        {
            throw e;
        }
        catch (KEAIOException &e)
        {
            throw e;
        }
        catch(std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }

    void KEAAttributeTableFile::getIntFields(size_t startfid, size_t len, size_t colIdx, int64_t *pnBuffer) const throw(KEAATTException)
    {
        if((startfid+len) > numRows)
        {
            std::string message = std::string("Requested feature (") + sizet2Str(startfid+len) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= numBoolFields)
        {
            std::string message = std::string("Requested integer column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }

        try
        {        
            size_t preRows = (ceil((double)startfid / chunkSize) * chunkSize) - startfid;
            size_t numOfBlocks = floor(((double)(len - preRows)/chunkSize));
            size_t remainRows = len - preRows - (numOfBlocks * chunkSize);
            fprintf( stderr, "start = %ld len = %ld preRows = %ld numOfBlocks = %ld remanRows = %ld\n", startfid, len, preRows, numOfBlocks, remainRows);
            size_t rowOff = 0;
                
            H5::DataSet intDataset;
            H5::DataSpace intDataspace;
            H5::DataSpace intFieldsMemspace;
            hsize_t intFieldsOffset[2];
            hsize_t intFieldsCount[2];
            hsize_t intFieldsDimsRead[2];
            hsize_t intFieldsOffset_out[2];
            hsize_t intFieldsCount_out[2];
            intDataset = keaImg->openDataSet( (bandPathBase + KEA_ATT_INT_DATA) );
            intDataspace = intDataset.getSpace();
                    
            int intNDims = intDataspace.getSimpleExtentNdims();

            if(intNDims != 2)
            {
                throw KEAIOException("The integer datasets needs to have 2 dimensions.");
            }
                    
            hsize_t *intDims = new hsize_t[intNDims];
            intDataspace.getSimpleExtentDims(intDims);
                    
            if(numRows > intDims[0])
            {
                throw KEAIOException("The number of features in integer dataset is smaller than expected.");
            }
                    
            if(numIntFields > intDims[1])
            {
                throw KEAIOException("The number of integer fields is smaller than expected.");
            }
            delete[] intDims;

            if(preRows > 0)
            {
                intFieldsOffset[0] = startfid;
                intFieldsOffset[1] = colIdx;

                intFieldsCount[0] = preRows;
                intFieldsCount[1] = 1;

                intDataspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount, intFieldsOffset );

                intFieldsDimsRead[0] = preRows;
                intFieldsDimsRead[1] = 1;
                intFieldsMemspace = H5::DataSpace( 2, intFieldsDimsRead );
                        
                intFieldsOffset_out[0] = 0;
                intFieldsOffset_out[1] = 0;
                intFieldsCount_out[0] = preRows;
                intFieldsCount_out[1] = 1;
                intFieldsMemspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount_out, intFieldsOffset_out );
                        
                intDataset.read(pnBuffer, H5::PredType::NATIVE_INT64, intFieldsMemspace, intDataspace);
            }
                
                
            if(numOfBlocks > 0)
            {                   
                intFieldsOffset[0] = 0;
                intFieldsOffset[1] = colIdx;
                    
                intFieldsCount[0] = chunkSize;
                intFieldsCount[1] = 1;
                    
                intDataspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount, intFieldsOffset );
                    
                intFieldsDimsRead[0] = chunkSize;
                intFieldsDimsRead[1] = 1;
                intFieldsMemspace = H5::DataSpace( 2, intFieldsDimsRead );
                    
                intFieldsOffset_out[1] = 0;
                    
                intFieldsCount_out[0] = chunkSize;
                intFieldsCount_out[1] = 1;
                    
                intFieldsMemspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount_out, intFieldsOffset_out );
                for(size_t n = 0; n < numOfBlocks; ++n)
                {
                    rowOff = (n*chunkSize) + preRows;
                    intFieldsOffset[0] = rowOff;
                    intDataspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount, intFieldsOffset );

                    intFieldsOffset_out[0] = preRows + rowOff;
                    intFieldsMemspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount_out, intFieldsOffset_out );

                    intDataset.read(pnBuffer, H5::PredType::NATIVE_INT64, intFieldsMemspace, intDataspace);
                }
            }
                
            if(remainRows > 0)
            {
                rowOff = (numOfBlocks*chunkSize) + preRows;
                intFieldsOffset[0] = rowOff;
                intFieldsOffset[1] = 0;
                intFieldsCount[0] = remainRows;
                intFieldsCount[1] = 1;
                intDataspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount, intFieldsOffset );
                        
                intFieldsDimsRead[0] = remainRows;
                intFieldsDimsRead[1] = 1;
                intFieldsMemspace = H5::DataSpace( 2, intFieldsDimsRead );
                        
                intFieldsOffset_out[0] = preRows + rowOff;
                intFieldsOffset_out[1] = 0;
                intFieldsCount_out[0] = remainRows;
                intFieldsCount_out[1] = 1;
                intFieldsMemspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount_out, intFieldsOffset_out );
                        
                intDataset.read(pnBuffer, H5::PredType::NATIVE_INT64, intFieldsMemspace, intDataspace);
            }
                
            intDataset.close();
            intDataspace.close();
            intFieldsMemspace.close();
        }
        catch(H5::Exception &e)
        {
            throw KEAIOException(e.getDetailMsg());
        }
        catch (KEAATTException &e)
        {
            throw e;
        }
        catch (KEAIOException &e)
        {
            throw e;
        }
        catch(std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }

    void KEAAttributeTableFile::getFloatFields(size_t startfid, size_t len, size_t colIdx, double *pfBuffer) const throw(KEAATTException)
    {
    }

    void KEAAttributeTableFile::getStringFields(size_t startfid, size_t len, size_t colIdx, char **papszStrList, char* (*pStrDup)(const char *)) const throw(KEAATTException)
    {
    }

    void KEAAttributeTableFile::setBoolField(size_t fid, size_t colIdx, bool value) throw(KEAATTException)
    {
        /*if(fid >= attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(fid)->boolFields->size())
        {
            std::string message = std::string("Requested boolean column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        attRows->at(fid)->boolFields->at(colIdx) = value;*/
    }
    
    void KEAAttributeTableFile::setIntField(size_t fid, size_t colIdx, int64_t value) throw(KEAATTException)
    {
        /*if(fid >= attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(fid)->intFields->size())
        {
            std::string message = std::string("Requested integer column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        attRows->at(fid)->intFields->at(colIdx) = value;*/
    }
    
    void KEAAttributeTableFile::setFloatField(size_t fid, size_t colIdx, double value) throw(KEAATTException)
    {
        /*if(fid >= attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(fid)->floatFields->size())
        {
            std::string message = std::string("Requested float column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        attRows->at(fid)->floatFields->at(colIdx) = value;*/
    }
    
    void KEAAttributeTableFile::setStringField(size_t fid, size_t colIdx, const std::string &value) throw(KEAATTException)
    {
        /*if(fid >= attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(fid)->strFields->size())
        {
            std::string message = std::string("Requested string column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        attRows->at(fid)->strFields->at(colIdx) = value;*/
    }

    // RFC40
    void KEAAttributeTableFile::setBoolFields(size_t startfid, size_t len, size_t colIdx, bool *pbBuffer) throw(KEAATTException)
    {
        /*if((startfid+len) > attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(startfid+len) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(startfid)->boolFields->size())
        {
            std::string message = std::string("Requested boolean column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        for( size_t n = 0; n < len; n++)
        {
            attRows->at(n+startfid)->boolFields->at(colIdx) = pbBuffer[n];
        }*/
    }

    void KEAAttributeTableFile::setIntFields(size_t startfid, size_t len, size_t colIdx, int64_t *pnBuffer) throw(KEAATTException)
    {
        /*if((startfid+len) > attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(startfid+len) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(startfid)->intFields->size())
        {
            std::string message = std::string("Requested integer column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        for( size_t n = 0; n < len; n++)
        {
            attRows->at(n+startfid)->intFields->at(colIdx) = pnBuffer[n];
        }*/
    }
    
    void KEAAttributeTableFile::setFloatFields(size_t startfid, size_t len, size_t colIdx, double *pfBuffer) throw(KEAATTException)
    {
        /*if((startfid+len) > attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(startfid+len) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(startfid)->floatFields->size())
        {
            std::string message = std::string("Requested float column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        for( size_t n = 0; n < len; n++)
        {
            attRows->at(n+startfid)->floatFields->at(colIdx) = pfBuffer[n];
        }*/
    }

    void KEAAttributeTableFile::setStringFields(size_t startfid, size_t len, size_t colIdx, char **papszStrList) throw(KEAATTException)
    {
        /*if((startfid+len) > attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(startfid+len) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(startfid)->strFields->size())
        {
            std::string message = std::string("Requested string column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        for( size_t n = 0; n < len; n++)
        {
            attRows->at(n+startfid)->strFields->at(colIdx) = papszStrList[n];
        }*/
    }
    
    KEAATTFeature* KEAAttributeTableFile::getFeature(size_t fid) const throw(KEAATTException)
    {
        /*if(fid >= attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        return attRows->at(fid);*/
        return new KEAATTFeature();
    }
        
    size_t KEAAttributeTableFile::getSize() const
    {
        return numRows;
    }
    
    void KEAAttributeTableFile::addAttBoolField(KEAATTField field, bool val) throw(KEAATTException)
    {
        /*for(std::vector<KEAATTFeature*>::iterator iterFeat = attRows->begin(); iterFeat != attRows->end(); ++iterFeat)
        {
            (*iterFeat)->boolFields->push_back(val);
        }*/
    }
    
    void KEAAttributeTableFile::addAttIntField(KEAATTField field, int64_t val) throw(KEAATTException)
    {
        /*for(std::vector<KEAATTFeature*>::iterator iterFeat = attRows->begin(); iterFeat != attRows->end(); ++iterFeat)
        {
            (*iterFeat)->intFields->push_back(val);
        }*/
    }
    
    void KEAAttributeTableFile::addAttFloatField(KEAATTField field, float val) throw(KEAATTException)
    {
        /*for(std::vector<KEAATTFeature*>::iterator iterFeat = attRows->begin(); iterFeat != attRows->end(); ++iterFeat)
        {
            (*iterFeat)->floatFields->push_back(val);
        }*/
    }
    
    void KEAAttributeTableFile::addAttStringField(KEAATTField field, const std::string &val) throw(KEAATTException)
    {
        /*for(std::vector<KEAATTFeature*>::iterator iterFeat = attRows->begin(); iterFeat != attRows->end(); ++iterFeat)
        {
            (*iterFeat)->strFields->push_back(val);
        }*/
    }
    
    void KEAAttributeTableFile::addRows(size_t numRows)
    {        
        /*KEAATTFeature *feat = NULL;
        
        for(size_t i = 0; i < numRows; ++i)
        {
            feat = this->createKeaFeature();
            feat->fid = attRows->size();
            attRows->push_back(feat);
        }*/
    }
    
    KEAAttributeTable* KEAAttributeTableFile::createKeaAtt(H5::H5File *keaImg, unsigned int band)throw(KEAATTException, KEAIOException)
    {
        // Create instance of class to populate and return.
        std::string bandPathBase = KEA_DATASETNAME_BAND + uint2Str(band);
        KEAAttributeTableFile *att = NULL;
        
        try
        {
            // Read header size.
            hsize_t *attSize = new hsize_t[5];
            try 
            {   
                hsize_t dimsValue[1];
                dimsValue[0] = 5;
                H5::DataSpace valueDataSpace(1, dimsValue);
                H5::DataSet datasetAttSize = keaImg->openDataSet( bandPathBase + KEA_ATT_SIZE_HEADER );
                datasetAttSize.read(attSize, H5::PredType::NATIVE_HSIZE, valueDataSpace);
                datasetAttSize.close();
                valueDataSpace.close();
            } 
            catch (H5::Exception &e) 
            {
                throw KEAIOException("The attribute table size field is not present.");
            }
            
            if(attSize[0] > 0)
            {
                size_t numRows = attSize[0];
                // READ ATTRIBUTE TABLE FROM KEA IMAGE BAND...
                
                // READ THE CHUNK SIZE
                size_t chunkSize = 0;
                try 
                {   
                    hsize_t dimsValue[1];
                    dimsValue[0] = 1;
                    H5::DataSpace valueDataSpace(1, dimsValue);
                    H5::DataSet datasetAttSize = keaImg->openDataSet( bandPathBase + KEA_ATT_CHUNKSIZE_HEADER );
                    hsize_t hChunkSize = 0;
                    datasetAttSize.read(&hChunkSize, H5::PredType::NATIVE_HSIZE, valueDataSpace);
                    datasetAttSize.close();
                    valueDataSpace.close();
                    if(hChunkSize > 0)
                    {
                        chunkSize = hChunkSize;
                    }
                } 
                catch(H5::Exception &e) 
                {
                    throw KEAIOException("The attribute table size field is not present.");
                }                

                att = new KEAAttributeTableFile(keaImg, bandPathBase, numRows, chunkSize);
                
                // READ TABLE HEADERS
                H5::CompType *fieldCompTypeMem = KEAAttributeTable::createAttibuteIdxCompTypeMem();
                
                bool firstColNum = true;
                
                // READ BOOLEAN HEADERS
                att->numBoolFields = attSize[1];
                if(att->numBoolFields > 0)
                {
                    try
                    {
                        H5::DataSet boolFieldsDataset = keaImg->openDataSet( bandPathBase + KEA_ATT_BOOL_FIELDS_HEADER );
                        
                        H5::DataSpace boolFieldsDataspace = boolFieldsDataset.getSpace();   
                        hsize_t boolFieldOff[1];
                        boolFieldOff[0] = 0;
                        
                        hsize_t boolFieldsDims[1]; 
                        boolFieldsDims[0] = att->numBoolFields;
                        H5::DataSpace boolFieldsMemspace(1, boolFieldsDims);
           
                        boolFieldsDataspace.selectHyperslab( H5S_SELECT_SET, boolFieldsDims, boolFieldOff );
                                                
                        KEAAttributeIdx *inFields = new KEAAttributeIdx[att->numBoolFields];
                                                
                        boolFieldsDataset.read(inFields, *fieldCompTypeMem, boolFieldsMemspace, boolFieldsDataspace);
                        
                        KEAATTField field;
                        for(unsigned int i = 0; i < att->numBoolFields; ++i)
                        {
                            field = KEAATTField();
                            field.name = std::string(inFields[i].name);
                            field.dataType = kea_att_bool;
                            field.idx = inFields[i].idx;
                            field.usage = std::string(inFields[i].usage);
                            field.colNum = inFields[i].colNum;
                            
                            if(firstColNum)
                            {
                                if(field.colNum == 0)
                                {
                                    att->numOfCols = 1;
                                }
                                else
                                {
                                    att->numOfCols = field.colNum + 1;
                                }
                                firstColNum = false;
                            }
                            else if(field.colNum >= att->numOfCols)
                            {
                                att->numOfCols = field.colNum + 1;
                            }

                            att->fields->insert(std::pair<std::string, KEAATTField>(field.name, field));
                        }
                        
                        boolFieldsDataset.close();
                        boolFieldsDataspace.close();
                        boolFieldsMemspace.close();
                        
                        delete[] inFields;
                    }
                    catch( H5::Exception &e )
                    {
                        throw KEAIOException(e.getDetailMsg());
                    }
                }
                
                // READ INTEGER HEADERS
                att->numIntFields = attSize[2];
                if(att->numIntFields > 0)
                {
                    try
                    {
                        H5::DataSet intFieldsDataset = keaImg->openDataSet( bandPathBase + KEA_ATT_INT_FIELDS_HEADER );
                        H5::DataSpace intFieldsDataspace = intFieldsDataset.getSpace();
                                            
                        hsize_t intFieldOff[1];
                        intFieldOff[0] = 0;
                        
                        hsize_t intFieldsDims[1]; 
                        intFieldsDims[0] = att->numIntFields;
                        H5::DataSpace intFieldsMemspace(1, intFieldsDims);
                        
                        intFieldsDataspace.selectHyperslab( H5S_SELECT_SET, intFieldsDims, intFieldOff );
                        
                        KEAAttributeIdx *inFields = new KEAAttributeIdx[att->numIntFields];                        
                        
                        intFieldsDataset.read(inFields, *fieldCompTypeMem, intFieldsMemspace, intFieldsDataspace);
                        
                        KEAATTField field;
                        for(unsigned int i = 0; i < att->numIntFields; ++i)
                        {
                            field = KEAATTField();
                            field.name = std::string(inFields[i].name);
                            field.dataType = kea_att_int;
                            field.idx = inFields[i].idx;
                            field.usage = std::string(inFields[i].usage);
                            field.colNum = inFields[i].colNum;
                            
                            if(firstColNum)
                            {
                                if(field.colNum == 0)
                                {
                                    att->numOfCols = 1;
                                }
                                else
                                {
                                    att->numOfCols = field.colNum + 1;
                                }
                                firstColNum = false;
                            }
                            else if(field.colNum >= att->numOfCols)
                            {
                                att->numOfCols = field.colNum + 1;
                            }

                            
                            att->fields->insert(std::pair<std::string, KEAATTField>(field.name, field));
                        }
                        
                        intFieldsDataset.close();
                        intFieldsDataspace.close();
                        intFieldsMemspace.close();
                        
                        delete[] inFields;
                    }
                    catch( H5::Exception &e )
                    {
                        throw KEAIOException(e.getDetailMsg());
                    }
                }
                
                // READ FLOAT HEADERS
                att->numFloatFields = attSize[3];
                if(att->numFloatFields > 0)
                {
                    try
                    {
                        H5::DataSet floatFieldsDataset = keaImg->openDataSet( bandPathBase + KEA_ATT_FLOAT_FIELDS_HEADER );
                        H5::DataSpace floatFieldsDataspace = floatFieldsDataset.getSpace();
                                                
                        hsize_t floatFieldOff[1];
                        floatFieldOff[0] = 0;
                        
                        hsize_t floatFieldsDims[1]; 
                        floatFieldsDims[0] = att->numFloatFields;
                        H5::DataSpace floatFieldsMemspace(1, floatFieldsDims);
                        
                        floatFieldsDataspace.selectHyperslab( H5S_SELECT_SET, floatFieldsDims, floatFieldOff );
                        
                        KEAAttributeIdx *inFields = new KEAAttributeIdx[att->numFloatFields];
                        
                        floatFieldsDataset.read(inFields, *fieldCompTypeMem, floatFieldsMemspace, floatFieldsDataspace);
                        
                        KEAATTField field;
                        for(unsigned int i = 0; i < att->numFloatFields; ++i)
                        {
                            field = KEAATTField();
                            field.name = std::string(inFields[i].name);
                            field.dataType = kea_att_float;
                            field.idx = inFields[i].idx;
                            field.usage = std::string(inFields[i].usage);
                            field.colNum = inFields[i].colNum;
                            
                            if(firstColNum)
                            {
                                if(field.colNum == 0)
                                {
                                    att->numOfCols = 1;
                                }
                                else
                                {
                                    att->numOfCols = field.colNum + 1;
                                }
                                firstColNum = false;
                            }
                            else if(field.colNum >= att->numOfCols)
                            {
                                att->numOfCols = field.colNum + 1;
                            }

                            
                            att->fields->insert(std::pair<std::string, KEAATTField>(field.name, field));
                        }
                        
                        floatFieldsDataset.close();
                        floatFieldsDataspace.close();
                        floatFieldsMemspace.close();
                        
                        delete[] inFields;
                    }
                    catch( H5::Exception &e )
                    {
                        throw KEAIOException(e.getDetailMsg());
                    }
                }
                
                // READ STRING HEADERS
                att->numStringFields = attSize[4];
                if(att->numStringFields > 0)
                {
                    try
                    {
                        H5::DataSet strFieldsDataset = keaImg->openDataSet( bandPathBase + KEA_ATT_STRING_FIELDS_HEADER );
                        H5::DataSpace strFieldsDataspace = strFieldsDataset.getSpace();
                        
                        hsize_t strFieldOff[1];
                        strFieldOff[0] = 0;
                        
                        hsize_t strFieldsDims[1]; 
                        strFieldsDims[0] = att->numStringFields;
                        H5::DataSpace strFieldsMemspace(1, strFieldsDims);
                        
                        strFieldsDataspace.selectHyperslab( H5S_SELECT_SET, strFieldsDims, strFieldOff );
                        
                        KEAAttributeIdx *inFields = new KEAAttributeIdx[att->numStringFields];
                        
                        strFieldsDataset.read(inFields, *fieldCompTypeMem, strFieldsMemspace, strFieldsDataspace);
                        
                        KEAATTField field;
                        for(unsigned int i = 0; i < att->numStringFields; ++i)
                        {
                            field = KEAATTField();
                            field.name = std::string(inFields[i].name);
                            field.dataType = kea_att_string;
                            field.idx = inFields[i].idx;
                            field.usage = std::string(inFields[i].usage);
                            field.colNum = inFields[i].colNum;
                            
                            if(firstColNum)
                            {
                                if(field.colNum == 0)
                                {
                                    att->numOfCols = 1;
                                }
                                else
                                {
                                    att->numOfCols = field.colNum + 1;
                                }
                                firstColNum = false;
                            }
                            else if(field.colNum >= att->numOfCols)
                            {
                                att->numOfCols = field.colNum + 1;
                            }

                            
                            att->fields->insert(std::pair<std::string, KEAATTField>(field.name, field));
                        }
                        
                        strFieldsDataset.close();
                        strFieldsDataspace.close();
                        strFieldsMemspace.close();
                        
                        delete[] inFields;
                    }
                    catch( H5::Exception &e )
                    {
                        throw KEAIOException(e.getDetailMsg());
                    }
                }
                /*
                // Reserve space in vector.
                att->attRows->reserve(attSize[0]);
                
                size_t numOfBlocks = 0;
                numOfBlocks = floor(((double)attSize[0]/chunkSize));
                size_t remainRows = attSize[0] - (numOfBlocks * chunkSize);
                
                KEAATTFeature *feat = NULL;
                size_t cFid = 0;
                size_t rowOff = 0;
                
                H5::DataSet boolDataset;
                H5::DataSpace boolDataspace;
                H5::DataSpace boolFieldsMemspace;
                int *boolVals = NULL;
                hsize_t boolFieldsOffset[2];
                hsize_t boolFieldsCount[2];
                hsize_t boolFieldsDimsRead[2];
                hsize_t boolFieldsOffset_out[2];
                hsize_t boolFieldsCount_out[2];
                if(att->numBoolFields > 0)
                {
                    boolDataset = keaImg->openDataSet( (bandPathBase + KEA_ATT_BOOL_DATA) );
                    boolDataspace = boolDataset.getSpace();
                    
                    int boolNDims = boolDataspace.getSimpleExtentNdims();

                    if(boolNDims != 2)
                    {
                        throw KEAIOException("The boolean datasets needs to have 2 dimensions.");
                    }
                    
                    hsize_t *boolDims = new hsize_t[boolNDims];
                    boolDataspace.getSimpleExtentDims(boolDims);
                    
                    if(attSize[0] > boolDims[0])
                    {
                        throw KEAIOException("The number of features in boolean dataset is smaller than expected.");
                    }
                    
                    if(att->numBoolFields > boolDims[1])
                    {
                        throw KEAIOException("The number of boolean fields is smaller than expected.");
                    }
                    delete[] boolDims;
                    
                    boolVals = new int[chunkSize*att->numBoolFields];
                    
                    boolFieldsOffset[0] = 0;
                    boolFieldsOffset[1] = 0;
                    
                    boolFieldsCount[0] = chunkSize;
                    boolFieldsCount[1] = att->numBoolFields;
                    
                    boolDataspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount, boolFieldsOffset );
                    
                    boolFieldsDimsRead[0] = chunkSize;
                    boolFieldsDimsRead[1] = att->numBoolFields;
                    boolFieldsMemspace = H5::DataSpace( 2, boolFieldsDimsRead );
                    
                    boolFieldsOffset_out[0] = 0;
                    boolFieldsOffset_out[1] = 0;
                    
                    boolFieldsCount_out[0] = chunkSize;
                    boolFieldsCount_out[1] = att->numBoolFields;
                    
                    boolFieldsMemspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount_out, boolFieldsOffset_out );
                }
                
                H5::DataSet intDataset;
                H5::DataSpace intDataspace;
                H5::DataSpace intFieldsMemspace;
                int64_t *intVals = NULL;
                hsize_t intFieldsOffset[2];
                hsize_t intFieldsCount[2];
                hsize_t intFieldsDimsRead[2];
                hsize_t intFieldsOffset_out[2];
                hsize_t intFieldsCount_out[2];
                if(att->numIntFields > 0)
                {
                    intDataset = keaImg->openDataSet( (bandPathBase + KEA_ATT_INT_DATA) );
                    intDataspace = intDataset.getSpace();
                    
                    int intNDims = intDataspace.getSimpleExtentNdims();
                    
                    if(intNDims != 2)
                    {
                        throw KEAIOException("The integer datasets needs to have 2 dimensions.");
                    }
                    
                    hsize_t *intDims = new hsize_t[intNDims];
                    intDataspace.getSimpleExtentDims(intDims);
                    
                    if(attSize[0] > intDims[0])
                    {
                        throw KEAIOException("The number of features in integer dataset is smaller than expected.");
                    }
                    
                    if(att->numIntFields > intDims[1])
                    {
                        throw KEAIOException("The number of integer fields is smaller than expected.");
                    }
                    delete[] intDims;
                    
                    intVals = new int64_t[chunkSize*att->numIntFields];
                    
                    intFieldsOffset[0] = 0;
                    intFieldsOffset[1] = 0;
                    
                    intFieldsCount[0] = chunkSize;
                    intFieldsCount[1] = att->numIntFields;
                    
                    intDataspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount, intFieldsOffset );
                    
                    intFieldsDimsRead[0] = chunkSize;
                    intFieldsDimsRead[1] = att->numIntFields;
                    intFieldsMemspace = H5::DataSpace( 2, intFieldsDimsRead );
                    
                    intFieldsOffset_out[0] = 0;
                    intFieldsOffset_out[1] = 0;
                    
                    intFieldsCount_out[0] = chunkSize;
                    intFieldsCount_out[1] = att->numIntFields;
                    
                    intFieldsMemspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount_out, intFieldsOffset_out );
                }
                
                H5::DataSet floatDataset;
                H5::DataSpace floatDataspace;
                H5::DataSpace floatFieldsMemspace;
                double *floatVals = NULL;
                hsize_t floatFieldsOffset[2];
                hsize_t floatFieldsCount[2];
                hsize_t floatFieldsDimsRead[2];
                hsize_t floatFieldsOffset_out[2];
                hsize_t floatFieldsCount_out[2];
                if(att->numFloatFields > 0)
                {
                    floatDataset = keaImg->openDataSet( (bandPathBase + KEA_ATT_FLOAT_DATA) );
                    floatDataspace = floatDataset.getSpace();
                    
                    int floatNDims = floatDataspace.getSimpleExtentNdims();
                    
                    if(floatNDims != 2)
                    {
                        throw KEAIOException("The float datasets needs to have 2 dimensions.");
                    }
                    
                    hsize_t *floatDims = new hsize_t[floatNDims];
                    floatDataspace.getSimpleExtentDims(floatDims);
                    
                    if(attSize[0] > floatDims[0])
                    {
                        throw KEAIOException("The number of features in float dataset is smaller than expected.");
                    }
                    
                    if(att->numFloatFields > floatDims[1])
                    {
                        throw KEAIOException("The number of float fields is smaller than expected.");
                    }
                    delete[] floatDims;
                    
                    floatVals = new double[chunkSize*att->numFloatFields];
                    
                    floatFieldsOffset[0] = 0;
                    floatFieldsOffset[1] = 0;
                    
                    floatFieldsCount[0] = chunkSize;
                    floatFieldsCount[1] = att->numFloatFields;
                    
                    floatDataspace.selectHyperslab( H5S_SELECT_SET, floatFieldsCount, floatFieldsOffset );
                    
                    floatFieldsDimsRead[0] = chunkSize;
                    floatFieldsDimsRead[1] = att->numFloatFields;
                    
                    floatFieldsMemspace = H5::DataSpace( 2, floatFieldsDimsRead );
                    
                    floatFieldsOffset_out[0] = 0;
                    floatFieldsOffset_out[1] = 0;
                    
                    floatFieldsCount_out[0] = chunkSize;
                    floatFieldsCount_out[1] = att->numFloatFields;
                    
                    floatFieldsMemspace.selectHyperslab( H5S_SELECT_SET, floatFieldsCount_out, floatFieldsOffset_out );
                }

                
                H5::DataSet strDataset;
                H5::DataSpace strDataspace;
                H5::DataSpace strFieldsMemspace;
                H5::CompType *strTypeMem = KEAAttributeTable::createKeaStringCompTypeMem();
                KEAString *stringVals = NULL;
                hsize_t strFieldsOffset[2];
                hsize_t strFieldsCount[2];
                hsize_t strFieldsDimsRead[2];
                hsize_t strFieldsOffset_out[2];
                hsize_t strFieldsCount_out[2];
                if(att->numStringFields > 0)
                {
                    strDataset = keaImg->openDataSet( (bandPathBase + KEA_ATT_STRING_DATA) );
                    strDataspace = strDataset.getSpace();
                    
                    int strNDims = strDataspace.getSimpleExtentNdims();
                    
                    if(strNDims != 2)
                    {
                        throw KEAIOException("The string datasets needs to have 2 dimensions.");
                    }
                    
                    hsize_t *strDims = new hsize_t[strNDims];
                    strDataspace.getSimpleExtentDims(strDims);
                    
                    if(attSize[0] > strDims[0])
                    {
                        throw KEAIOException("The number of features in string dataset is smaller than expected.");
                    }
                    
                    if(att->numStringFields > strDims[1])
                    {
                        throw KEAIOException("The number of string fields is smaller than expected.");
                    }
                    delete[] strDims;
                    
                    stringVals = new KEAString[chunkSize*att->numStringFields];
                    
                    strFieldsOffset[0] = 0;
                    strFieldsOffset[1] = 0;
                    
                    strFieldsCount[0] = chunkSize;
                    strFieldsCount[1] = att->numStringFields;
                    strDataspace.selectHyperslab( H5S_SELECT_SET, strFieldsCount, strFieldsOffset );
                    
                    strFieldsDimsRead[0] = chunkSize;
                    strFieldsDimsRead[1] = att->numStringFields;
                    strFieldsMemspace = H5::DataSpace( 2, strFieldsDimsRead );
                    
                    strFieldsOffset_out[0] = 0;
                    strFieldsOffset_out[1] = 0;
                    
                    strFieldsCount_out[0] = chunkSize;
                    strFieldsCount_out[1] = att->numStringFields;
                    
                    strFieldsMemspace.selectHyperslab( H5S_SELECT_SET, strFieldsCount_out, strFieldsOffset_out );
                }
                
                H5::DataSet neighboursDataset = keaImg->openDataSet( (bandPathBase + KEA_ATT_NEIGHBOURS_DATA) );
                H5::DataSpace neighboursDataspace = neighboursDataset.getSpace();
                
                int neighboursNDims = neighboursDataspace.getSimpleExtentNdims();
                if(neighboursNDims != 1)
                {
                    throw KEAIOException("The neighbours datasets needs to have 1 dimension.");
                }
                
                // Neighbours 
                hsize_t *neighboursDims = new hsize_t[neighboursNDims];
                neighboursDataspace.getSimpleExtentDims(neighboursDims);
                if(attSize[0] > neighboursDims[0])
                {
                    throw KEAIOException("The number of features in neighbours dataset smaller than expected.");
                }
                delete[] neighboursDims;
                
                VarLenFieldHDF *neighbourVals = new VarLenFieldHDF[chunkSize];
                H5::DataType intVarLenMemDT = H5::VarLenType(&H5::PredType::NATIVE_HSIZE);
                hsize_t neighboursOffset[1];
                neighboursOffset[0] = 0;
                hsize_t neighboursCount[1];
                neighboursCount[0] = chunkSize;
                neighboursDataspace.selectHyperslab( H5S_SELECT_SET, neighboursCount, neighboursOffset );
                
                hsize_t neighboursDimsRead[1]; 
                neighboursDimsRead[0] = chunkSize;
                H5::DataSpace neighboursMemspace( 1, neighboursDimsRead );
                
                hsize_t neighboursOffset_out[1];
                neighboursOffset_out[0] = 0;
                hsize_t neighboursCount_out[1];
                neighboursCount_out[0] = chunkSize;
                neighboursMemspace.selectHyperslab( H5S_SELECT_SET, neighboursCount_out, neighboursOffset_out );
                                
                if(numOfBlocks > 0)
                {                   
                    for(size_t n = 0; n < numOfBlocks; ++n)
                    {
                        rowOff = (n*chunkSize);
                        // Read data.
                        neighboursOffset[0] = rowOff;
                        neighboursDataspace.selectHyperslab( H5S_SELECT_SET, neighboursCount, neighboursOffset );
                        neighboursDataset.read(neighbourVals, intVarLenMemDT, neighboursMemspace, neighboursDataspace);
                        
                        if(att->numBoolFields > 0)
                        {
                            boolFieldsOffset[0] = rowOff;
                            boolDataspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount, boolFieldsOffset );
                            boolDataset.read(boolVals, H5::PredType::NATIVE_INT, boolFieldsMemspace, boolDataspace);
                        }
                        
                        if(att->numIntFields > 0)
                        {
                            intFieldsOffset[0] = rowOff;
                            intDataspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount, intFieldsOffset );
                            intDataset.read(intVals, H5::PredType::NATIVE_INT64, intFieldsMemspace, intDataspace);
                        }
                        
                        if(att->numFloatFields > 0)
                        {
                            floatFieldsOffset[0] = rowOff;
                            floatDataspace.selectHyperslab( H5S_SELECT_SET, floatFieldsCount, floatFieldsOffset );
                            floatDataset.read(floatVals, H5::PredType::NATIVE_DOUBLE, floatFieldsMemspace, floatDataspace);
                        }
                        
                        if(att->numStringFields > 0)
                        {
                            strFieldsOffset[0] = rowOff;
                            strDataspace.selectHyperslab( H5S_SELECT_SET, strFieldsCount, strFieldsOffset );
                            strDataset.read(stringVals, *strTypeMem, strFieldsMemspace, strDataspace);
                        }
                        
                        // Write data into KEAATTFeatures
                        for(size_t i = 0; i < chunkSize; ++i)
                        {
                            feat = att->createKeaFeature();
                            feat->fid = cFid++;
                            
                            if(att->numBoolFields > 0)
                            {
                                feat->boolFields->reserve(att->numBoolFields);
                                for(hsize_t j = 0; j < att->numBoolFields; ++j)
                                {
                                    feat->boolFields->at(j) = boolVals[(i*att->numBoolFields)+j];
                                }
                            }
                            
                            if(att->numIntFields > 0)
                            {
                                feat->intFields->reserve(att->numIntFields);
                                for(hsize_t j = 0; j < att->numIntFields; ++j)
                                {
                                    feat->intFields->at(j) = intVals[(i*att->numIntFields)+j];
                                }
                            }
                            
                            if(att->numFloatFields > 0)
                            {
                                feat->floatFields->reserve(att->numFloatFields);
                                for(hsize_t j = 0; j < att->numFloatFields; ++j)
                                {
                                    feat->floatFields->at(j) = floatVals[(i*att->numFloatFields)+j];
                                }
                            }
                            
                            if(att->numStringFields > 0)
                            {
                                feat->strFields->reserve(att->numStringFields);
                                for(hsize_t j = 0; j < att->numStringFields; ++j)
                                {
                                    feat->strFields->at(j) = std::string(stringVals[(i*att->numStringFields)+j].str);
                                }
                            }
                            
                            if(neighbourVals[i].length > 0)
                            {
                                feat->neighbours->reserve(neighbourVals[i].length);
                                for(hsize_t n = 0; n < neighbourVals[i].length; ++n)
                                {
                                    feat->neighbours->push_back(((size_t*)neighbourVals[i].p)[n]);
                                }
                                delete[] ((size_t*)neighbourVals[i].p);
                            }
                                                        
                            att->attRows->push_back(feat);
                        }                        
                    }
                }
                
                if(remainRows > 0)
                {
                    rowOff = (numOfBlocks*chunkSize);
                    // Read data.
                    neighboursOffset[0] = rowOff;
                    neighboursCount[0] = remainRows;
                    neighboursDataspace.selectHyperslab( H5S_SELECT_SET, neighboursCount, neighboursOffset );
                    
                    neighboursDimsRead[0] = remainRows;
                    neighboursMemspace = H5::DataSpace( 1, neighboursDimsRead );
                    neighboursDataset.read(neighbourVals, intVarLenMemDT, neighboursMemspace, neighboursDataspace);
                    
                    if(att->numBoolFields > 0)
                    {
                        boolFieldsOffset[0] = rowOff;
                        boolFieldsOffset[1] = 0;
                        boolFieldsCount[0] = remainRows;
                        boolFieldsCount[1] = att->numBoolFields;
                        boolDataspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount, boolFieldsOffset );
                        
                        boolFieldsDimsRead[0] = remainRows;
                        boolFieldsDimsRead[1] = att->numBoolFields;
                        boolFieldsMemspace = H5::DataSpace( 2, boolFieldsDimsRead );
                        
                        boolFieldsOffset_out[0] = 0;
                        boolFieldsOffset_out[1] = 0;
                        boolFieldsCount_out[0] = remainRows;
                        boolFieldsCount_out[1] = att->numBoolFields;
                        boolFieldsMemspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount_out, boolFieldsOffset_out );
                        
                        boolDataset.read(boolVals, H5::PredType::NATIVE_INT, boolFieldsMemspace, boolDataspace);
                    }
                    
                    if(att->numIntFields > 0)
                    {
                        intFieldsOffset[0] = rowOff;
                        intFieldsOffset[1] = 0;
                        intFieldsCount[0] = remainRows;
                        intFieldsCount[1] = att->numIntFields;
                        intDataspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount, intFieldsOffset );
                        
                        intFieldsDimsRead[0] = remainRows;
                        intFieldsDimsRead[1] = att->numIntFields;
                        intFieldsMemspace = H5::DataSpace( 2, intFieldsDimsRead );
                        
                        intFieldsOffset_out[0] = 0;
                        intFieldsOffset_out[1] = 0;
                        intFieldsCount_out[0] = remainRows;
                        intFieldsCount_out[1] = att->numIntFields;
                        intFieldsMemspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount_out, intFieldsOffset_out );

                        intDataset.read(intVals, H5::PredType::NATIVE_INT64, intFieldsMemspace, intDataspace);
                    }
                    
                    if(att->numFloatFields > 0)
                    {
                        floatFieldsOffset[0] = rowOff;
                        floatFieldsOffset[1] = 0;
                        floatFieldsCount[0] = remainRows;
                        floatFieldsCount[1] = att->numFloatFields;
                        floatDataspace.selectHyperslab( H5S_SELECT_SET, floatFieldsCount, floatFieldsOffset );
                        
                        floatFieldsDimsRead[0] = remainRows;
                        floatFieldsDimsRead[1] = att->numFloatFields;
                        floatFieldsMemspace = H5::DataSpace( 2, floatFieldsDimsRead );
                        
                        floatFieldsOffset_out[0] = 0;
                        floatFieldsOffset_out[1] = 0;
                        floatFieldsCount_out[0] = remainRows;
                        floatFieldsCount_out[1] = att->numFloatFields;
                        floatFieldsMemspace.selectHyperslab( H5S_SELECT_SET, floatFieldsCount_out, floatFieldsOffset_out );
                        
                        floatDataset.read(floatVals, H5::PredType::NATIVE_DOUBLE, floatFieldsMemspace, floatDataspace);
                    }
                    
                    if(att->numStringFields > 0)
                    {
                        strFieldsOffset[0] = rowOff;
                        strFieldsOffset[1] = 0;
                        strFieldsCount[0] = remainRows;
                        strFieldsCount[1] = att->numStringFields;
                        strDataspace.selectHyperslab( H5S_SELECT_SET, strFieldsCount, strFieldsOffset );
                        
                        strFieldsDimsRead[0] = remainRows;
                        strFieldsDimsRead[1] = att->numStringFields;
                        strFieldsMemspace = H5::DataSpace( 2, strFieldsDimsRead );
                        
                        strFieldsOffset_out[0] = 0;
                        strFieldsOffset_out[1] = 0;
                        strFieldsCount_out[0] = remainRows;
                        strFieldsCount_out[1] = att->numStringFields;
                        strFieldsMemspace.selectHyperslab( H5S_SELECT_SET, strFieldsCount_out, strFieldsOffset_out );
                        
                        strDataset.read(stringVals, *strTypeMem, strFieldsMemspace, strDataspace);
                    }
                    
                    // Write data into KEAATTFeatures
                    for(size_t i = 0; i < remainRows; ++i)
                    {
                        feat = att->createKeaFeature();
                        feat->fid = cFid++;
                        
                        if(att->numBoolFields > 0)
                        {
                            feat->boolFields->reserve(att->numBoolFields);
                            for(hsize_t j = 0; j < att->numBoolFields; ++j)
                            {
                                feat->boolFields->at(j) = boolVals[(i*att->numBoolFields)+j];
                            }
                        }
                        
                        if(att->numIntFields > 0)
                        {
                            feat->intFields->reserve(att->numIntFields);
                            for(hsize_t j = 0; j < att->numIntFields; ++j)
                            {
                                feat->intFields->at(j) = intVals[(i*att->numIntFields)+j];
                            }
                        }
                        
                        if(att->numFloatFields > 0)
                        {
                            feat->floatFields->reserve(att->numFloatFields);
                            for(hsize_t j = 0; j < att->numFloatFields; ++j)
                            {
                                feat->floatFields->at(j) = floatVals[(i*att->numFloatFields)+j];
                            }
                        }
                        
                        if(att->numStringFields > 0)
                        {
                            feat->strFields->reserve(att->numStringFields);
                            for(hsize_t j = 0; j < att->numStringFields; ++j)
                            {
                                feat->strFields->at(j) = std::string(stringVals[(i*att->numStringFields)+j].str);
                            }
                        }
                        
                        if(neighbourVals[i].length > 0)
                        {
                            feat->neighbours->reserve(neighbourVals[i].length);
                            for(hsize_t n = 0; n < neighbourVals[i].length; ++n)
                            {
                                feat->neighbours->push_back(((size_t*)neighbourVals[i].p)[n]);
                            }
                            delete[] ((size_t*)neighbourVals[i].p);
                        }
                        
                        att->attRows->push_back(feat);
                    }    
                }
                
                boolDataset.close();
                boolDataspace.close();
                boolFieldsMemspace.close();
                
                intDataset.close();
                intDataspace.close();
                intFieldsMemspace.close();
                
                floatDataset.close();
                floatDataspace.close();
                floatFieldsMemspace.close();
                
                strDataset.close();
                strDataspace.close();
                strFieldsMemspace.close();
                
                neighboursDataset.close();
                neighboursDataspace.close();
                neighboursMemspace.close();
                
                delete[] neighbourVals;
                
                delete strTypeMem;
                */
            }
            
            delete[] attSize;
        }
        catch(H5::Exception &e)
        {
            throw KEAIOException(e.getDetailMsg());
        }
        catch (KEAATTException &e)
        {
            throw e;
        }
        catch (KEAIOException &e)
        {
            throw e;
        }
        catch(std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return att;
    }

    void KEAAttributeTableFile::exportToKeaFile(H5::H5File *keaImg, unsigned int band, unsigned int chunkSize, unsigned int deflate)throw(KEAATTException, KEAIOException)
    {
        throw KEAIOException("KEAAttributeTableFile does not support exporting to file");
    }
    
    KEAAttributeTableFile::~KEAAttributeTableFile()
    {
    }
    
}
