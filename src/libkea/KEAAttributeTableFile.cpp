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
#include <string.h>
#include <algorithm>

namespace kealib{

    static void* kealibmalloc(size_t nSize, void* ignored)
    {
        return malloc(nSize);
    }

    static void kealibfree(void* ptr, void* ignored)
    {
        free(ptr);
    }

    KEAAttributeTableFile::KEAAttributeTableFile(H5::H5File *keaImgIn, const std::shared_ptr<kealib::kea_mutex>& mutex, const std::string &bandPathBaseIn, size_t numRowsIn, size_t chunkSizeIn, unsigned int deflateIn) : KEAAttributeTable(kea_att_file, mutex)
    {
        numRows = numRowsIn;
        chunkSize = chunkSizeIn;
        deflate = deflateIn;
        keaImg = keaImgIn;
        bandPathBase = bandPathBaseIn;
    }
    
    bool KEAAttributeTableFile::getBoolField(size_t fid, const std::string &name) const
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
        catch (const KEAATTException &e)
        {
            throw e;
        }
        return value;
    }
    
    int64_t KEAAttributeTableFile::getIntField(size_t fid, const std::string &name) const
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
        catch (const KEAATTException &e)
        {
            throw e;
        }
        return value;
    }
    
    double KEAAttributeTableFile::getFloatField(size_t fid, const std::string &name) const
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
        catch (const KEAATTException &e)
        {
            throw e;
        }
        return value;
    }
    
    std::string KEAAttributeTableFile::getStringField(size_t fid, const std::string &name) const
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
        catch (const KEAATTException &e)
        {
            throw e;
        }
        return value;
    }
    
    void KEAAttributeTableFile::setBoolField(size_t fid, const std::string &name, bool value)
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
        catch (const KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTableFile::setIntField(size_t fid, const std::string &name, int64_t value)
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
        catch (const KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTableFile::setFloatField(size_t fid, const std::string &name, double value)
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
        catch (const KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTableFile::setStringField(size_t fid, const std::string &name, const std::string &value)
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
        catch (const KEAATTException &e)
        {
            throw e;
        }
    }
    
    bool KEAAttributeTableFile::getBoolField(size_t fid, size_t colIdx) const
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
        try
        {
            this->getBoolFields(fid, 1, colIdx, &value);
        }
        catch(const KEAATTException &e)
        {
            throw e;
        }
        return value;
    }
    
    int64_t KEAAttributeTableFile::getIntField(size_t fid, size_t colIdx) const
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
        try
        {
            this->getIntFields(fid, 1, colIdx, &value);
        }
        catch(const KEAATTException &e)
        {
            throw e;
        }
        return value;
    }
    
    double KEAAttributeTableFile::getFloatField(size_t fid, size_t colIdx) const
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
        try
        {
            this->getFloatFields(fid, 1, colIdx, &value);
        }
        catch(const KEAATTException &e)
        {
            throw e;
        }
        return value;
    }
    
    std::string KEAAttributeTableFile::getStringField(size_t fid, size_t colIdx) const
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
        std::vector<std::string> *pSBuffer = new std::vector<std::string>();
        std::string sValue;
        try
        {
            this->getStringFields(fid, 1, colIdx, pSBuffer);
        }
        catch(const KEAATTException &e)
        {
            throw e;
        }
        sValue = pSBuffer->front();
        delete pSBuffer;
        return sValue;
    }
    
    // RFC40
    void KEAAttributeTableFile::getBoolFields(size_t startfid, size_t len, size_t colIdx, bool *pbBuffer) const
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
            H5::DataSet boolDataset;
            H5::DataSpace boolDataspace;
            H5::DataSpace boolFieldsMemspace;
            int *boolVals = new int[len];
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
            
            boolFieldsOffset[0] = startfid;
            boolFieldsOffset[1] = colIdx;
            
            boolFieldsCount[0] = len;
            boolFieldsCount[1] = 1;
            
            boolDataspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount, boolFieldsOffset );
            
            boolFieldsDimsRead[0] = len;
            boolFieldsDimsRead[1] = 1;
            boolFieldsMemspace = H5::DataSpace( 2, boolFieldsDimsRead );
            
            boolFieldsOffset_out[0] = 0;
            boolFieldsOffset_out[1] = 0;
            boolFieldsCount_out[0] = len;
            boolFieldsCount_out[1] = 1;
            boolFieldsMemspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount_out, boolFieldsOffset_out );
            
            boolDataset.read(boolVals, H5::PredType::NATIVE_INT, boolFieldsMemspace, boolDataspace);
            for( size_t i = 0; i < len; i++ )
            {
                pbBuffer[i] = (boolVals[i] != 0);
            }
            
            boolDataset.close();
            boolDataspace.close();
            boolFieldsMemspace.close();
            
            delete[] boolVals;
        }
        catch(const H5::Exception &e)
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
        catch (const KEAIOException &e)
        {
            throw KEAATTException(e.what());
        }
        catch(const std::exception &e)
        {
            throw KEAATTException(e.what());
        }
    }
    
    void KEAAttributeTableFile::getIntFields(size_t startfid, size_t len, size_t colIdx, int64_t *pnBuffer) const
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if((startfid+len) > numRows)
        {
            std::string message = std::string("Requested feature (") + sizet2Str(startfid+len) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= numIntFields)
        {
            std::string message = std::string("Requested integer column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        try
        {
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
            
            intFieldsOffset[0] = startfid;
            intFieldsOffset[1] = colIdx;
            
            intFieldsCount[0] = len;
            intFieldsCount[1] = 1;
            
            intDataspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount, intFieldsOffset );
            
            intFieldsDimsRead[0] = len;
            intFieldsDimsRead[1] = 1;
            intFieldsMemspace = H5::DataSpace( 2, intFieldsDimsRead );
            
            intFieldsOffset_out[0] = 0;
            intFieldsOffset_out[1] = 0;
            intFieldsCount_out[0] = len;
            intFieldsCount_out[1] = 1;
            intFieldsMemspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount_out, intFieldsOffset_out );
            
            intDataset.read(pnBuffer, H5::PredType::NATIVE_INT64, intFieldsMemspace, intDataspace);
            
            intDataset.close();
            intDataspace.close();
            intFieldsMemspace.close();
        }
        catch(const H5::Exception &e)
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
        catch (const KEAIOException &e)
        {
            throw KEAATTException(e.what());
        }
        catch(const std::exception &e)
        {
            throw KEAATTException(e.what());
        }
    }
    
    void KEAAttributeTableFile::getFloatFields(size_t startfid, size_t len, size_t colIdx, double *pfBuffer) const
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if((startfid+len) > numRows)
        {
            std::string message = std::string("Requested feature (") + sizet2Str(startfid+len) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= numFloatFields)
        {
            std::string message = std::string("Requested float column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        try
        {
            H5::DataSet floatDataset;
            H5::DataSpace floatDataspace;
            H5::DataSpace floatFieldsMemspace;
            hsize_t floatFieldsOffset[2];
            hsize_t floatFieldsCount[2];
            hsize_t floatFieldsDimsRead[2];
            hsize_t floatFieldsOffset_out[2];
            hsize_t floatFieldsCount_out[2];
            floatDataset = keaImg->openDataSet( (bandPathBase + KEA_ATT_FLOAT_DATA) );
            floatDataspace = floatDataset.getSpace();
            
            int floatNDims = floatDataspace.getSimpleExtentNdims();
            
            if(floatNDims != 2)
            {
                throw KEAIOException("The float datasets needs to have 2 dimensions.");
            }
            
            hsize_t *floatDims = new hsize_t[floatNDims];
            floatDataspace.getSimpleExtentDims(floatDims);
            
            if(numRows > floatDims[0])
            {
                throw KEAIOException("The number of features in float dataset is smaller than expected.");
            }
            
            if(numFloatFields > floatDims[1])
            {
                throw KEAIOException("The number of float fields is smaller than expected.");
            }
            delete[] floatDims;
            
            floatFieldsOffset[0] = startfid;
            floatFieldsOffset[1] = colIdx;
            
            floatFieldsCount[0] = len;
            floatFieldsCount[1] = 1;
            
            floatDataspace.selectHyperslab( H5S_SELECT_SET, floatFieldsCount, floatFieldsOffset );
            
            floatFieldsDimsRead[0] = len;
            floatFieldsDimsRead[1] = 1;
            floatFieldsMemspace = H5::DataSpace( 2, floatFieldsDimsRead );
            
            floatFieldsOffset_out[0] = 0;
            floatFieldsOffset_out[1] = 0;
            floatFieldsCount_out[0] = len;
            floatFieldsCount_out[1] = 1;
            floatFieldsMemspace.selectHyperslab( H5S_SELECT_SET, floatFieldsCount_out, floatFieldsOffset_out );
            
            floatDataset.read(pfBuffer, H5::PredType::NATIVE_DOUBLE, floatFieldsMemspace, floatDataspace);
            
            floatDataset.close();
            floatDataspace.close();
            floatFieldsMemspace.close();
        }
        catch(const H5::Exception &e)
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
        catch (const KEAIOException &e)
        {
            throw KEAATTException(e.what());
        }
        catch(const std::exception &e)
        {
            throw KEAATTException(e.what());
        }
    }
    
    void KEAAttributeTableFile::getStringFields(size_t startfid, size_t len, size_t colIdx, std::vector<std::string> *psBuffer) const
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if((startfid+len) > numRows)
        {
            std::string message = std::string("Requested feature (") + sizet2Str(startfid+len) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= numStringFields)
        {
            std::string message = std::string("Requested string column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        try
        {
            H5::DataSet strDataset;
            H5::DataSpace strDataspace;
            H5::DataSpace strFieldsMemspace;
            H5::CompType *strTypeMem = KEAAttributeTable::createKeaStringCompTypeMem();
            hsize_t strFieldsOffset[2];
            hsize_t strFieldsCount[2];
            hsize_t strFieldsDimsRead[2];
            hsize_t strFieldsOffset_out[2];
            hsize_t strFieldsCount_out[2];
            strDataset = keaImg->openDataSet( (bandPathBase + KEA_ATT_STRING_DATA) );
            strDataspace = strDataset.getSpace();
            KEAString *stringVals = new KEAString[len];
            
            int strNDims = strDataspace.getSimpleExtentNdims();
            
            if(strNDims != 2)
            {
                throw KEAIOException("The str datasets needs to have 2 dimensions.");
            }
            
            hsize_t *strDims = new hsize_t[strNDims];
            strDataspace.getSimpleExtentDims(strDims);
            
            if(numRows > strDims[0])
            {
                throw KEAIOException("The number of features in str dataset is smaller than expected.");
            }
            
            if(numStringFields > strDims[1])
            {
                throw KEAIOException("The number of str fields is smaller than expected.");
            }
            delete[] strDims;
            
            strFieldsOffset[0] = startfid;
            strFieldsOffset[1] = colIdx;
            
            strFieldsCount[0] = len;
            strFieldsCount[1] = 1;
            
            strDataspace.selectHyperslab( H5S_SELECT_SET, strFieldsCount, strFieldsOffset );
            
            strFieldsDimsRead[0] = len;
            strFieldsDimsRead[1] = 1;
            strFieldsMemspace = H5::DataSpace( 2, strFieldsDimsRead );
            
            strFieldsOffset_out[0] = 0;
            strFieldsOffset_out[1] = 0;
            strFieldsCount_out[0] = len;
            strFieldsCount_out[1] = 1;
            strFieldsMemspace.selectHyperslab( H5S_SELECT_SET, strFieldsCount_out, strFieldsOffset_out );
            
            strDataset.read(stringVals, *strTypeMem, strFieldsMemspace, strDataspace);
            psBuffer->clear();
            psBuffer->reserve(len);
            for( size_t i = 0; i < len; i++ )
            {
                psBuffer->push_back(std::string(stringVals[i].str));
                free(stringVals[i].str);
            }

            strDataset.close();
            strDataspace.close();
            strFieldsMemspace.close();
            delete strTypeMem;
            delete[] stringVals;
        }
        catch(const H5::Exception &e)
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
        catch (const KEAIOException &e)
        {
            throw KEAATTException(e.what());
        }
        catch(const std::exception &e)
        {
            throw KEAATTException(e.what());
        }
    }
    
    void KEAAttributeTableFile::getNeighbours(size_t startfid, size_t len, std::vector<std::vector<size_t>* > *neighbours) const
    {
        try
        {
            kealib::kea_lock lock(*this->m_mutex); 
            KEAStackPrintState printState;
            if(!neighbours->empty())
            {
                for(auto iterNeigh = neighbours->begin(); iterNeigh != neighbours->end(); ++iterNeigh)
                {
                    delete *iterNeigh;
                }
                neighbours->clear();
            }
            neighbours->reserve(len);
            
            H5::DataSet neighboursDataset = keaImg->openDataSet( (bandPathBase + KEA_ATT_NEIGHBOURS_DATA) );
            H5::DataSpace neighboursDataspace = neighboursDataset.getSpace();
            
            int neighboursNDims = neighboursDataspace.getSimpleExtentNdims();
            if(neighboursNDims != 1)
            {
                throw KEAIOException("The neighbours datasets needs to have 1 dimension.");
            }
            
            hsize_t *neighboursDims = new hsize_t[neighboursNDims];
            neighboursDataspace.getSimpleExtentDims(neighboursDims);
            if(this->getSize() > neighboursDims[0])
            {
                throw KEAIOException("The number of features in neighbours dataset smaller than expected.");
            }
            delete[] neighboursDims;
            
            VarLenFieldHDF *neighbourVals = new VarLenFieldHDF[len];
            H5::DataType intVarLenMemDT = H5::VarLenType(&H5::PredType::NATIVE_HSIZE);
            hsize_t neighboursOffset[1];
            neighboursOffset[0] = 0;
            hsize_t neighboursCount[1];
            neighboursCount[0] = len;
            neighboursDataspace.selectHyperslab( H5S_SELECT_SET, neighboursCount, neighboursOffset );
            
            hsize_t neighboursDimsRead[1];
            neighboursDimsRead[0] = len;
            H5::DataSpace neighboursMemspace( 1, neighboursDimsRead );
            
            hsize_t neighboursOffset_out[1];
            neighboursOffset_out[0] = 0;
            hsize_t neighboursCount_out[1];
            neighboursCount_out[0] = len;
            neighboursMemspace.selectHyperslab( H5S_SELECT_SET, neighboursCount_out, neighboursOffset_out );
            
            neighboursOffset[0] = startfid;
            neighboursDataspace.selectHyperslab( H5S_SELECT_SET, neighboursCount, neighboursOffset );
            neighboursDataset.read(neighbourVals, intVarLenMemDT, neighboursMemspace, neighboursDataspace);
            
            for(size_t i = 0; i < len; ++i)
            {
                neighbours->push_back(new std::vector<size_t>());
                if(neighbourVals[i].length > 0)
                {
                    neighbours->back()->reserve(neighbourVals[i].length);
                    for(hsize_t n = 0; n < neighbourVals[i].length; ++n)
                    {
                        neighbours->back()->push_back(((size_t*)neighbourVals[i].p)[n]);
                    }
                }
            }
        }
        catch(const H5::Exception &e)
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
        catch (const KEAIOException &e)
        {
            throw KEAATTException(e.what());
        }
        catch(const std::exception &e)
        {
            throw KEAATTException(e.what());
        }
    }
    
    void KEAAttributeTableFile::setBoolField(size_t fid, size_t colIdx, bool value)
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
        this->setBoolFields(fid, 1, colIdx, &value);
    }
    
    void KEAAttributeTableFile::setIntField(size_t fid, size_t colIdx, int64_t value)
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
        
        this->setIntFields(fid, 1, colIdx, &value);
    }
    
    void KEAAttributeTableFile::setFloatField(size_t fid, size_t colIdx, double value)
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
        
        this->setFloatFields(fid, 1, colIdx, &value);
    }
    
    void KEAAttributeTableFile::setStringField(size_t fid, size_t colIdx, const std::string &value)
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
        std::vector<std::string> *data = new std::vector<std::string>();
        data->push_back(value);
        this->setStringFields(fid, 1, colIdx, data);
        delete data;
    }
    
    // RFC40
    void KEAAttributeTableFile::setBoolFields(size_t startfid, size_t len, size_t colIdx, bool *pbBuffer)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
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
            H5::DataSet boolDataset;
            H5::DataSpace boolDataspace;
            H5::DataSpace boolFieldsMemspace;
            int *boolVals = new int[len];
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
            
            boolFieldsOffset[0] = startfid;
            boolFieldsOffset[1] = colIdx;
            
            boolFieldsCount[0] = len;
            boolFieldsCount[1] = 1;
            
            boolDataspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount, boolFieldsOffset );
            
            boolFieldsDimsRead[0] = len;
            boolFieldsDimsRead[1] = 1;
            boolFieldsMemspace = H5::DataSpace( 2, boolFieldsDimsRead );
            
            boolFieldsOffset_out[0] = 0;
            boolFieldsOffset_out[1] = 0;
            boolFieldsCount_out[0] = len;
            boolFieldsCount_out[1] = 1;
            boolFieldsMemspace.selectHyperslab( H5S_SELECT_SET, boolFieldsCount_out, boolFieldsOffset_out );
            
            for( size_t i = 0; i < len; i++ )
            {
                boolVals[i] = pbBuffer[i]? 1:0;
            }
            
            boolDataset.write(boolVals, H5::PredType::NATIVE_INT, boolFieldsMemspace, boolDataspace);
            
            boolDataset.close();
            boolDataspace.close();
            boolFieldsMemspace.close();
            
            delete[] boolVals;
            keaImg->flush(H5F_SCOPE_GLOBAL);
        }
        catch(const H5::Exception &e)
        {
            throw KEAIOException(e.getDetailMsg());
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
        catch (const KEAIOException &e)
        {
            throw e;
        }
        catch(const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    void KEAAttributeTableFile::setIntFields(size_t startfid, size_t len, size_t colIdx, int64_t *pnBuffer)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if((startfid+len) > numRows)
        {
            std::string message = std::string("Requested feature (") + sizet2Str(startfid+len) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= numIntFields)
        {
            std::string message = std::string("Requested integer column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        try
        {
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
            
            intFieldsOffset[0] = startfid;
            intFieldsOffset[1] = colIdx;
            
            intFieldsCount[0] = len;
            intFieldsCount[1] = 1;
            
            intDataspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount, intFieldsOffset );
            
            intFieldsDimsRead[0] = len;
            intFieldsDimsRead[1] = 1;
            intFieldsMemspace = H5::DataSpace( 2, intFieldsDimsRead );
            
            intFieldsOffset_out[0] = 0;
            intFieldsOffset_out[1] = 0;
            intFieldsCount_out[0] = len;
            intFieldsCount_out[1] = 1;
            intFieldsMemspace.selectHyperslab( H5S_SELECT_SET, intFieldsCount_out, intFieldsOffset_out );
            
            intDataset.write(pnBuffer, H5::PredType::NATIVE_INT64, intFieldsMemspace, intDataspace);
            
            intDataset.close();
            intDataspace.close();
            intFieldsMemspace.close();
            keaImg->flush(H5F_SCOPE_GLOBAL);
        }
        catch(const H5::Exception &e)
        {
            throw KEAIOException(e.getDetailMsg());
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
        catch (const KEAIOException &e)
        {
            throw e;
        }
        catch(const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    void KEAAttributeTableFile::setFloatFields(size_t startfid, size_t len, size_t colIdx, double *pfBuffer)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if((startfid+len) > numRows)
        {
            std::string message = std::string("Requested feature (") + sizet2Str(startfid+len) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= numFloatFields)
        {
            std::string message = std::string("Requested float column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        try
        {
            H5::DataSet floatDataset;
            H5::DataSpace floatDataspace;
            H5::DataSpace floatFieldsMemspace;
            hsize_t floatFieldsOffset[2];
            hsize_t floatFieldsCount[2];
            hsize_t floatFieldsDimsRead[2];
            hsize_t floatFieldsOffset_out[2];
            hsize_t floatFieldsCount_out[2];
            floatDataset = keaImg->openDataSet( (bandPathBase + KEA_ATT_FLOAT_DATA) );
            floatDataspace = floatDataset.getSpace();
            
            int floatNDims = floatDataspace.getSimpleExtentNdims();
            
            if(floatNDims != 2)
            {
                throw KEAIOException("The float datasets needs to have 2 dimensions.");
            }
            
            hsize_t *floatDims = new hsize_t[floatNDims];
            floatDataspace.getSimpleExtentDims(floatDims);
            
            if(numRows > floatDims[0])
            {
                throw KEAIOException("The number of features in float dataset is smaller than expected.");
            }
            
            if(numFloatFields > floatDims[1])
            {
                throw KEAIOException("The number of float fields is smaller than expected.");
            }
            delete[] floatDims;
            
            floatFieldsOffset[0] = startfid;
            floatFieldsOffset[1] = colIdx;
            
            floatFieldsCount[0] = len;
            floatFieldsCount[1] = 1;
            
            floatDataspace.selectHyperslab( H5S_SELECT_SET, floatFieldsCount, floatFieldsOffset );
            
            floatFieldsDimsRead[0] = len;
            floatFieldsDimsRead[1] = 1;
            floatFieldsMemspace = H5::DataSpace( 2, floatFieldsDimsRead );
            
            floatFieldsOffset_out[0] = 0;
            floatFieldsOffset_out[1] = 0;
            floatFieldsCount_out[0] = len;
            floatFieldsCount_out[1] = 1;
            floatFieldsMemspace.selectHyperslab( H5S_SELECT_SET, floatFieldsCount_out, floatFieldsOffset_out );
            
            floatDataset.write(pfBuffer, H5::PredType::NATIVE_DOUBLE, floatFieldsMemspace, floatDataspace);
            
            floatDataset.close();
            floatDataspace.close();
            floatFieldsMemspace.close();
            keaImg->flush(H5F_SCOPE_GLOBAL);
        }
        catch(const H5::Exception &e)
        {
            throw KEAIOException(e.getDetailMsg());
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
        catch (const KEAIOException &e)
        {
            throw e;
        }
        catch(const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    void KEAAttributeTableFile::setStringFields(size_t startfid, size_t len, size_t colIdx, std::vector<std::string> *papszStrList)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        if((startfid+len) > numRows)
        {
            std::string message = std::string("Requested feature (") + sizet2Str(startfid+len) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= numStringFields)
        {
            std::string message = std::string("Requested string column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        try
        {
            if(papszStrList->size() != len)
            {
                throw KEAATTException("The number of items in the vector<std::string> passed was not equal to the length specified.");
            }
            
            H5::DataSet strDataset;
            H5::DataSpace strDataspace;
            H5::DataSpace strFieldsMemspace;
            H5::CompType *strTypeMem = KEAAttributeTable::createKeaStringCompTypeMem();
            hsize_t strFieldsOffset[2];
            hsize_t strFieldsCount[2];
            hsize_t strFieldsDimsRead[2];
            hsize_t strFieldsOffset_out[2];
            hsize_t strFieldsCount_out[2];
            strDataset = keaImg->openDataSet( (bandPathBase + KEA_ATT_STRING_DATA) );
            strDataspace = strDataset.getSpace();
            KEAString *stringVals = new KEAString[len];
            
            int strNDims = strDataspace.getSimpleExtentNdims();
            
            if(strNDims != 2)
            {
                throw KEAIOException("The str datasets needs to have 2 dimensions.");
            }
            
            hsize_t *strDims = new hsize_t[strNDims];
            strDataspace.getSimpleExtentDims(strDims);
            
            if(numRows > strDims[0])
            {
                throw KEAIOException("The number of features in str dataset is smaller than expected.");
            }
            
            if(numStringFields > strDims[1])
            {
                throw KEAIOException("The number of str fields is smaller than expected.");
            }
            delete[] strDims;
            
            strFieldsOffset[0] = startfid;
            strFieldsOffset[1] = colIdx;
            
            strFieldsCount[0] = len;
            strFieldsCount[1] = 1;
            
            strDataspace.selectHyperslab( H5S_SELECT_SET, strFieldsCount, strFieldsOffset );
            
            strFieldsDimsRead[0] = len;
            strFieldsDimsRead[1] = 1;
            strFieldsMemspace = H5::DataSpace( 2, strFieldsDimsRead );
            
            strFieldsOffset_out[0] = 0;
            strFieldsOffset_out[1] = 0;
            strFieldsCount_out[0] = len;
            strFieldsCount_out[1] = 1;
            strFieldsMemspace.selectHyperslab( H5S_SELECT_SET, strFieldsCount_out, strFieldsOffset_out );
            
            for( size_t i = 0; i < len; i++ )
            {
                // set up the hdf structures
                stringVals[i].str = const_cast<char*>(papszStrList->at(i).c_str());
            }
            
            strDataset.write(stringVals, *strTypeMem, strFieldsMemspace, strDataspace);
            
            strDataset.close();
            strDataspace.close();
            strFieldsMemspace.close();
            delete strTypeMem;
            delete[] stringVals;
            keaImg->flush(H5F_SCOPE_GLOBAL);
        }
        catch(const H5::Exception &e)
        {
            throw KEAIOException(e.getDetailMsg());
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
        catch (const KEAIOException &e)
        {
            throw e;
        }
        catch(const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
    }
    
    void KEAAttributeTableFile::setNeighbours(size_t startfid, size_t len, std::vector<std::vector<size_t>* > *neighbours)
    {
        //throw KEAATTException("KEAAttributeTableFile::setNeighbours(size_t startfid, size_t len, std::vector<size_t> neighbours) is not implemented.");
        
        try
        {
            kealib::kea_lock lock(*this->m_mutex); 
            KEAStackPrintState printState;
            H5::DataSet *neighboursDataset = nullptr;
            try
            {
                neighboursDataset = new H5::DataSet(keaImg->openDataSet(bandPathBase + KEA_ATT_NEIGHBOURS_DATA));
                H5::DataSpace dimsDataSpace = neighboursDataset->getSpace();
                
                hsize_t dataDims[1];
                dimsDataSpace.getSimpleExtentDims(dataDims);
                hsize_t extendDatasetTo[1];
                
                if(this->getSize() > dataDims[0])
                {
                    extendDatasetTo[0] = this->getSize();
                    neighboursDataset->extend(extendDatasetTo);
                }
                
                dimsDataSpace.close();
            }
            catch(const H5::Exception &e)
            {
                // Create Neighbours dataset
                hsize_t initDimsNeighboursDS[1];
                initDimsNeighboursDS[0] = this->getSize();
                hsize_t maxDimsNeighboursDS[1];
                maxDimsNeighboursDS[0] = H5S_UNLIMITED;
                H5::DataSpace neighboursDataspace = H5::DataSpace(1, initDimsNeighboursDS, maxDimsNeighboursDS);
                
                hsize_t dimsNeighboursChunk[1];
                dimsNeighboursChunk[0] = chunkSize;
                
                H5::DataType intVarLenDiskDT = H5::VarLenType(&H5::PredType::STD_U64LE);
                H5::DataType intVarLenMemDT = H5::VarLenType(&H5::PredType::NATIVE_HSIZE);
                VarLenFieldHDF neighboursDataFillVal[1];
                neighboursDataFillVal[0].p = nullptr;
                neighboursDataFillVal[0].length = 0;
                H5::DSetCreatPropList creationNeighboursDSPList;
                creationNeighboursDSPList.setChunk(1, dimsNeighboursChunk);
                creationNeighboursDSPList.setShuffle();
                creationNeighboursDSPList.setDeflate(deflate);
                creationNeighboursDSPList.setFillValue( intVarLenMemDT, &neighboursDataFillVal);
                
                neighboursDataset = new H5::DataSet(keaImg->createDataSet((bandPathBase + KEA_ATT_NEIGHBOURS_DATA), intVarLenDiskDT, neighboursDataspace, creationNeighboursDSPList));
                neighboursDataspace.close();
            }
            
            VarLenFieldHDF *neighbourVals = new VarLenFieldHDF[len];
            
            hsize_t neighboursDataOffset[1];
            neighboursDataOffset[0] = startfid;
            hsize_t neighboursDataDims[1];
            neighboursDataDims[0] = len;
            H5::DataType intVarLenMemDT = H5::VarLenType(&H5::PredType::NATIVE_HSIZE);
            H5::DataSpace memNeighboursDataspace = H5::DataSpace(1, neighboursDataDims);
            
            
            unsigned int i = 0;
            for(auto iterClumps = neighbours->begin(); iterClumps != neighbours->end(); ++iterClumps)
            {
                neighbourVals[i].length = 0;
                neighbourVals[i].p = nullptr;
                if((*iterClumps)->size() > 0)
                {
                    neighbourVals[i].length = (*iterClumps)->size();
                    neighbourVals[i].p = new hsize_t[(*iterClumps)->size()];
                    for(unsigned int k = 0; k < (*iterClumps)->size(); ++k)
                    {
                        ((hsize_t*)neighbourVals[i].p)[k] = (*iterClumps)->at(k);
                    }
                }
                
                ++i;
            }
            
            H5::DataSpace neighboursWriteDataSpace = neighboursDataset->getSpace();
            neighboursWriteDataSpace.selectHyperslab(H5S_SELECT_SET, neighboursDataDims, neighboursDataOffset);
            neighboursDataset->write(neighbourVals, intVarLenMemDT, memNeighboursDataspace, neighboursWriteDataSpace);
            neighboursWriteDataSpace.close();
            
            for(size_t i = 0; i < len; ++i)
            {
                if(neighbourVals[i].length > 0)
                {
                    neighbourVals[i].length = 0;
                    delete[] ((hsize_t*)neighbourVals[i].p);
                }
            }
            
            keaImg->flush(H5F_SCOPE_GLOBAL);
        }
        catch(const H5::Exception &e)
        {
            throw KEAIOException(e.getDetailMsg());
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
        catch (const KEAIOException &e)
        {
            throw e;
        }
        catch(const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        
    }
    
    KEAATTFeature* KEAAttributeTableFile::getFeature(size_t fid) const
    {
        throw KEAATTException("KEAAttributeTableFile::getFeature(size_t fid) has not been implemented.");
        return nullptr;
    }
    
    size_t KEAAttributeTableFile::getSize() const
    {
        return numRows;
    }
    
    void KEAAttributeTableFile::updateSizeHeader(hsize_t nbools, hsize_t nints, hsize_t nfloats, hsize_t nstrings)
    {
        try
        {
            kealib::kea_lock lock(*this->m_mutex); 
            KEAStackPrintState printState;
            // WRITE THE ATT SIZE USED TO THE FILE.
            hsize_t sizeDataOffset[1];
            sizeDataOffset[0] = 0;
            hsize_t sizeDataDims[1];
            sizeDataDims[0] = 5;
            
            H5::DataSet sizeDataset = keaImg->openDataSet(bandPathBase + KEA_ATT_SIZE_HEADER);
            H5::DataSpace sizeWriteDataSpace = sizeDataset.getSpace();
            sizeWriteDataSpace.selectHyperslab(H5S_SELECT_SET, sizeDataDims, sizeDataOffset);
            H5::DataSpace newSizeDataspace = H5::DataSpace(1, sizeDataDims);
            
            hsize_t attSize[5];
            attSize[0] = this->numRows;
            attSize[1] = nbools;
            attSize[2] = nints;
            attSize[3] = nfloats;
            attSize[4] = nstrings;
            
            sizeDataset.write(attSize, H5::PredType::NATIVE_HSIZE, newSizeDataspace, sizeWriteDataSpace);
            sizeDataset.close();
            sizeWriteDataSpace.close();
            newSizeDataspace.close();
            
            // WRITE THE CHUNK SIZE USED TO THE FILE.
            hsize_t chunkSizeDataOffset[1];
            chunkSizeDataOffset[0] = 0;
            hsize_t chunkSizeDataDims[1];
            chunkSizeDataDims[0] = 1;
            
            H5::DataSet chunkSizeDataset = keaImg->openDataSet(bandPathBase + KEA_ATT_CHUNKSIZE_HEADER);
            H5::DataSpace chunkSizeWriteDataSpace = chunkSizeDataset.getSpace();
            chunkSizeWriteDataSpace.selectHyperslab(H5S_SELECT_SET, chunkSizeDataDims, chunkSizeDataOffset);
            H5::DataSpace newChunkSizeDataspace = H5::DataSpace(1, chunkSizeDataDims);
            chunkSizeDataset.write(&chunkSize, H5::PredType::NATIVE_UINT, newChunkSizeDataspace, chunkSizeWriteDataSpace);
            chunkSizeDataset.close();
            chunkSizeWriteDataSpace.close();
            newChunkSizeDataspace.close();
        }
        catch(const H5::Exception &e)
        {
            throw KEAATTException(e.getDetailMsg());
        }
    }
    
    void KEAAttributeTableFile::addAttBoolField(KEAATTField field, bool val)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        // field already been inserted into this->fields by base class
        updateSizeHeader(numBoolFields+1, numIntFields, numFloatFields, numStringFields);
        
        // update BOOL_FIELDS
        KEAAttributeIdx *boolFields = new KEAAttributeIdx[this->numBoolFields+1];
        unsigned int boolFieldsIdx = 0;
        for(auto iterField = fields->begin(); iterField != fields->end(); ++iterField)
        {
            if((*iterField).second.dataType == kea_att_bool)
            {
                boolFields[boolFieldsIdx].name = const_cast<char*>((*iterField).second.name.c_str());
                boolFields[boolFieldsIdx].idx = (*iterField).second.idx;
                boolFields[boolFieldsIdx].usage = const_cast<char*>((*iterField).second.usage.c_str());
                boolFields[boolFieldsIdx].colNum = (*iterField).second.colNum;
                ++boolFieldsIdx;
            }
        }
        // add the new one which isn't added to fields by the base class yet
        boolFields[boolFieldsIdx].name = const_cast<char*>(field.name.c_str());
        boolFields[boolFieldsIdx].idx = field.idx;
        boolFields[boolFieldsIdx].usage = const_cast<char*>(field.usage.c_str());
        boolFields[boolFieldsIdx].colNum = field.colNum;
        
        H5::CompType *fieldDtMem = this->createAttibuteIdxCompTypeMem();
        
        try
        {
            H5::DataSet boolFieldsDataset = keaImg->openDataSet(bandPathBase + KEA_ATT_BOOL_FIELDS_HEADER);
            H5::DataSpace boolFieldsWriteDataSpace = boolFieldsDataset.getSpace();
            
            hsize_t boolFieldsDataDims[1];
            boolFieldsWriteDataSpace.getSimpleExtentDims(boolFieldsDataDims);
            
            if(this->numBoolFields+1 > boolFieldsDataDims[0])
            {
                hsize_t extendboolFieldsDatasetTo[1];
                extendboolFieldsDatasetTo[0] = this->numBoolFields+1;
                boolFieldsDataset.extend( extendboolFieldsDatasetTo );
            }
            
            hsize_t boolFieldsOffset[1];
            boolFieldsOffset[0] = 0;
            boolFieldsDataDims[0] = this->numBoolFields+1;
            
            boolFieldsWriteDataSpace.close();
            boolFieldsWriteDataSpace = boolFieldsDataset.getSpace();
            boolFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolFieldsDataDims, boolFieldsOffset);
            H5::DataSpace newboolFieldsDataspace = H5::DataSpace(1, boolFieldsDataDims);
            
            boolFieldsDataset.write(boolFields, *fieldDtMem, newboolFieldsDataspace, boolFieldsWriteDataSpace);
            
            boolFieldsWriteDataSpace.close();
            newboolFieldsDataspace.close();
            boolFieldsDataset.close();
            
        }
        catch (const H5::Exception &e)
        {
            hsize_t initDimsboolFieldsDS[1];
            initDimsboolFieldsDS[0] = this->numBoolFields+1;
            hsize_t maxDimsboolFieldsDS[1];
            maxDimsboolFieldsDS[0] = H5S_UNLIMITED;
            H5::DataSpace boolFieldsDataSpace = H5::DataSpace(1, initDimsboolFieldsDS, maxDimsboolFieldsDS);
            
            hsize_t dimsboolFieldsChunk[1];
            dimsboolFieldsChunk[0] = chunkSize;
            
            H5::DSetCreatPropList creationboolFieldsDSPList;
            creationboolFieldsDSPList.setChunk(1, dimsboolFieldsChunk);
            creationboolFieldsDSPList.setShuffle();
            creationboolFieldsDSPList.setDeflate(deflate);
            H5::DataSet boolFieldsDataset = keaImg->createDataSet((bandPathBase + KEA_ATT_BOOL_FIELDS_HEADER), *fieldDtMem, boolFieldsDataSpace, creationboolFieldsDSPList);
            
            hsize_t boolFieldsOffset[1];
            boolFieldsOffset[0] = 0;
            hsize_t boolFieldsDataDims[1];
            boolFieldsDataDims[0] = this->numBoolFields+1;
            
            H5::DataSpace boolFieldsWriteDataSpace = boolFieldsDataset.getSpace();
            boolFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolFieldsDataDims, boolFieldsOffset);
            H5::DataSpace newboolFieldsDataspace = H5::DataSpace(1, boolFieldsDataDims);
            
            boolFieldsDataset.write(boolFields, *fieldDtMem, newboolFieldsDataspace, boolFieldsWriteDataSpace);
            
            boolFieldsDataSpace.close();
            boolFieldsWriteDataSpace.close();
            newboolFieldsDataspace.close();
            boolFieldsDataset.close();
            
        }
        delete[] boolFields;
        delete fieldDtMem;
        
        // expand or create bool_DATA
        H5::DataSet *boolDataset = nullptr;
        try
        {
            boolDataset = new H5::DataSet(keaImg->openDataSet(bandPathBase + KEA_ATT_BOOL_DATA));
            
            hsize_t extendDatasetTo[2];
            extendDatasetTo[0] = this->numRows;
            extendDatasetTo[1] = this->numBoolFields+1;
            boolDataset->extend(extendDatasetTo);
            // TODO: don't know how to set reset the fill value
            
        }
        catch(const H5::Exception &e)
        {
            hsize_t initDimsbools[2];
            initDimsbools[0] = numRows;
            initDimsbools[1] = this->numBoolFields+1;
            hsize_t maxDimsbool[2];
            maxDimsbool[0] = H5S_UNLIMITED;
            maxDimsbool[1] = H5S_UNLIMITED;
            H5::DataSpace boolDataSpace = H5::DataSpace(2, initDimsbools, maxDimsbool);
            
            hsize_t dimsboolChunk[2];
            dimsboolChunk[0] = chunkSize;
            dimsboolChunk[1] = 1;
            
            H5::DSetCreatPropList creationboolDSPList;
            creationboolDSPList.setChunk(2, dimsboolChunk);
            creationboolDSPList.setShuffle();
            creationboolDSPList.setDeflate(deflate);
            int fill = val? 1:0;
            creationboolDSPList.setFillValue( H5::PredType::NATIVE_INT, &fill);
            
            boolDataset = new H5::DataSet(keaImg->createDataSet((bandPathBase + KEA_ATT_BOOL_DATA), H5::PredType::STD_I8LE, boolDataSpace, creationboolDSPList));
            boolDataSpace.close();
        }
        boolDataset->close();
        delete boolDataset;
        keaImg->flush(H5F_SCOPE_GLOBAL);
    }
    
    void KEAAttributeTableFile::addAttIntField(KEAATTField field, int64_t val)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        // field already been inserted into this->fields by base class
        updateSizeHeader(numBoolFields, numIntFields+1, numFloatFields, numStringFields);
        
        // update INT_FIELDS
        KEAAttributeIdx *intFields = new KEAAttributeIdx[this->numIntFields+1];
        unsigned int intFieldsIdx = 0;
        for(auto iterField = fields->begin(); iterField != fields->end(); ++iterField)
        {
            if((*iterField).second.dataType == kea_att_int)
            {
                intFields[intFieldsIdx].name = const_cast<char*>((*iterField).second.name.c_str());
                intFields[intFieldsIdx].idx = (*iterField).second.idx;
                intFields[intFieldsIdx].usage = const_cast<char*>((*iterField).second.usage.c_str());
                intFields[intFieldsIdx].colNum = (*iterField).second.colNum;
                ++intFieldsIdx;
            }
        }
        // add the new one which isn't added to fields by the base class yet
        intFields[intFieldsIdx].name = const_cast<char*>(field.name.c_str());
        intFields[intFieldsIdx].idx = field.idx;
        intFields[intFieldsIdx].usage = const_cast<char*>(field.usage.c_str());
        intFields[intFieldsIdx].colNum = field.colNum;
        
        H5::CompType *fieldDtMem = this->createAttibuteIdxCompTypeMem();
        
        try
        {
            H5::DataSet intFieldsDataset = keaImg->openDataSet(bandPathBase + KEA_ATT_INT_FIELDS_HEADER);
            H5::DataSpace intFieldsWriteDataSpace = intFieldsDataset.getSpace();
            
            hsize_t intFieldsDataDims[1];
            intFieldsWriteDataSpace.getSimpleExtentDims(intFieldsDataDims);
            
            if(this->numIntFields+1 > intFieldsDataDims[0])
            {
                hsize_t extendIntFieldsDatasetTo[1];
                extendIntFieldsDatasetTo[0] = this->numIntFields+1;
                intFieldsDataset.extend( extendIntFieldsDatasetTo );
            }
            
            hsize_t intFieldsOffset[1];
            intFieldsOffset[0] = 0;
            intFieldsDataDims[0] = this->numIntFields+1;
            
            intFieldsWriteDataSpace.close();
            intFieldsWriteDataSpace = intFieldsDataset.getSpace();
            intFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intFieldsDataDims, intFieldsOffset);
            H5::DataSpace newIntFieldsDataspace = H5::DataSpace(1, intFieldsDataDims);
            
            intFieldsDataset.write(intFields, *fieldDtMem, newIntFieldsDataspace, intFieldsWriteDataSpace);
            
            intFieldsWriteDataSpace.close();
            newIntFieldsDataspace.close();
            intFieldsDataset.close();
            
        }
        catch (const H5::Exception &e)
        {
            hsize_t initDimsIntFieldsDS[1];
            initDimsIntFieldsDS[0] = this->numIntFields+1;
            hsize_t maxDimsIntFieldsDS[1];
            maxDimsIntFieldsDS[0] = H5S_UNLIMITED;
            H5::DataSpace intFieldsDataSpace = H5::DataSpace(1, initDimsIntFieldsDS, maxDimsIntFieldsDS);
            
            hsize_t dimsIntFieldsChunk[1];
            dimsIntFieldsChunk[0] = chunkSize;
            
            H5::DSetCreatPropList creationIntFieldsDSPList;
            creationIntFieldsDSPList.setChunk(1, dimsIntFieldsChunk);
            creationIntFieldsDSPList.setShuffle();
            creationIntFieldsDSPList.setDeflate(deflate);
            H5::DataSet intFieldsDataset = keaImg->createDataSet((bandPathBase + KEA_ATT_INT_FIELDS_HEADER), *fieldDtMem, intFieldsDataSpace, creationIntFieldsDSPList);
            
            hsize_t intFieldsOffset[1];
            intFieldsOffset[0] = 0;
            hsize_t intFieldsDataDims[1];
            intFieldsDataDims[0] = this->numIntFields+1;
            
            H5::DataSpace intFieldsWriteDataSpace = intFieldsDataset.getSpace();
            intFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intFieldsDataDims, intFieldsOffset);
            H5::DataSpace newIntFieldsDataspace = H5::DataSpace(1, intFieldsDataDims);
            
            intFieldsDataset.write(intFields, *fieldDtMem, newIntFieldsDataspace, intFieldsWriteDataSpace);
            
            intFieldsDataSpace.close();
            intFieldsWriteDataSpace.close();
            newIntFieldsDataspace.close();
            intFieldsDataset.close();
            
        }
        delete[] intFields;
        delete fieldDtMem;
        
        // expand or create INT_DATA
        H5::DataSet *intDataset = nullptr;
        try
        {
            intDataset = new H5::DataSet(keaImg->openDataSet(bandPathBase + KEA_ATT_INT_DATA));
            
            hsize_t extendDatasetTo[2];
            extendDatasetTo[0] = this->numRows;
            extendDatasetTo[1] = this->numIntFields+1;
            intDataset->extend(extendDatasetTo);
            // TODO: don't know how to set reset the fill value
            
        }
        catch(const H5::Exception &e)
        {
            hsize_t initDimsInts[2];
            initDimsInts[0] = numRows;
            initDimsInts[1] = this->numIntFields+1;
            hsize_t maxDimsInt[2];
            maxDimsInt[0] = H5S_UNLIMITED;
            maxDimsInt[1] = H5S_UNLIMITED;
            H5::DataSpace intDataSpace = H5::DataSpace(2, initDimsInts, maxDimsInt);
            
            hsize_t dimsIntChunk[2];
            dimsIntChunk[0] = chunkSize;
            dimsIntChunk[1] = 1;
            
            H5::DSetCreatPropList creationIntDSPList;
            creationIntDSPList.setChunk(2, dimsIntChunk);
            creationIntDSPList.setShuffle();
            creationIntDSPList.setDeflate(deflate);
            creationIntDSPList.setFillValue( H5::PredType::NATIVE_INT64, &val);
            
            intDataset = new H5::DataSet(keaImg->createDataSet((bandPathBase + KEA_ATT_INT_DATA), H5::PredType::STD_I64LE, intDataSpace, creationIntDSPList));
            intDataSpace.close();
        }
        intDataset->close();
        delete intDataset;
        keaImg->flush(H5F_SCOPE_GLOBAL);
    }
    
    void KEAAttributeTableFile::addAttFloatField(KEAATTField field, float val)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        // field already been inserted into this->fields by base class
        updateSizeHeader(numBoolFields, numIntFields, numFloatFields+1, numStringFields);
        
        // update FLOAT_FIELDS
        KEAAttributeIdx *floatFields = new KEAAttributeIdx[this->numFloatFields+1];
        unsigned int floatFieldsIdx = 0;
        for(auto iterField = fields->begin(); iterField != fields->end(); ++iterField)
        {
            if((*iterField).second.dataType == kea_att_float)
            {
                floatFields[floatFieldsIdx].name = const_cast<char*>((*iterField).second.name.c_str());
                floatFields[floatFieldsIdx].idx = (*iterField).second.idx;
                floatFields[floatFieldsIdx].usage = const_cast<char*>((*iterField).second.usage.c_str());
                floatFields[floatFieldsIdx].colNum = (*iterField).second.colNum;
                ++floatFieldsIdx;
            }
        }
        // add the new one which isn't added to fields by the base class yet
        floatFields[floatFieldsIdx].name = const_cast<char*>(field.name.c_str());
        floatFields[floatFieldsIdx].idx = field.idx;
        floatFields[floatFieldsIdx].usage = const_cast<char*>(field.usage.c_str());
        floatFields[floatFieldsIdx].colNum = field.colNum;
        
        H5::CompType *fieldDtMem = this->createAttibuteIdxCompTypeMem();
        
        try
        {
            H5::DataSet floatFieldsDataset = keaImg->openDataSet(bandPathBase + KEA_ATT_FLOAT_FIELDS_HEADER);
            H5::DataSpace floatFieldsWriteDataSpace = floatFieldsDataset.getSpace();
            
            hsize_t floatFieldsDataDims[1];
            floatFieldsWriteDataSpace.getSimpleExtentDims(floatFieldsDataDims);
            
            if(this->numFloatFields+1 > floatFieldsDataDims[0])
            {
                hsize_t extendfloatFieldsDatasetTo[1];
                extendfloatFieldsDatasetTo[0] = this->numFloatFields+1;
                floatFieldsDataset.extend( extendfloatFieldsDatasetTo );
            }
            
            hsize_t floatFieldsOffset[1];
            floatFieldsOffset[0] = 0;
            floatFieldsDataDims[0] = this->numFloatFields+1;
            
            floatFieldsWriteDataSpace.close();
            floatFieldsWriteDataSpace = floatFieldsDataset.getSpace();
            floatFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatFieldsDataDims, floatFieldsOffset);
            H5::DataSpace newfloatFieldsDataspace = H5::DataSpace(1, floatFieldsDataDims);
            
            floatFieldsDataset.write(floatFields, *fieldDtMem, newfloatFieldsDataspace, floatFieldsWriteDataSpace);
            
            floatFieldsWriteDataSpace.close();
            newfloatFieldsDataspace.close();
            floatFieldsDataset.close();
            
        }
        catch (const H5::Exception &e)
        {
            hsize_t initDimsfloatFieldsDS[1];
            initDimsfloatFieldsDS[0] = this->numFloatFields+1;
            hsize_t maxDimsfloatFieldsDS[1];
            maxDimsfloatFieldsDS[0] = H5S_UNLIMITED;
            H5::DataSpace floatFieldsDataSpace = H5::DataSpace(1, initDimsfloatFieldsDS, maxDimsfloatFieldsDS);
            
            hsize_t dimsfloatFieldsChunk[1];
            dimsfloatFieldsChunk[0] = chunkSize;
            
            H5::DSetCreatPropList creationfloatFieldsDSPList;
            creationfloatFieldsDSPList.setChunk(1, dimsfloatFieldsChunk);
            creationfloatFieldsDSPList.setShuffle();
            creationfloatFieldsDSPList.setDeflate(deflate);
            H5::DataSet floatFieldsDataset = keaImg->createDataSet((bandPathBase + KEA_ATT_FLOAT_FIELDS_HEADER), *fieldDtMem, floatFieldsDataSpace, creationfloatFieldsDSPList);
            
            hsize_t floatFieldsOffset[1];
            floatFieldsOffset[0] = 0;
            hsize_t floatFieldsDataDims[1];
            floatFieldsDataDims[0] = this->numFloatFields+1;
            
            H5::DataSpace floatFieldsWriteDataSpace = floatFieldsDataset.getSpace();
            floatFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatFieldsDataDims, floatFieldsOffset);
            H5::DataSpace newfloatFieldsDataspace = H5::DataSpace(1, floatFieldsDataDims);
            
            floatFieldsDataset.write(floatFields, *fieldDtMem, newfloatFieldsDataspace, floatFieldsWriteDataSpace);
            
            floatFieldsDataSpace.close();
            floatFieldsWriteDataSpace.close();
            newfloatFieldsDataspace.close();
            floatFieldsDataset.close();
            
        }
        delete[] floatFields;
        delete fieldDtMem;
        
        // expand or create float_DATA
        H5::DataSet *floatDataset = nullptr;
        try
        {
            floatDataset = new H5::DataSet(keaImg->openDataSet(bandPathBase + KEA_ATT_FLOAT_DATA));
            
            hsize_t extendDatasetTo[2];
            extendDatasetTo[0] = this->numRows;
            extendDatasetTo[1] = this->numFloatFields+1;
            floatDataset->extend(extendDatasetTo);
            // TODO: don't know how to set reset the fill value
            
        }
        catch(const H5::Exception &e)
        {
            hsize_t initDimsfloats[2];
            initDimsfloats[0] = numRows;
            initDimsfloats[1] = this->numFloatFields+1;
            hsize_t maxDimsfloat[2];
            maxDimsfloat[0] = H5S_UNLIMITED;
            maxDimsfloat[1] = H5S_UNLIMITED;
            H5::DataSpace floatDataSpace = H5::DataSpace(2, initDimsfloats, maxDimsfloat);
            
            hsize_t dimsfloatChunk[2];
            dimsfloatChunk[0] = chunkSize;
            dimsfloatChunk[1] = 1;
            
            H5::DSetCreatPropList creationfloatDSPList;
            creationfloatDSPList.setChunk(2, dimsfloatChunk);
            creationfloatDSPList.setShuffle();
            creationfloatDSPList.setDeflate(deflate);
            creationfloatDSPList.setFillValue( H5::PredType::NATIVE_FLOAT, &val);
            
            floatDataset = new H5::DataSet(keaImg->createDataSet((bandPathBase + KEA_ATT_FLOAT_DATA), H5::PredType::IEEE_F64LE, floatDataSpace, creationfloatDSPList));
            floatDataSpace.close();
        }
        floatDataset->close();
        delete floatDataset;
        keaImg->flush(H5F_SCOPE_GLOBAL);
    }
    
    void KEAAttributeTableFile::addAttStringField(KEAATTField field, const std::string &val)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        // field already been inserted into this->fields by base class
        updateSizeHeader(numBoolFields, numIntFields, numFloatFields, numStringFields+1);
        
        // update string_FIELDS
        KEAAttributeIdx *stringFields = new KEAAttributeIdx[this->numStringFields+1];
        unsigned int stringFieldsIdx = 0;
        for(auto iterField = fields->begin(); iterField != fields->end(); ++iterField)
        {
            if((*iterField).second.dataType == kea_att_string)
            {
                stringFields[stringFieldsIdx].name = const_cast<char*>((*iterField).second.name.c_str());
                stringFields[stringFieldsIdx].idx = (*iterField).second.idx;
                stringFields[stringFieldsIdx].usage = const_cast<char*>((*iterField).second.usage.c_str());
                stringFields[stringFieldsIdx].colNum = (*iterField).second.colNum;
                ++stringFieldsIdx;
            }
        }
        // add the new one which isn't added to fields by the base class yet
        stringFields[stringFieldsIdx].name = const_cast<char*>(field.name.c_str());
        stringFields[stringFieldsIdx].idx = field.idx;
        stringFields[stringFieldsIdx].usage = const_cast<char*>(field.usage.c_str());
        stringFields[stringFieldsIdx].colNum = field.colNum;
        
        H5::CompType *fieldDtMem = this->createAttibuteIdxCompTypeMem();
        
        try
        {
            H5::DataSet stringFieldsDataset = keaImg->openDataSet(bandPathBase + KEA_ATT_STRING_FIELDS_HEADER);
            H5::DataSpace stringFieldsWriteDataSpace = stringFieldsDataset.getSpace();
            
            hsize_t stringFieldsDataDims[1];
            stringFieldsWriteDataSpace.getSimpleExtentDims(stringFieldsDataDims);
            
            if(this->numStringFields+1 > stringFieldsDataDims[0])
            {
                hsize_t extendstringFieldsDatasetTo[1];
                extendstringFieldsDatasetTo[0] = this->numStringFields+1;
                stringFieldsDataset.extend( extendstringFieldsDatasetTo );
            }
            
            hsize_t stringFieldsOffset[1];
            stringFieldsOffset[0] = 0;
            stringFieldsDataDims[0] = this->numStringFields+1;
            
            stringFieldsWriteDataSpace.close();
            stringFieldsWriteDataSpace = stringFieldsDataset.getSpace();
            stringFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, stringFieldsDataDims, stringFieldsOffset);
            H5::DataSpace newstringFieldsDataspace = H5::DataSpace(1, stringFieldsDataDims);
            
            stringFieldsDataset.write(stringFields, *fieldDtMem, newstringFieldsDataspace, stringFieldsWriteDataSpace);
            
            stringFieldsWriteDataSpace.close();
            newstringFieldsDataspace.close();
            stringFieldsDataset.close();
            
        }
        catch (const H5::Exception &e)
        {
            hsize_t initDimsstringFieldsDS[1];
            initDimsstringFieldsDS[0] = this->numStringFields+1;
            hsize_t maxDimsstringFieldsDS[1];
            maxDimsstringFieldsDS[0] = H5S_UNLIMITED;
            H5::DataSpace stringFieldsDataSpace = H5::DataSpace(1, initDimsstringFieldsDS, maxDimsstringFieldsDS);
            
            hsize_t dimsstringFieldsChunk[1];
            dimsstringFieldsChunk[0] = chunkSize;
            
            H5::DSetCreatPropList creationstringFieldsDSPList;
            creationstringFieldsDSPList.setChunk(1, dimsstringFieldsChunk);
            creationstringFieldsDSPList.setShuffle();
            creationstringFieldsDSPList.setDeflate(deflate);
            H5::DataSet stringFieldsDataset = keaImg->createDataSet((bandPathBase + KEA_ATT_STRING_FIELDS_HEADER), *fieldDtMem, stringFieldsDataSpace, creationstringFieldsDSPList);
            
            hsize_t stringFieldsOffset[1];
            stringFieldsOffset[0] = 0;
            hsize_t stringFieldsDataDims[1];
            stringFieldsDataDims[0] = this->numStringFields+1;
            
            H5::DataSpace stringFieldsWriteDataSpace = stringFieldsDataset.getSpace();
            stringFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, stringFieldsDataDims, stringFieldsOffset);
            H5::DataSpace newstringFieldsDataspace = H5::DataSpace(1, stringFieldsDataDims);
            
            stringFieldsDataset.write(stringFields, *fieldDtMem, newstringFieldsDataspace, stringFieldsWriteDataSpace);
            
            stringFieldsDataSpace.close();
            stringFieldsWriteDataSpace.close();
            newstringFieldsDataspace.close();
            stringFieldsDataset.close();
            
        }
        delete[] stringFields;
        delete fieldDtMem;
        
        H5::CompType *strTypeMem = this->createKeaStringCompTypeMem();
        
        // expand or create string_DATA
        H5::DataSet *stringDataset = nullptr;
        try
        {
            stringDataset = new H5::DataSet(keaImg->openDataSet(bandPathBase + KEA_ATT_STRING_DATA));
            
            hsize_t extendDatasetTo[2];
            extendDatasetTo[0] = this->numRows;
            extendDatasetTo[1] = this->numStringFields+1;
            stringDataset->extend(extendDatasetTo);
            // TODO: don't know how to set reset the fill value
            
        }
        catch(const H5::Exception &e)
        {
            hsize_t initDimsstrings[2];
            initDimsstrings[0] = numRows;
            initDimsstrings[1] = this->numStringFields+1;
            hsize_t maxDimsstring[2];
            maxDimsstring[0] = H5S_UNLIMITED;
            maxDimsstring[1] = H5S_UNLIMITED;
            H5::DataSpace stringDataSpace = H5::DataSpace(2, initDimsstrings, maxDimsstring);
            
            hsize_t dimsstringChunk[2];
            dimsstringChunk[0] = chunkSize;
            dimsstringChunk[1] = 1;
            
            KEAString fillValueStr = KEAString();
            fillValueStr.str = const_cast<char*>(val.c_str());
            H5::DSetCreatPropList creationstringDSPList;
            creationstringDSPList.setChunk(2, dimsstringChunk);
            creationstringDSPList.setShuffle();
            creationstringDSPList.setDeflate(deflate);
            creationstringDSPList.setFillValue( *strTypeMem, &fillValueStr);
            
            stringDataset = new H5::DataSet(keaImg->createDataSet((bandPathBase + KEA_ATT_STRING_DATA), *strTypeMem, stringDataSpace, creationstringDSPList));
            stringDataSpace.close();
        }
        stringDataset->close();
        delete stringDataset;
        delete strTypeMem;
        keaImg->flush(H5F_SCOPE_GLOBAL);
    }
    
    void KEAAttributeTableFile::addRows(size_t numRowsIn)
    {
        if( numRowsIn > 0 )
        {
            kealib::kea_lock lock(*this->m_mutex); 
            KEAStackPrintState printState;
            // update header
            numRows += numRowsIn;
            updateSizeHeader(numBoolFields, numIntFields, numFloatFields, numStringFields);
            
            // extend the various data tables if they exist
            hsize_t extendDatasetTo[2];
            extendDatasetTo[0] = this->numRows;
            try
            {
                H5::DataSet *boolDataset = new H5::DataSet(keaImg->openDataSet(bandPathBase + KEA_ATT_BOOL_DATA));
                extendDatasetTo[1] = this->numBoolFields;
                boolDataset->extend(extendDatasetTo);
                boolDataset->close();
                delete boolDataset;
            }
            catch(const H5::Exception &e)
            {
                // can't exist
            }
            
            try
            {
                H5::DataSet *intDataset = new H5::DataSet(keaImg->openDataSet(bandPathBase + KEA_ATT_INT_DATA));
                extendDatasetTo[1] = this->numIntFields;
                intDataset->extend(extendDatasetTo);
                intDataset->close();
                delete intDataset;
            }
            catch(const H5::Exception &e)
            {
                // can't exist
            }
            
            try
            {
                H5::DataSet *floatDataset = new H5::DataSet(keaImg->openDataSet(bandPathBase + KEA_ATT_FLOAT_DATA));
                extendDatasetTo[1] = this->numFloatFields;
                floatDataset->extend(extendDatasetTo);
                floatDataset->close();
                delete floatDataset;
            }
            catch(const H5::Exception &e)
            {
                // can't exist
            }
            
            try
            {
                H5::DataSet *stringDataset = new H5::DataSet(keaImg->openDataSet(bandPathBase + KEA_ATT_STRING_DATA));
                extendDatasetTo[1] = this->numStringFields;
                stringDataset->extend(extendDatasetTo);
                stringDataset->close();
                delete stringDataset;
            }
            catch(const H5::Exception &e)
            {
                // can't exist
            }
        }
        keaImg->flush(H5F_SCOPE_GLOBAL);
    }
    
    KEAAttributeTable* KEAAttributeTableFile::createKeaAtt(H5::H5File *keaImg, const std::shared_ptr<kealib::kea_mutex>& mutex, unsigned int band, unsigned int chunkSizeIn, unsigned int deflate)
    {
        // Create instance of class to populate and return.
        std::string bandPathBase = KEA_DATASETNAME_BAND + uint2Str(band);
        KEAAttributeTableFile *att = nullptr;
        // no lock needed - should be done by caller
        
        try
        {
            // Read header size.
            hsize_t attSize[5];
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
            catch (const H5::Exception &e)
            {
                throw KEAIOException("The attribute table size field is not present.");
            }
            
            size_t numRows = attSize[0];
            // READ ATTRIBUTE TABLE FROM KEA IMAGE BAND...
            
            // READ THE CHUNK SIZE - default to given
            size_t chunkSize = chunkSizeIn;
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
            catch(const H5::Exception &e)
            {
                throw KEAIOException("The attribute table size field is not present.");
            }
            
            att = new KEAAttributeTableFile(keaImg, mutex, bandPathBase, numRows, chunkSize, deflate);
            
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
                catch( const H5::Exception &e )
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
                    
                    H5::DSetMemXferPropList xfer;
                    /* Ensures that malloc()/free() are from the same C runtime */
                    xfer.setVlenMemManager(kealibmalloc, nullptr, kealibfree, nullptr);
                    intFieldsDataset.read(inFields, *fieldCompTypeMem, intFieldsMemspace, intFieldsDataspace, xfer);
                    
                    KEAATTField field;
                    for(unsigned int i = 0; i < att->numIntFields; ++i)
                    {
                        field = KEAATTField();
                        field.name = std::string(inFields[i].name);
                        free(inFields[i].name);
                        field.dataType = kea_att_int;
                        field.idx = inFields[i].idx;
                        field.usage = std::string(inFields[i].usage);
                        free(inFields[i].usage);
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
                catch( const H5::Exception &e )
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
                    
                    H5::DSetMemXferPropList xfer;
                    /* Ensures that malloc()/free() are from the same C runtime */
                    xfer.setVlenMemManager(kealibmalloc, nullptr, kealibfree, nullptr);
                    floatFieldsDataset.read(inFields, *fieldCompTypeMem, floatFieldsMemspace, floatFieldsDataspace, xfer);
                    
                    KEAATTField field;
                    for(unsigned int i = 0; i < att->numFloatFields; ++i)
                    {
                        field = KEAATTField();
                        field.name = std::string(inFields[i].name);
                        free(inFields[i].name);
                        field.dataType = kea_att_float;
                        field.idx = inFields[i].idx;
                        field.usage = std::string(inFields[i].usage);
                        free(inFields[i].usage);
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
                catch( const H5::Exception &e )
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
                    
                    H5::DSetMemXferPropList xfer;
                    /* Ensures that malloc()/free() are from the same C runtime */
                    xfer.setVlenMemManager(kealibmalloc, nullptr, kealibfree, nullptr);
                    strFieldsDataset.read(inFields, *fieldCompTypeMem, strFieldsMemspace, strFieldsDataspace, xfer);
                    
                    KEAATTField field;
                    for(unsigned int i = 0; i < att->numStringFields; ++i)
                    {
                        field = KEAATTField();
                        field.name = std::string(inFields[i].name);
                        free(inFields[i].name);
                        field.dataType = kea_att_string;
                        field.idx = inFields[i].idx;
                        field.usage = std::string(inFields[i].usage);
                        free(inFields[i].usage);
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
                catch( const H5::Exception &e )
                {
                    throw KEAIOException(e.getDetailMsg());
                }
            }
            
            delete fieldCompTypeMem;
            keaImg->flush(H5F_SCOPE_GLOBAL);
        }
        catch(const H5::Exception &e)
        {
            throw KEAIOException(e.getDetailMsg());
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
        catch (const KEAIOException &e)
        {
            throw e;
        }
        catch(const std::exception &e)
        {
            throw KEAIOException(e.what());
        }
        
        return att;
    }
    
    void KEAAttributeTableFile::exportToKeaFile(H5::H5File *keaImg, unsigned int band, unsigned int chunkSize, unsigned int deflate)
    {
        throw KEAIOException("KEAAttributeTableFile does not support exporting to file");
    }
    
    KEAAttributeTableFile::~KEAAttributeTableFile()
    {
    }
    
}
