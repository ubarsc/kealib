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

HIGHFIVE_REGISTER_TYPE(kealib::KEAAttString, kealib::KEAAttributeTable::createKeaStringCompType)

namespace kealib{

    KEAAttributeTableFile::KEAAttributeTableFile(HighFive::File *keaImgIn, KEAAttributeTable *pBaseAtt, const std::shared_ptr<kealib::kea_mutex>& mutex, unsigned int deflateIn) : KEAAttributeTable(kea_att_file, mutex)
    {
        keaImg = keaImgIn;
        numRows = pBaseAtt->getSize();
        numOfCols = pBaseAtt->getMaxGlobalColIdx();
        numBoolFields = pBaseAtt->getNumBoolFields();
        numIntFields = pBaseAtt->getNumIntFields();
        numFloatFields = pBaseAtt->getNumFloatFields();
        numStringFields = pBaseAtt->getNumStringFields();
        bandPathBase = pBaseAtt->getBandPathBase();
        chunkSize = pBaseAtt->getChunkSize();
        deflate = deflateIn;
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
            auto boolDataset = keaImg->getDataSet(bandPathBase + KEA_ATT_BOOL_DATA);
            int *boolVals = new int[len];

			std::vector<size_t> startOffset = {startfid, colIdx};
			std::vector<size_t> bufSize = {len, 1};
            
            boolDataset.select(startOffset, bufSize).read_raw(boolVals);
            for( size_t i = 0; i < len; i++ )
            {
                pbBuffer[i] = (boolVals[i] != 0);
            }
            delete[] boolVals;
        }
        catch(const HighFive::Exception &e)
        {
            throw KEAATTException(e.what());
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
            auto intDataset = keaImg->getDataSet(bandPathBase + KEA_ATT_INT_DATA);

			std::vector<size_t> startOffset = {startfid, colIdx};
			std::vector<size_t> bufSize = {len, 1};
            
            intDataset.select(startOffset, bufSize).read_raw(pnBuffer);
        }
        catch(const HighFive::Exception &e)
        {
            throw KEAATTException(e.what());
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
            auto floatDataset = keaImg->getDataSet(bandPathBase + KEA_ATT_FLOAT_DATA);

			std::vector<size_t> startOffset = {startfid, colIdx};
			std::vector<size_t> bufSize = {len, 1};
            
            floatDataset.select(startOffset, bufSize).read_raw(pfBuffer);
        }
        catch(const HighFive::Exception &e)
        {
            throw KEAATTException(e.what());
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
            KEAAttString *stringVals = new KEAAttString[len];
            
            auto stringDataset = keaImg->getDataSet(bandPathBase + KEA_ATT_STRING_DATA);

			std::vector<size_t> startOffset = {startfid, colIdx};
			std::vector<size_t> bufSize = {len, 1};
			
            stringDataset.select(startOffset, bufSize).read_raw(stringVals);
            psBuffer->clear();
            psBuffer->reserve(len);
            for( size_t i = 0; i < len; i++ )
            {
                psBuffer->push_back(std::string(stringVals[i].str));
                free(stringVals[i].str);
            }
            delete[] stringVals;
        }
        catch(const HighFive::Exception &e)
        {
            throw KEAATTException(e.what());
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
        /*
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
        */
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
            int8_t *boolVals = new int8_t[len];
            auto boolDataset = keaImg->getDataSet(bandPathBase + KEA_ATT_BOOL_DATA);
            for( size_t i = 0; i < len; i++ )
            {
                boolVals[i] = pbBuffer[i]? 1:0;
            }

			std::vector<size_t> startOffset = {startfid, colIdx};
			std::vector<size_t> bufSize = {len, 1};
            
            boolDataset.select(startOffset, bufSize).write_raw(boolVals);
            delete[] boolVals;
        }
        catch(const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
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
            auto intDataset = keaImg->getDataSet(bandPathBase + KEA_ATT_INT_DATA);
			std::vector<size_t> startOffset = {startfid, colIdx};
			std::vector<size_t> bufSize = {len, 1};
            
            intDataset.select(startOffset, bufSize).write_raw(pnBuffer);
        }
        catch(const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
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
        
            auto floatDataset = keaImg->getDataSet(bandPathBase + KEA_ATT_FLOAT_DATA);
			std::vector<size_t> startOffset = {startfid, colIdx};
			std::vector<size_t> bufSize = {len, 1};
            
            floatDataset.select(startOffset, bufSize).write_raw(pfBuffer);
        }
        catch(const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
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

        if(papszStrList->size() != len)
        {
            throw KEAATTException("The number of items in the vector<std::string> passed was not equal to the length specified.");
        }
        
        try
        {

            KEAAttString *stringVals = new KEAAttString[len];
            for( size_t i = 0; i < len; i++ )
            {
                // set up the hdf structures
                stringVals[i].str = const_cast<char*>(papszStrList->at(i).c_str());
            }
            
            auto stringDataset = keaImg->getDataSet(bandPathBase + KEA_ATT_STRING_DATA);

			std::vector<size_t> startOffset = {startfid, colIdx};
			std::vector<size_t> bufSize = {len, 1};
            
            stringDataset.select(startOffset, bufSize).write_raw(stringVals);
            delete[] stringVals;
        }
        catch(const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
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
        /*
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
            neighboursDataset->close();
            delete neighboursDataset;
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
        */
    }
    
    KEAATTFeature* KEAAttributeTableFile::getFeature(size_t fid) const
    {
        throw KEAATTException("KEAAttributeTableFile::getFeature(size_t fid) has not been implemented.");
        return nullptr;
    }
    
    void KEAAttributeTableFile::updateSizeHeader(hsize_t nbools, hsize_t nints, hsize_t nfloats, hsize_t nstrings)
    {
        try
        {
            kealib::kea_lock lock(*this->m_mutex); 
            KEAStackPrintState printState;
            // WRITE THE ATT SIZE USED TO THE FILE.
            
            std::vector<uint64_t> attSize(5);
            attSize[0] = this->numRows;
            attSize[1] = nbools;
            attSize[2] = nints;
            attSize[3] = nfloats;
            attSize[4] = nstrings;
            auto attSizeDataset = keaImg->getDataSet(bandPathBase + KEA_ATT_SIZE_HEADER);
            attSizeDataset.write(attSize);
            
            auto datasetAttSize = keaImg->getDataSet( bandPathBase + KEA_ATT_CHUNKSIZE_HEADER);
            datasetAttSize.write(chunkSize);
        }
        catch(const HighFive::Exception &e)
        {
            throw KEAATTException(e.what());
        }
    }
    
    void KEAAttributeTableFile::addAttBoolField(KEAATTField field, bool val)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        // field already been inserted into this->fields by base class
        updateSizeHeader(numBoolFields+1, numIntFields, numFloatFields, numStringFields);

        int fill = val? 1:0;

        addAttField(field, bandPathBase + KEA_ATT_BOOL_FIELDS_HEADER, bandPathBase + KEA_ATT_BOOL_DATA, 
             numBoolFields + 1, HighFive::AtomicType<int8_t>(), H5T_NATIVE_INT, &fill);        
    }
    
    void KEAAttributeTableFile::addAttIntField(KEAATTField field, int64_t val)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        // field already been inserted into this->fields by base class
        updateSizeHeader(numBoolFields, numIntFields+1, numFloatFields, numStringFields);

        addAttField(field, bandPathBase + KEA_ATT_INT_FIELDS_HEADER, bandPathBase + KEA_ATT_INT_DATA, 
             numIntFields + 1, HighFive::AtomicType<int64_t>(), H5T_NATIVE_INT64, &val);        
    }
    
    void KEAAttributeTableFile::addAttFloatField(KEAATTField field, float val)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        // field already been inserted into this->fields by base class
        updateSizeHeader(numBoolFields, numIntFields, numFloatFields+1, numStringFields);

        addAttField(field, bandPathBase + KEA_ATT_FLOAT_FIELDS_HEADER, bandPathBase + KEA_ATT_FLOAT_DATA, 
             numFloatFields + 1, HighFive::AtomicType<double>(), H5T_NATIVE_FLOAT, &val);        
    }
    
    void KEAAttributeTableFile::addAttStringField(KEAATTField field, const std::string &val)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        KEAStackPrintState printState;
        // field already been inserted into this->fields by base class
        updateSizeHeader(numBoolFields, numIntFields, numFloatFields, numStringFields + 1);
        
        auto strTypeMem = this->createKeaStringCompType();

        KEAAttString fillValueStr;
        fillValueStr.str = const_cast<char*>(val.c_str());
        
        addAttField(field, bandPathBase + KEA_ATT_STRING_FIELDS_HEADER, bandPathBase + KEA_ATT_STRING_DATA, 
             numStringFields + 1, strTypeMem, strTypeMem.getId(), &fillValueStr);        
    }
    
    void KEAAttributeTableFile::addAttField(KEAATTField field, const std::string &headerPath, const std::string &dataPath, 
            unsigned int nfields, HighFive::DataType data_type, hid_t fill_type, const void *fill_value)
    {
        // update string_FIELDS
        KEAAttributeIdx *newfields = new KEAAttributeIdx[nfields];
        unsigned int fieldsIdx = 0;
        for(auto iterField = fields->begin(); iterField != fields->end(); ++iterField)
        {
            if((*iterField).second.dataType == field.dataType)
            {
                newfields[fieldsIdx].name = const_cast<char*>((*iterField).second.name.c_str());
                newfields[fieldsIdx].idx = (*iterField).second.idx;
                newfields[fieldsIdx].usage = const_cast<char*>((*iterField).second.usage.c_str());
                newfields[fieldsIdx].colNum = (*iterField).second.colNum;
                ++fieldsIdx;
            }
        }
        // add the new one which isn't added to fields by the base class yet
        newfields[fieldsIdx].name = const_cast<char*>(field.name.c_str());
        newfields[fieldsIdx].idx = field.idx;
        newfields[fieldsIdx].usage = const_cast<char*>(field.usage.c_str());
        newfields[fieldsIdx].colNum = field.colNum;

        auto fieldCompTypeMem = KEAAttributeTable::createAttibuteIdxCompType();
        
        try
        {   
            if( !keaImg->exist(headerPath) )
            {
                // create it first
                // allow the number of fields to grow
                std::vector<size_t> dims = {nfields};
                std::vector<size_t> maxdims = {HighFive::DataSpace::UNLIMITED};
                HighFive::DataSpace fieldsDataSpace = HighFive::DataSpace(dims, maxdims);
                
                HighFive::DataSetCreateProps creationFeldsDSPList;
                creationFeldsDSPList.add(HighFive::Chunking(chunkSize));
                creationFeldsDSPList.add(HighFive::Deflate(deflate));
                creationFeldsDSPList.add(HighFive::Shuffle());
                
                auto fieldsDataset = this->keaImg->createDataSet(headerPath, 
                    fieldsDataSpace, fieldCompTypeMem, creationFeldsDSPList);
                fieldsDataset.write_raw(newfields);
            }
            else
            {
                auto fieldsDataset = keaImg->getDataSet(headerPath);
                
                auto fieldsDataDims = fieldsDataset.getDimensions();
                
                if(nfields > fieldsDataDims[0])
                {
                    fieldsDataDims[0] = nfields;
                    fieldsDataset.resize(fieldsDataDims);
                }
                
                fieldsDataset.write_raw(newfields);
            }
        }
        catch (const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
        }
        delete[] newfields;

        try
        {
            if( !keaImg->exist(dataPath))
            {
                std::vector<size_t> dims = {this->numRows, nfields};
                std::vector<size_t> maxdims = {HighFive::DataSpace::UNLIMITED, HighFive::DataSpace::UNLIMITED};
                HighFive::DataSpace dataSpace = HighFive::DataSpace(dims, maxdims);
                
                HighFive::DataSetCreateProps creationDataDSPList;
                creationDataDSPList.add(HighFive::Chunking(chunkSize, 1));
                creationDataDSPList.add(HighFive::Deflate(deflate));
                creationDataDSPList.add(HighFive::Shuffle());
                if( H5Pset_fill_value(creationDataDSPList.getId(), fill_type, fill_value) < 0 )
                {
    				H5Eprint(H5E_DEFAULT, stderr);
    				throw KEAIOException("Error in H5Pset_fill_value");
                }
                
                this->keaImg->createDataSet(dataPath, 
                    dataSpace, data_type, creationDataDSPList);                
            }
            else
            {
        
                auto dataset = keaImg->getDataSet(dataPath);
                
                std::vector<size_t> extendDatasetTo(2);
                extendDatasetTo[0] = this->numRows;
                extendDatasetTo[1] = nfields;
                dataset.resize(extendDatasetTo);
                // Not possible to reset the fill value so only first column's fill_value does anything...
            }            
        }
        catch(const HighFive::Exception &e)
        {
            throw KEAIOException(e.what());
        }

        keaImg->flush();
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
            std::vector<size_t> extendDatasetTo(2);
            extendDatasetTo[0] = this->numRows;
            try
            {
                auto boolDataset = keaImg->getDataSet(bandPathBase + KEA_ATT_BOOL_DATA);
                extendDatasetTo[1] = this->numBoolFields;
                boolDataset.resize(extendDatasetTo);
            }
            catch(const HighFive::Exception &e)
            {
                // can't exist
            }
            
            try
            {
                auto intDataset = keaImg->getDataSet(bandPathBase + KEA_ATT_INT_DATA);
                extendDatasetTo[1] = this->numIntFields;
                intDataset.resize(extendDatasetTo);
            }
            catch(const HighFive::Exception &e)
            {
                // can't exist
            }
            
            try
            {
                auto floatDataset = keaImg->getDataSet(bandPathBase + KEA_ATT_FLOAT_DATA);
                extendDatasetTo[1] = this->numFloatFields;
                floatDataset.resize(extendDatasetTo);
            }
            catch(const HighFive::Exception &e)
            {
                // can't exist
            }
            
            try
            {
                auto stringDataset = keaImg->getDataSet(bandPathBase + KEA_ATT_STRING_DATA);
                extendDatasetTo[1] = this->numStringFields;
                stringDataset.resize(extendDatasetTo);
            }
            catch(const HighFive::Exception &e)
            {
                // can't exist
            }
            
            keaImg->flush();
        }
    }
    
    KEAAttributeTable* KEAAttributeTableFile::createKeaAtt(HighFive::File *keaImg, const std::shared_ptr<kealib::kea_mutex>& mutex, unsigned int band, unsigned int chunkSizeIn, unsigned int deflate)
    {
        // create an instance of the base class with the number of cols etc
        KEAAttributeTable *pBaseAtt = KEAAttributeTable::createKeaAtt(keaImg, mutex, band, chunkSizeIn);
        
        // now create an instance of KEAAttributeTableFile with the copy constructor
        KEAAttributeTable *pAtt = new KEAAttributeTableFile(keaImg, pBaseAtt, mutex, deflate);
        delete pBaseAtt;

        return pAtt;
    }
    
    void KEAAttributeTableFile::exportToKeaFile(HighFive::File *keaImg, unsigned int band, unsigned int chunkSize, unsigned int deflate)
    {
        throw KEAIOException("KEAAttributeTableFile does not support exporting to file");
    }
    
    KEAAttributeTableFile::~KEAAttributeTableFile()
    {

    }
    
}
