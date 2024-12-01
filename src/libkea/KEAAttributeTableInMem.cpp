/*
 *  KEAAttributeTableInMem.h
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

#include "libkea/KEAAttributeTableInMem.h"
#include <string.h>

namespace kealib{
    
    KEAAttributeTableInMem::KEAAttributeTableInMem(const std::shared_ptr<kealib::kea_mutex>& mutex) : KEAAttributeTable(kea_att_mem, mutex)
    {
        attRows = new std::vector<KEAATTFeature*>();
    }
    
    bool KEAAttributeTableInMem::getBoolField(size_t fid, const std::string &name) const
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
    
    int64_t KEAAttributeTableInMem::getIntField(size_t fid, const std::string &name) const
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
    
    double KEAAttributeTableInMem::getFloatField(size_t fid, const std::string &name) const
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
    
    std::string KEAAttributeTableInMem::getStringField(size_t fid, const std::string &name) const
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
    
    void KEAAttributeTableInMem::setBoolField(size_t fid, const std::string &name, bool value)
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
    
    void KEAAttributeTableInMem::setIntField(size_t fid, const std::string &name, int64_t value)
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
    
    void KEAAttributeTableInMem::setFloatField(size_t fid, const std::string &name, double value)
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
    
    void KEAAttributeTableInMem::setStringField(size_t fid, const std::string &name, const std::string &value)
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
    
    bool KEAAttributeTableInMem::getBoolField(size_t fid, size_t colIdx) const
    {
        kealib::kea_lock lock(*this->m_mutex); 
        if(fid >= attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(fid)->boolFields->size())
        {
            std::string message = std::string("Requested boolean column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        return attRows->at(fid)->boolFields->at(colIdx);
    }
    
    int64_t KEAAttributeTableInMem::getIntField(size_t fid, size_t colIdx) const
    {
        kealib::kea_lock lock(*this->m_mutex); 
        if(fid >= attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(fid)->intFields->size())
        {
            std::string message = std::string("Requested integer column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        return attRows->at(fid)->intFields->at(colIdx);
    }
    
    double KEAAttributeTableInMem::getFloatField(size_t fid, size_t colIdx) const
    {
        kealib::kea_lock lock(*this->m_mutex); 
        if(fid >= attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(fid)->floatFields->size())
        {
            std::string message = std::string("Requested float column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        return attRows->at(fid)->floatFields->at(colIdx);
    }
    
    std::string KEAAttributeTableInMem::getStringField(size_t fid, size_t colIdx) const
    {
        kealib::kea_lock lock(*this->m_mutex); 
        if(fid >= attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(fid)->strFields->size())
        {
            std::string message = std::string("Requested string column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        return attRows->at(fid)->strFields->at(colIdx);
    }
    
    // RFC40
    void KEAAttributeTableInMem::getBoolFields(size_t startfid, size_t len, size_t colIdx, bool *pbBuffer) const
    {
        kealib::kea_lock lock(*this->m_mutex); 
        if((startfid+len) > attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(startfid+len) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(startfid)->boolFields->size())
        {
            std::string message = std::string("Requested boolean column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        for(size_t n = 0; n < len; n++)
        {
            pbBuffer[n] = attRows->at(n+startfid)->boolFields->at(colIdx);
        }
    }

    void KEAAttributeTableInMem::getIntFields(size_t startfid, size_t len, size_t colIdx, int64_t *pnBuffer) const
    {
        kealib::kea_lock lock(*this->m_mutex); 
        if((startfid+len) > attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(startfid+len) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(startfid)->intFields->size())
        {
            std::string message = std::string("Requested integer column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        for(size_t n = 0; n < len; n++)
        {
            pnBuffer[n] = attRows->at(n+startfid)->intFields->at(colIdx);
        }
    }

    void KEAAttributeTableInMem::getFloatFields(size_t startfid, size_t len, size_t colIdx, double *pfBuffer) const
    {
        kealib::kea_lock lock(*this->m_mutex); 
        if((startfid+len) > attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(startfid+len) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(startfid)->floatFields->size())
        {
            std::string message = std::string("Requested float column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        for(size_t n = 0; n < len; n++)
        {
            pfBuffer[n] = attRows->at(n+startfid)->floatFields->at(colIdx);
        }
    }

    void KEAAttributeTableInMem::getStringFields(size_t startfid, size_t len, size_t colIdx, std::vector<std::string> *psBuffer) const
    {
        kealib::kea_lock lock(*this->m_mutex); 
        if((startfid+len) > attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(startfid+len) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(startfid)->strFields->size())
        {
            std::string message = std::string("Requested string column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        psBuffer->clear();
        psBuffer->reserve(len);
        for(size_t n = 0; n < len; n++)
        {
            psBuffer->push_back(std::string(attRows->at(n+startfid)->strFields->at(colIdx).c_str()));
        }
    }
    
    void KEAAttributeTableInMem::getNeighbours(size_t startfid, size_t len, std::vector<std::vector<size_t>* > *neighbours) const
    {
        throw KEAATTException("KEAAttributeTableInMem::getNeighbours(size_t startfid, size_t len, std::vector<size_t> neighbours) is not implemented.");
    }

    void KEAAttributeTableInMem::setBoolField(size_t fid, size_t colIdx, bool value)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        if(fid >= attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(fid)->boolFields->size())
        {
            std::string message = std::string("Requested boolean column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        attRows->at(fid)->boolFields->at(colIdx) = value;
    }
    
    void KEAAttributeTableInMem::setIntField(size_t fid, size_t colIdx, int64_t value)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        if(fid >= attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(fid)->intFields->size())
        {
            std::string message = std::string("Requested integer column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        attRows->at(fid)->intFields->at(colIdx) = value;
    }
    
    void KEAAttributeTableInMem::setFloatField(size_t fid, size_t colIdx, double value)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        if(fid >= attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(fid)->floatFields->size())
        {
            std::string message = std::string("Requested float column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        attRows->at(fid)->floatFields->at(colIdx) = value;
    }
    
    void KEAAttributeTableInMem::setStringField(size_t fid, size_t colIdx, const std::string &value)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        if(fid >= attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(fid)->strFields->size())
        {
            std::string message = std::string("Requested string column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        attRows->at(fid)->strFields->at(colIdx) = value;
    }

    // RFC40
    void KEAAttributeTableInMem::setBoolFields(size_t startfid, size_t len, size_t colIdx, bool *pbBuffer)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        if((startfid+len) > attRows->size())
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
        }
    }

    void KEAAttributeTableInMem::setIntFields(size_t startfid, size_t len, size_t colIdx, int64_t *pnBuffer)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        if((startfid+len) > attRows->size())
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
        }
    }
    
    void KEAAttributeTableInMem::setFloatFields(size_t startfid, size_t len, size_t colIdx, double *pfBuffer)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        if((startfid+len) > attRows->size())
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
        }
    }

    void KEAAttributeTableInMem::setStringFields(size_t startfid, size_t len, size_t colIdx, std::vector<std::string> *papszStrList)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        if((startfid+len) > attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(startfid+len) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx >= attRows->at(startfid)->strFields->size())
        {
            std::string message = std::string("Requested string column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(papszStrList->size() != len)
        {
            throw KEAATTException("The number of items in the vector<std::string> passed was not equal to the length specified.");
        }
        
        for( size_t n = 0; n < len; n++)
        {
            attRows->at(n+startfid)->strFields->at(colIdx) = papszStrList->at(colIdx);
        }
    }
    
    void KEAAttributeTableInMem::setNeighbours(size_t startfid, size_t len, std::vector<std::vector<size_t>* > *neighbours)
    {
        throw KEAATTException("KEAAttributeTableInMem::setNeighbours(size_t startfid, size_t len, std::vector<size_t> neighbours) is not implemented.");
    }
    
    KEAATTFeature* KEAAttributeTableInMem::getFeature(size_t fid) const
    {
        kealib::kea_lock lock(*this->m_mutex); 
        if(fid >= attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        return attRows->at(fid);
    }
        
    size_t KEAAttributeTableInMem::getSize() const
    {
        kealib::kea_lock lock(*this->m_mutex); 
        return attRows->size();
    }
    
    void KEAAttributeTableInMem::addAttBoolField(KEAATTField field, bool val)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        for(auto iterFeat = attRows->begin(); iterFeat != attRows->end(); ++iterFeat)
        {
            (*iterFeat)->boolFields->push_back(val);
        }
    }
    
    void KEAAttributeTableInMem::addAttIntField(KEAATTField field, int64_t val)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        for(auto iterFeat = attRows->begin(); iterFeat != attRows->end(); ++iterFeat)
        {
            (*iterFeat)->intFields->push_back(val);
        }
    }
    
    void KEAAttributeTableInMem::addAttFloatField(KEAATTField field, float val)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        for(auto iterFeat = attRows->begin(); iterFeat != attRows->end(); ++iterFeat)
        {
            (*iterFeat)->floatFields->push_back(val);
        }
    }
    
    void KEAAttributeTableInMem::addAttStringField(KEAATTField field, const std::string &val)
    {
        kealib::kea_lock lock(*this->m_mutex); 
        for(auto iterFeat = attRows->begin(); iterFeat != attRows->end(); ++iterFeat)
        {
            (*iterFeat)->strFields->push_back(val);
        }
    }
    
    void KEAAttributeTableInMem::addRows(size_t numRows)
    {        
        KEAATTFeature *feat = nullptr;
        kealib::kea_lock lock(*this->m_mutex); 
        
        for(size_t i = 0; i < numRows; ++i)
        {
            feat = this->createKeaFeature();
            feat->fid = attRows->size();
            attRows->push_back(feat);
        }
    }
    
    void KEAAttributeTableInMem::exportToKeaFile(H5::H5File *keaImg, unsigned int band, unsigned int chunkSize, unsigned int deflate)
    {        
        try
        {
            if(attRows->size() == 0)
            {
                throw KEAATTException("There is no attribute table to be saved to the file.");
            }
            
            std::string bandPathBase = KEA_DATASETNAME_BAND + uint2Str(band);
            
            // Read header size.
            hsize_t *attSize = new hsize_t[5];
            try 
            {   
                hsize_t dimsValue[1];
                dimsValue[0] = 5;
                H5::DataSpace valueDataSpace(1, dimsValue);
                H5::DataSet datasetAttSize = keaImg->openDataSet(bandPathBase + KEA_ATT_SIZE_HEADER );
                datasetAttSize.read(attSize, H5::PredType::NATIVE_HSIZE, valueDataSpace);
                datasetAttSize.close();
                valueDataSpace.close();
            } 
            catch(const H5::Exception &e) 
            {
                throw KEAIOException("The attribute table size field is not present.");
            }
                        
            KEAAttributeIdx *boolFields = nullptr;
            if(this->numBoolFields > 0)
            {
                boolFields = new KEAAttributeIdx[this->numBoolFields];
            }
            KEAAttributeIdx *intFields = nullptr;
            if(this->numIntFields > 0)
            {
                intFields = new KEAAttributeIdx[this->numIntFields];
            }
            KEAAttributeIdx *floatFields = nullptr;
            if(this->numFloatFields > 0)
            {
                floatFields = new KEAAttributeIdx[this->numFloatFields];
            }
            KEAAttributeIdx *stringFields = nullptr;
            if(this->numStringFields > 0)
            {
                stringFields = new KEAAttributeIdx[this->numStringFields];
            }
            
            unsigned int boolFieldsIdx = 0;
            unsigned int intFieldsIdx = 0;
            unsigned int floatFieldsIdx = 0;
            unsigned int stringFieldIdx = 0;
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
                else if((*iterField).second.dataType == kea_att_int)
                {
                    intFields[intFieldsIdx].name = const_cast<char*>((*iterField).second.name.c_str());
                    intFields[intFieldsIdx].idx = (*iterField).second.idx;
                    intFields[intFieldsIdx].usage = const_cast<char*>((*iterField).second.usage.c_str());
                    intFields[intFieldsIdx].colNum = (*iterField).second.colNum;
                    ++intFieldsIdx;
                }
                else if((*iterField).second.dataType == kea_att_float)
                {
                    floatFields[floatFieldsIdx].name = const_cast<char*>((*iterField).second.name.c_str());
                    floatFields[floatFieldsIdx].idx = (*iterField).second.idx;
                    floatFields[floatFieldsIdx].usage = const_cast<char*>((*iterField).second.usage.c_str());
                    floatFields[floatFieldsIdx].colNum = (*iterField).second.colNum;
                    ++floatFieldsIdx;
                }
                else if((*iterField).second.dataType == kea_att_string)
                {
                    stringFields[stringFieldIdx].name = const_cast<char*>((*iterField).second.name.c_str());
                    stringFields[stringFieldIdx].idx = (*iterField).second.idx;
                    stringFields[stringFieldIdx].usage = const_cast<char*>((*iterField).second.usage.c_str());
                    stringFields[stringFieldIdx].colNum = (*iterField).second.colNum;
                    ++stringFieldIdx;
                }
                else
                {
                    throw KEAATTException("Data type was not recognised.");
                }
            }
            
            H5::DataSet *boolDataset = nullptr;
            H5::DataSet *intDataset = nullptr;
            H5::DataSet *floatDataset = nullptr;
            H5::DataSet *strDataset = nullptr;
            H5::DataSet *neighboursDataset = nullptr;
            
            H5::CompType *strTypeDisk = this->createKeaStringCompTypeDisk();
            H5::CompType *strTypeMem = this->createKeaStringCompTypeMem();
            
            if(attSize[0] > 0)
            {
                // THERE IS AN EXISTING TABLE AND YOU NEED TO MAKE SURE THEY ARE BUG ENOUGH.
                H5::CompType *fieldDtMem = this->createAttibuteIdxCompTypeMem();
                H5::CompType *fieldDtDisk = this->createAttibuteIdxCompTypeDisk();
                if(this->numBoolFields > 0)
                {
                    try 
                    {
                        H5::DataSet boolFieldsDataset = keaImg->openDataSet(bandPathBase + KEA_ATT_BOOL_FIELDS_HEADER);
                        H5::DataSpace boolFieldsWriteDataSpace = boolFieldsDataset.getSpace();
                        
                        hsize_t boolFieldsDataDims[1];
                        boolFieldsWriteDataSpace.getSimpleExtentDims(boolFieldsDataDims);
                        
                        if(this->numBoolFields > boolFieldsDataDims[0])
                        {
                            hsize_t extendBoolFieldsDatasetTo[1];
                            extendBoolFieldsDatasetTo[0] = this->numBoolFields;
                            boolFieldsDataset.extend( extendBoolFieldsDatasetTo );
                        }
                        
                        hsize_t boolFieldsOffset[1];
                        boolFieldsOffset[0] = 0;
                        boolFieldsDataDims[0] = this->numBoolFields;
                        
                        boolFieldsWriteDataSpace.close();
                        boolFieldsWriteDataSpace = boolFieldsDataset.getSpace();
                        boolFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolFieldsDataDims, boolFieldsOffset);
                        H5::DataSpace newBoolFieldsDataspace = H5::DataSpace(1, boolFieldsDataDims);
                        
                        boolFieldsDataset.write(boolFields, *fieldDtMem, newBoolFieldsDataspace, boolFieldsWriteDataSpace);

                        boolFieldsWriteDataSpace.close();
                        newBoolFieldsDataspace.close();
                        boolFieldsDataset.close();
                        
                        delete[] boolFields;
                    }
                    catch(const H5::Exception &e)
                    {
                        hsize_t initDimsBoolFieldsDS[1];
                        initDimsBoolFieldsDS[0] = this->numBoolFields;
                        hsize_t maxDimsBoolFieldsDS[1];
                        maxDimsBoolFieldsDS[0] = H5S_UNLIMITED;
                        H5::DataSpace boolFieldsDataSpace = H5::DataSpace(1, initDimsBoolFieldsDS, maxDimsBoolFieldsDS);
                        
                        hsize_t dimsBoolFieldsChunk[1];
                        dimsBoolFieldsChunk[0] = chunkSize;
                        
                        H5::DSetCreatPropList creationBoolFieldsDSPList;
                        creationBoolFieldsDSPList.setChunk(1, dimsBoolFieldsChunk);
                        creationBoolFieldsDSPList.setShuffle();
                        creationBoolFieldsDSPList.setDeflate(deflate);
                        H5::DataSet boolFieldsDataset = keaImg->createDataSet((bandPathBase + KEA_ATT_BOOL_FIELDS_HEADER), *fieldDtDisk, boolFieldsDataSpace, creationBoolFieldsDSPList);
                        
                        hsize_t boolFieldsOffset[1];
                        boolFieldsOffset[0] = 0;
                        hsize_t boolFieldsDataDims[1];
                        boolFieldsDataDims[0] = this->numBoolFields;
                        
                        H5::DataSpace boolFieldsWriteDataSpace = boolFieldsDataset.getSpace();
                        boolFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolFieldsDataDims, boolFieldsOffset);
                        H5::DataSpace newBoolFieldsDataspace = H5::DataSpace(1, boolFieldsDataDims);
                        
                        boolFieldsDataset.write(boolFields, *fieldDtMem, newBoolFieldsDataspace, boolFieldsWriteDataSpace);
                        
                        boolFieldsDataSpace.close();
                        boolFieldsWriteDataSpace.close();
                        newBoolFieldsDataspace.close();
                        boolFieldsDataset.close();
                        
                        delete[] boolFields;
                    }
                }
                
                if(this->numIntFields > 0)
                {
                    try 
                    {
                        H5::DataSet intFieldsDataset = keaImg->openDataSet(bandPathBase + KEA_ATT_INT_FIELDS_HEADER);
                        H5::DataSpace intFieldsWriteDataSpace = intFieldsDataset.getSpace();
                        
                        hsize_t intFieldsDataDims[1];
                        intFieldsWriteDataSpace.getSimpleExtentDims(intFieldsDataDims);
                        
                        if(this->numIntFields > intFieldsDataDims[0])
                        {
                            hsize_t extendIntFieldsDatasetTo[1];
                            extendIntFieldsDatasetTo[0] = this->numIntFields;
                            intFieldsDataset.extend( extendIntFieldsDatasetTo );
                        }
                        
                        hsize_t intFieldsOffset[1];
                        intFieldsOffset[0] = 0;
                        intFieldsDataDims[0] = this->numIntFields;
                        
                        intFieldsWriteDataSpace.close();
                        intFieldsWriteDataSpace = intFieldsDataset.getSpace();
                        intFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intFieldsDataDims, intFieldsOffset);
                        H5::DataSpace newIntFieldsDataspace = H5::DataSpace(1, intFieldsDataDims);
                        
                        intFieldsDataset.write(intFields, *fieldDtMem, newIntFieldsDataspace, intFieldsWriteDataSpace);
                        
                        intFieldsWriteDataSpace.close();
                        newIntFieldsDataspace.close();
                        intFieldsDataset.close();
                        
                        delete[] intFields;

                    }
                    catch (const H5::Exception &e)
                    {
                        hsize_t initDimsIntFieldsDS[1];
                        initDimsIntFieldsDS[0] = this->numIntFields;
                        hsize_t maxDimsIntFieldsDS[1];
                        maxDimsIntFieldsDS[0] = H5S_UNLIMITED;
                        H5::DataSpace intFieldsDataSpace = H5::DataSpace(1, initDimsIntFieldsDS, maxDimsIntFieldsDS);
                        
                        hsize_t dimsIntFieldsChunk[1];
                        dimsIntFieldsChunk[0] = chunkSize;
                        
                        H5::DSetCreatPropList creationIntFieldsDSPList;
                        creationIntFieldsDSPList.setChunk(1, dimsIntFieldsChunk);
                        creationIntFieldsDSPList.setShuffle();
                        creationIntFieldsDSPList.setDeflate(deflate);
                        H5::DataSet intFieldsDataset = keaImg->createDataSet((bandPathBase + KEA_ATT_INT_FIELDS_HEADER), *fieldDtDisk, intFieldsDataSpace, creationIntFieldsDSPList);
                        
                        hsize_t intFieldsOffset[1];
                        intFieldsOffset[0] = 0;
                        hsize_t intFieldsDataDims[1];
                        intFieldsDataDims[0] = this->numIntFields;
                        
                        H5::DataSpace intFieldsWriteDataSpace = intFieldsDataset.getSpace();
                        intFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intFieldsDataDims, intFieldsOffset);
                        H5::DataSpace newIntFieldsDataspace = H5::DataSpace(1, intFieldsDataDims);
                        
                        intFieldsDataset.write(intFields, *fieldDtMem, newIntFieldsDataspace, intFieldsWriteDataSpace);
                        
                        intFieldsDataSpace.close();
                        intFieldsWriteDataSpace.close();
                        newIntFieldsDataspace.close();
                        intFieldsDataset.close();
                        
                        delete[] intFields;
                    }
                }
                
                if(this->numFloatFields > 0)
                {
                    try
                    {
                        H5::DataSet floatFieldsDataset = keaImg->openDataSet(bandPathBase + KEA_ATT_FLOAT_FIELDS_HEADER);
                        H5::DataSpace floatFieldsWriteDataSpace = floatFieldsDataset.getSpace();
                        
                        hsize_t floatFieldsDataDims[1];
                        floatFieldsWriteDataSpace.getSimpleExtentDims(floatFieldsDataDims);
                        
                        if(this->numFloatFields > floatFieldsDataDims[0])
                        {
                            hsize_t extendFloatFieldsDatasetTo[1];
                            extendFloatFieldsDatasetTo[0] = this->numFloatFields;
                            floatFieldsDataset.extend( extendFloatFieldsDatasetTo );
                        }
                        
                        hsize_t floatFieldsOffset[1];
                        floatFieldsOffset[0] = 0;
                        floatFieldsDataDims[0] = this->numFloatFields;
                        
                        floatFieldsWriteDataSpace.close();
                        floatFieldsWriteDataSpace = floatFieldsDataset.getSpace();
                        floatFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatFieldsDataDims, floatFieldsOffset);
                        H5::DataSpace newFloatFieldsDataspace = H5::DataSpace(1, floatFieldsDataDims);
                        
                        floatFieldsDataset.write(floatFields, *fieldDtMem, newFloatFieldsDataspace, floatFieldsWriteDataSpace);
                        
                        floatFieldsWriteDataSpace.close();
                        newFloatFieldsDataspace.close();
                        floatFieldsDataset.close();
                        
                        delete[] floatFields;
                    }
                    catch(const H5::Exception &e)
                    {
                        hsize_t initDimsFloatFieldsDS[1];
                        initDimsFloatFieldsDS[0] = this->numFloatFields;
                        hsize_t maxDimsFloatFieldsDS[1];
                        maxDimsFloatFieldsDS[0] = H5S_UNLIMITED;
                        H5::DataSpace floatFieldsDataSpace = H5::DataSpace(1, initDimsFloatFieldsDS, maxDimsFloatFieldsDS);
                        
                        hsize_t dimsFloatFieldsChunk[1];
                        dimsFloatFieldsChunk[0] = chunkSize;
                        
                        H5::DSetCreatPropList creationFloatFieldsDSPList;
                        creationFloatFieldsDSPList.setChunk(1, dimsFloatFieldsChunk);
                        creationFloatFieldsDSPList.setShuffle();
                        creationFloatFieldsDSPList.setDeflate(deflate);
                        H5::DataSet floatFieldsDataset = keaImg->createDataSet((bandPathBase + KEA_ATT_FLOAT_FIELDS_HEADER), *fieldDtDisk, floatFieldsDataSpace, creationFloatFieldsDSPList);
                        
                        hsize_t floatFieldsOffset[1];
                        floatFieldsOffset[0] = 0;
                        hsize_t floatFieldsDataDims[1];
                        floatFieldsDataDims[0] = this->numFloatFields;
                        
                        H5::DataSpace floatFieldsWriteDataSpace = floatFieldsDataset.getSpace();
                        floatFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatFieldsDataDims, floatFieldsOffset);
                        H5::DataSpace newFloatFieldsDataspace = H5::DataSpace(1, floatFieldsDataDims);
                        
                        floatFieldsDataset.write(floatFields, *fieldDtMem, newFloatFieldsDataspace, floatFieldsWriteDataSpace);
                        
                        floatFieldsDataSpace.close();
                        floatFieldsWriteDataSpace.close();
                        newFloatFieldsDataspace.close();
                        floatFieldsDataset.close();
                        
                        delete[] floatFields;
                    }
                }
                
                if(this->numStringFields > 0)
                {
                    try
                    {
                        H5::DataSet stringFieldsDataset = keaImg->openDataSet(bandPathBase + KEA_ATT_STRING_FIELDS_HEADER);
                        H5::DataSpace stringFieldsWriteDataSpace = stringFieldsDataset.getSpace();
                        
                        hsize_t stringFieldsDataDims[1];
                        stringFieldsWriteDataSpace.getSimpleExtentDims(stringFieldsDataDims);
                        
                        if(this->numStringFields > stringFieldsDataDims[0])
                        {
                            hsize_t extendStringFieldsDatasetTo[1];
                            extendStringFieldsDatasetTo[0] = this->numStringFields;
                            stringFieldsDataset.extend( extendStringFieldsDatasetTo );
                        }
                        
                        hsize_t stringFieldsOffset[1];
                        stringFieldsOffset[0] = 0;
                        stringFieldsDataDims[0] = this->numStringFields;
                        
                        stringFieldsWriteDataSpace.close();
                        stringFieldsWriteDataSpace = stringFieldsDataset.getSpace();
                        stringFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, stringFieldsDataDims, stringFieldsOffset);
                        H5::DataSpace newStringFieldsDataspace = H5::DataSpace(1, stringFieldsDataDims);
                        
                        stringFieldsDataset.write(stringFields, *fieldDtMem, newStringFieldsDataspace, stringFieldsWriteDataSpace);
                        
                        stringFieldsWriteDataSpace.close();
                        newStringFieldsDataspace.close();
                        stringFieldsDataset.close();
                        
                        delete[] stringFields;
                    }
                    catch(const H5::Exception &e)
                    {
                        hsize_t initDimsStringFieldsDS[1];
                        initDimsStringFieldsDS[0] = 0;
                        hsize_t maxDimsStringFieldsDS[1];
                        maxDimsStringFieldsDS[0] = H5S_UNLIMITED;
                        H5::DataSpace stringFieldsDataSpace = H5::DataSpace(1, initDimsStringFieldsDS, maxDimsStringFieldsDS);
                        
                        hsize_t dimsStringFieldsChunk[1];
                        dimsStringFieldsChunk[0] = chunkSize;
                        
                        H5::DSetCreatPropList creationStringFieldsDSPList;
                        creationStringFieldsDSPList.setChunk(1, dimsStringFieldsChunk);
                        creationStringFieldsDSPList.setShuffle();
                        creationStringFieldsDSPList.setDeflate(deflate);
                        H5::DataSet stringFieldsDataset = keaImg->createDataSet((bandPathBase + KEA_ATT_STRING_FIELDS_HEADER), *fieldDtDisk, stringFieldsDataSpace, creationStringFieldsDSPList);
                        
                        hsize_t extendStringFieldsDatasetTo[1];
                        extendStringFieldsDatasetTo[0] = this->numStringFields;
                        stringFieldsDataset.extend( extendStringFieldsDatasetTo );
                        
                        hsize_t stringFieldsOffset[1];
                        stringFieldsOffset[0] = 0;
                        hsize_t stringFieldsDataDims[1];
                        stringFieldsDataDims[0] = this->numStringFields;
                        
                        H5::DataSpace stringFieldsWriteDataSpace = stringFieldsDataset.getSpace();
                        stringFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, stringFieldsDataDims, stringFieldsOffset);
                        H5::DataSpace newStringFieldsDataspace = H5::DataSpace(1, stringFieldsDataDims);

                        stringFieldsDataset.write(stringFields, *fieldDtMem, newStringFieldsDataspace, stringFieldsWriteDataSpace);
                        
                        stringFieldsDataSpace.close();
                        stringFieldsWriteDataSpace.close();
                        newStringFieldsDataspace.close();
                        stringFieldsDataset.close();
                        
                        delete[] stringFields;
                    }
                }
                delete fieldDtDisk;
                delete fieldDtMem;
                
                if(this->numBoolFields > 0)
                {
                    try
                    {
                        boolDataset = new H5::DataSet(keaImg->openDataSet(bandPathBase + KEA_ATT_BOOL_DATA));
                        H5::DataSpace dimsDataSpace = boolDataset->getSpace();
                        
                        hsize_t dataDims[2];
                        dimsDataSpace.getSimpleExtentDims(dataDims);
                        hsize_t extendDatasetTo[2];
                        bool extend = false;
                        
                        if(this->attRows->size() > dataDims[0])
                        {
                            extendDatasetTo[0] = this->attRows->size();
                            extend = true;
                        }
                        else
                        {
                            extendDatasetTo[0] = dataDims[0];
                        }
                        
                        if(this->numBoolFields > dataDims[1])
                        {
                            extendDatasetTo[1] = this->numBoolFields;
                            extend = true;
                        }
                        else
                        {
                            extendDatasetTo[1] = dataDims[1];
                        }
                        
                        if(extend)
                        {
                            boolDataset->extend(extendDatasetTo);
                        }
                        
                        dimsDataSpace.close();
                    }
                    catch(const H5::Exception &e)
                    {
                        // Create the boolean
                        hsize_t initDimsBools[2];
                        initDimsBools[0] = this->attRows->size();
                        initDimsBools[1] = this->numBoolFields;
                        hsize_t maxDimsBool[2];
                        maxDimsBool[0] = H5S_UNLIMITED;
                        maxDimsBool[1] = H5S_UNLIMITED;
                        H5::DataSpace boolDataSpace = H5::DataSpace(2, initDimsBools, maxDimsBool);
                        
                        hsize_t dimsBoolChunk[2];
                        dimsBoolChunk[0] = chunkSize;
                        dimsBoolChunk[1] = 1;
                        
                        int fillValueBool = 0;
                        H5::DSetCreatPropList creationBoolDSPList;
                        creationBoolDSPList.setChunk(2, dimsBoolChunk);
                        creationBoolDSPList.setShuffle();
                        creationBoolDSPList.setDeflate(deflate);
                        creationBoolDSPList.setFillValue( H5::PredType::NATIVE_INT, &fillValueBool);
                        
                        boolDataset = new H5::DataSet(keaImg->createDataSet((bandPathBase + KEA_ATT_BOOL_DATA), H5::PredType::STD_I8LE, boolDataSpace, creationBoolDSPList));
                        boolDataSpace.close();
                    }
                }
                
                if(this->numIntFields > 0)
                {
                    try
                    {
                        intDataset = new H5::DataSet(keaImg->openDataSet(bandPathBase + KEA_ATT_INT_DATA));
                        H5::DataSpace dimsDataSpace = intDataset->getSpace();
                        
                        hsize_t dataDims[2];
                        dimsDataSpace.getSimpleExtentDims(dataDims);
                        hsize_t extendDatasetTo[2];
                        bool extend = false;
                        
                        if(this->attRows->size() > dataDims[0])
                        {
                            extendDatasetTo[0] = this->attRows->size();
                            extend = true;
                        }
                        else
                        {
                            extendDatasetTo[0] = dataDims[0];
                        }
                        
                        if(this->numIntFields > dataDims[1])
                        {
                            extendDatasetTo[1] = this->numIntFields;
                            extend = true;
                        }
                        else
                        {
                            extendDatasetTo[1] = dataDims[1];
                        }
                        
                        if(extend)
                        {
                            intDataset->extend(extendDatasetTo);
                        }
                        
                        dimsDataSpace.close();
                    }
                    catch(const H5::Exception &e)
                    {
                        hsize_t initDimsInts[2];
                        initDimsInts[0] = this->attRows->size();
                        initDimsInts[1] = this->numIntFields;
                        hsize_t maxDimsInt[2];
                        maxDimsInt[0] = H5S_UNLIMITED;
                        maxDimsInt[1] = H5S_UNLIMITED;
                        H5::DataSpace intDataSpace = H5::DataSpace(2, initDimsInts, maxDimsInt);
                        
                        hsize_t dimsIntChunk[2];
                        dimsIntChunk[0] = chunkSize;
                        dimsIntChunk[1] = 1;
                        
                        int64_t fillValueInt = 0;
                        H5::DSetCreatPropList creationIntDSPList;
                        creationIntDSPList.setChunk(2, dimsIntChunk);
                        creationIntDSPList.setShuffle();
                        creationIntDSPList.setDeflate(deflate);
                        creationIntDSPList.setFillValue( H5::PredType::NATIVE_INT64, &fillValueInt);
                        
                        intDataset = new H5::DataSet(keaImg->createDataSet((bandPathBase + KEA_ATT_INT_DATA), H5::PredType::STD_I64LE, intDataSpace, creationIntDSPList));
                        intDataSpace.close();
                    }
                }
                
                if(this->numFloatFields > 0)
                {
                    try
                    {
                        floatDataset = new H5::DataSet(keaImg->openDataSet(bandPathBase + KEA_ATT_FLOAT_DATA));
                        H5::DataSpace dimsDataSpace = floatDataset->getSpace();
                        
                        hsize_t dataDims[2];
                        dimsDataSpace.getSimpleExtentDims(dataDims);
                        hsize_t extendDatasetTo[2];
                        bool extend = false;
                        
                        if(this->attRows->size() > dataDims[0])
                        {
                            extendDatasetTo[0] = this->attRows->size();
                            extend = true;
                        }
                        else
                        {
                            extendDatasetTo[0] = dataDims[0];
                        }
                        
                        if(this->numFloatFields > dataDims[1])
                        {
                            extendDatasetTo[1] = this->numFloatFields;
                            extend = true;
                        }
                        else
                        {
                            extendDatasetTo[1] = dataDims[1];
                        }
                        
                        if(extend)
                        {
                            floatDataset->extend(extendDatasetTo);
                        }
                        
                        dimsDataSpace.close();
                    }
                    catch(const H5::Exception &e)
                    {
                        hsize_t initDimsFloats[2];
                        initDimsFloats[0] = this->attRows->size();
                        initDimsFloats[1] = this->numFloatFields;
                        hsize_t maxDimsFloat[2];
                        maxDimsFloat[0] = H5S_UNLIMITED;
                        maxDimsFloat[1] = H5S_UNLIMITED;
                        H5::DataSpace floatDataSpace = H5::DataSpace(2, initDimsFloats, maxDimsFloat);
                        
                        hsize_t dimsFloatChunk[2];
                        dimsFloatChunk[0] = chunkSize;
                        dimsFloatChunk[1] = 1;
                        
                        double fillValueFloat = 0;
                        H5::DSetCreatPropList creationFloatDSPList;
                        creationFloatDSPList.setChunk(2, dimsFloatChunk);
                        creationFloatDSPList.setShuffle();
                        creationFloatDSPList.setDeflate(deflate);
                        creationFloatDSPList.setFillValue( H5::PredType::NATIVE_DOUBLE, &fillValueFloat);
                        
                        floatDataset = new H5::DataSet(keaImg->createDataSet((bandPathBase + KEA_ATT_FLOAT_DATA), H5::PredType::IEEE_F64LE, floatDataSpace, creationFloatDSPList));
                        floatDataSpace.close();
                    }
                }
                
                if(this->numStringFields > 0)
                {
                    try
                    {
                        strDataset = new H5::DataSet(keaImg->openDataSet(bandPathBase + KEA_ATT_STRING_DATA));
                        H5::DataSpace dimsDataSpace = strDataset->getSpace();
                        
                        hsize_t dataDims[2];
                        dimsDataSpace.getSimpleExtentDims(dataDims);
                        hsize_t extendDatasetTo[2];
                        bool extend = false;
                        
                        if(this->attRows->size() > dataDims[0])
                        {
                            extendDatasetTo[0] = this->attRows->size();
                            extend = true;
                        }
                        else
                        {
                            extendDatasetTo[0] = dataDims[0];
                        }
                        
                        if(this->numStringFields > dataDims[1])
                        {
                            extendDatasetTo[1] = this->numStringFields;
                            extend = true;
                        }
                        else
                        {
                            extendDatasetTo[1] = dataDims[1];
                        }
                        
                        if(extend)
                        {
                            strDataset->extend(extendDatasetTo);
                        }
                        
                        dimsDataSpace.close();
                    }
                    catch(const H5::Exception &e)
                    {
                        hsize_t initDimsString[2];
                        initDimsString[0] = this->attRows->size();
                        initDimsString[1] = this->numStringFields;
                        hsize_t maxDimsString[2];
                        maxDimsString[0] = H5S_UNLIMITED;
                        maxDimsString[1] = H5S_UNLIMITED;
                        H5::DataSpace stringDataSpace = H5::DataSpace(2, initDimsString, maxDimsString);
                        
                        hsize_t dimsStringChunk[2];
                        dimsStringChunk[0] = chunkSize;
                        dimsStringChunk[1] = 1;
                        
                        KEAString fillValueStr = KEAString();
                        fillValueStr.str = const_cast<char*>(std::string("").c_str());
                        H5::DSetCreatPropList creationStringDSPList;
                        creationStringDSPList.setChunk(2, dimsStringChunk);
                        creationStringDSPList.setShuffle();
                        creationStringDSPList.setDeflate(deflate);
                        creationStringDSPList.setFillValue(*strTypeMem, &fillValueStr);
                        strDataset = new H5::DataSet(keaImg->createDataSet((bandPathBase + KEA_ATT_STRING_DATA), *strTypeDisk, stringDataSpace, creationStringDSPList));
                        stringDataSpace.close();
                    }
                }
                
                try
                {
                    neighboursDataset = new H5::DataSet(keaImg->openDataSet(bandPathBase + KEA_ATT_NEIGHBOURS_DATA));
                    H5::DataSpace dimsDataSpace = neighboursDataset->getSpace();
                    
                    hsize_t dataDims[1];
                    dimsDataSpace.getSimpleExtentDims(dataDims);
                    hsize_t extendDatasetTo[1];
                    
                    if(this->attRows->size() > dataDims[0])
                    {
                        extendDatasetTo[0] = this->attRows->size();
                        neighboursDataset->extend(extendDatasetTo);
                    }
                    
                    dimsDataSpace.close();
                }
                catch(const H5::Exception &e)
                {
                    throw KEAIOException(e.getDetailMsg());
                }
            }
            else
            {
                // THERE IS NO EXISTING TABLE AND THE DATA STRUCTURES NEEDS TO BE CREATED.
                H5::CompType *fieldDtDisk = this->createAttibuteIdxCompTypeDisk();
                H5::CompType *fieldDtMem = this->createAttibuteIdxCompTypeMem();
                if(this->numBoolFields > 0)
                {
                    hsize_t initDimsBoolFieldsDS[1];
                    initDimsBoolFieldsDS[0] = this->numBoolFields;
                    hsize_t maxDimsBoolFieldsDS[1];
                    maxDimsBoolFieldsDS[0] = H5S_UNLIMITED;
                    H5::DataSpace boolFieldsDataSpace = H5::DataSpace(1, initDimsBoolFieldsDS, maxDimsBoolFieldsDS);
                    
                    hsize_t dimsBoolFieldsChunk[1];
                    dimsBoolFieldsChunk[0] = chunkSize;
                    
                    H5::DSetCreatPropList creationBoolFieldsDSPList;
                    creationBoolFieldsDSPList.setChunk(1, dimsBoolFieldsChunk);
                    creationBoolFieldsDSPList.setShuffle();
                    creationBoolFieldsDSPList.setDeflate(deflate);
                    H5::DataSet boolFieldsDataset = keaImg->createDataSet((bandPathBase + KEA_ATT_BOOL_FIELDS_HEADER), *fieldDtDisk, boolFieldsDataSpace, creationBoolFieldsDSPList);
                    
                    hsize_t boolFieldsOffset[1];
                    boolFieldsOffset[0] = 0;
                    hsize_t boolFieldsDataDims[1];
                    boolFieldsDataDims[0] = this->numBoolFields;
                    
                    H5::DataSpace boolFieldsWriteDataSpace = boolFieldsDataset.getSpace();
                    boolFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolFieldsDataDims, boolFieldsOffset);
                    H5::DataSpace newBoolFieldsDataspace = H5::DataSpace(1, boolFieldsDataDims);
                    
                    boolFieldsDataset.write(boolFields, *fieldDtMem, newBoolFieldsDataspace, boolFieldsWriteDataSpace);
                    
                    boolFieldsDataSpace.close();
                    boolFieldsWriteDataSpace.close();
                    newBoolFieldsDataspace.close();
                    boolFieldsDataset.close();
                    
                    delete[] boolFields;
                }
                
                if(this->numIntFields > 0)
                {
                    hsize_t initDimsIntFieldsDS[1];
                    initDimsIntFieldsDS[0] = this->numIntFields;
                    hsize_t maxDimsIntFieldsDS[1];
                    maxDimsIntFieldsDS[0] = H5S_UNLIMITED;
                    H5::DataSpace intFieldsDataSpace = H5::DataSpace(1, initDimsIntFieldsDS, maxDimsIntFieldsDS);
                    
                    hsize_t dimsIntFieldsChunk[1];
                    dimsIntFieldsChunk[0] = chunkSize;
                    
                    H5::DSetCreatPropList creationIntFieldsDSPList;
                    creationIntFieldsDSPList.setChunk(1, dimsIntFieldsChunk);
                    creationIntFieldsDSPList.setShuffle();
                    creationIntFieldsDSPList.setDeflate(deflate);
                    H5::DataSet intFieldsDataset = keaImg->createDataSet((bandPathBase + KEA_ATT_INT_FIELDS_HEADER), *fieldDtDisk, intFieldsDataSpace, creationIntFieldsDSPList);
                    
                    hsize_t intFieldsOffset[1];
                    intFieldsOffset[0] = 0;
                    hsize_t intFieldsDataDims[1];
                    intFieldsDataDims[0] = this->numIntFields;
                    
                    H5::DataSpace intFieldsWriteDataSpace = intFieldsDataset.getSpace();
                    intFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intFieldsDataDims, intFieldsOffset);
                    H5::DataSpace newIntFieldsDataspace = H5::DataSpace(1, intFieldsDataDims);
                    
                    intFieldsDataset.write(intFields, *fieldDtMem, newIntFieldsDataspace, intFieldsWriteDataSpace);
                    
                    intFieldsDataSpace.close();
                    intFieldsWriteDataSpace.close();
                    newIntFieldsDataspace.close();
                    intFieldsDataset.close();
                    
                    delete[] intFields;
                }
                
                if(this->numFloatFields > 0)
                {
                    hsize_t initDimsFloatFieldsDS[1];
                    initDimsFloatFieldsDS[0] = this->numFloatFields;
                    hsize_t maxDimsFloatFieldsDS[1];
                    maxDimsFloatFieldsDS[0] = H5S_UNLIMITED;
                    H5::DataSpace floatFieldsDataSpace = H5::DataSpace(1, initDimsFloatFieldsDS, maxDimsFloatFieldsDS);
                    
                    hsize_t dimsFloatFieldsChunk[1];
                    dimsFloatFieldsChunk[0] = chunkSize;
                    
                    H5::DSetCreatPropList creationFloatFieldsDSPList;
                    creationFloatFieldsDSPList.setChunk(1, dimsFloatFieldsChunk);
                    creationFloatFieldsDSPList.setShuffle();
                    creationFloatFieldsDSPList.setDeflate(deflate);
                    H5::DataSet floatFieldsDataset = keaImg->createDataSet((bandPathBase + KEA_ATT_FLOAT_FIELDS_HEADER), *fieldDtDisk, floatFieldsDataSpace, creationFloatFieldsDSPList);
                    
                    hsize_t floatFieldsOffset[1];
                    floatFieldsOffset[0] = 0;
                    hsize_t floatFieldsDataDims[1];
                    floatFieldsDataDims[0] = this->numFloatFields;
                    
                    H5::DataSpace floatFieldsWriteDataSpace = floatFieldsDataset.getSpace();
                    floatFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatFieldsDataDims, floatFieldsOffset);
                    H5::DataSpace newFloatFieldsDataspace = H5::DataSpace(1, floatFieldsDataDims);
                    
                    floatFieldsDataset.write(floatFields, *fieldDtMem, newFloatFieldsDataspace, floatFieldsWriteDataSpace);
                    
                    floatFieldsDataSpace.close();
                    floatFieldsWriteDataSpace.close();
                    newFloatFieldsDataspace.close();
                    floatFieldsDataset.close();
                    
                    delete[] floatFields;
                }
                
                if(this->numStringFields > 0)
                {
                    hsize_t initDimsStringFieldsDS[1];
                    initDimsStringFieldsDS[0] = 0;
                    hsize_t maxDimsStringFieldsDS[1];
                    maxDimsStringFieldsDS[0] = H5S_UNLIMITED;
                    H5::DataSpace stringFieldsDataSpace = H5::DataSpace(1, initDimsStringFieldsDS, maxDimsStringFieldsDS);
                    
                    hsize_t dimsStringFieldsChunk[1];
                    dimsStringFieldsChunk[0] = chunkSize;
                    
                    H5::DSetCreatPropList creationStringFieldsDSPList;
                    creationStringFieldsDSPList.setChunk(1, dimsStringFieldsChunk);
                    creationStringFieldsDSPList.setShuffle();
                    creationStringFieldsDSPList.setDeflate(deflate);
                    H5::DataSet stringFieldsDataset = keaImg->createDataSet((bandPathBase + KEA_ATT_STRING_FIELDS_HEADER), *fieldDtDisk, stringFieldsDataSpace, creationStringFieldsDSPList);
                    
                    hsize_t extendStringFieldsDatasetTo[1];
                    extendStringFieldsDatasetTo[0] = this->numStringFields;
                    stringFieldsDataset.extend( extendStringFieldsDatasetTo );
                    
                    hsize_t stringFieldsOffset[1];
                    stringFieldsOffset[0] = 0;
                    hsize_t stringFieldsDataDims[1];
                    stringFieldsDataDims[0] = this->numStringFields;
                    
                    H5::DataSpace stringFieldsWriteDataSpace = stringFieldsDataset.getSpace();
                    stringFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, stringFieldsDataDims, stringFieldsOffset);
                    H5::DataSpace newStringFieldsDataspace = H5::DataSpace(1, stringFieldsDataDims);
                    
                    stringFieldsDataset.write(stringFields, *fieldDtMem, newStringFieldsDataspace, stringFieldsWriteDataSpace);
                    
                    stringFieldsDataSpace.close();
                    stringFieldsWriteDataSpace.close();
                    newStringFieldsDataspace.close();
                    stringFieldsDataset.close();
                    
                    delete[] stringFields;
                }
                delete fieldDtDisk;
                delete fieldDtMem;
                
                if(this->numBoolFields > 0)
                {
                    // Create the boolean
                    hsize_t initDimsBools[2];
                    initDimsBools[0] = this->attRows->size();
                    initDimsBools[1] = this->numBoolFields;
                    hsize_t maxDimsBool[2];
                    maxDimsBool[0] = H5S_UNLIMITED;
                    maxDimsBool[1] = H5S_UNLIMITED;
                    H5::DataSpace boolDataSpace = H5::DataSpace(2, initDimsBools, maxDimsBool);
                    
                    hsize_t dimsBoolChunk[2];
                    dimsBoolChunk[0] = chunkSize;
                    dimsBoolChunk[1] = 1;
                    
                    int fillValueBool = 0;
                    H5::DSetCreatPropList creationBoolDSPList;
                    creationBoolDSPList.setChunk(2, dimsBoolChunk);
                    creationBoolDSPList.setShuffle();
                    creationBoolDSPList.setDeflate(deflate);
                    creationBoolDSPList.setFillValue( H5::PredType::NATIVE_INT, &fillValueBool);
                    
                    boolDataset = new H5::DataSet(keaImg->createDataSet((bandPathBase + KEA_ATT_BOOL_DATA), H5::PredType::STD_I8LE, boolDataSpace, creationBoolDSPList));
                    boolDataSpace.close();
                }
                
                if(this->numIntFields > 0)
                {
                    // Create the integer
                    hsize_t initDimsInts[2];
                    initDimsInts[0] = this->attRows->size();
                    initDimsInts[1] = this->numIntFields;
                    hsize_t maxDimsInt[2];
                    maxDimsInt[0] = H5S_UNLIMITED;
                    maxDimsInt[1] = H5S_UNLIMITED;
                    H5::DataSpace intDataSpace = H5::DataSpace(2, initDimsInts, maxDimsInt);
                    
                    hsize_t dimsIntChunk[2];
                    dimsIntChunk[0] = chunkSize;
                    dimsIntChunk[1] = 1;
                    
                    int64_t fillValueInt = 0;
                    H5::DSetCreatPropList creationIntDSPList;
                    creationIntDSPList.setChunk(2, dimsIntChunk);
                    creationIntDSPList.setShuffle();
                    creationIntDSPList.setDeflate(deflate);
                    creationIntDSPList.setFillValue( H5::PredType::NATIVE_INT64, &fillValueInt);
                    
                    intDataset = new H5::DataSet(keaImg->createDataSet((bandPathBase + KEA_ATT_INT_DATA), H5::PredType::STD_I64LE, intDataSpace, creationIntDSPList));
                    intDataSpace.close();
                }
                
                if(this->numFloatFields > 0)
                {
                    // Create the float
                    hsize_t initDimsFloats[2];
                    initDimsFloats[0] = this->attRows->size();
                    initDimsFloats[1] = this->numFloatFields;
                    hsize_t maxDimsFloat[2];
                    maxDimsFloat[0] = H5S_UNLIMITED;
                    maxDimsFloat[1] = H5S_UNLIMITED;
                    H5::DataSpace floatDataSpace = H5::DataSpace(2, initDimsFloats, maxDimsFloat);
                    
                    hsize_t dimsFloatChunk[2];
                    dimsFloatChunk[0] = chunkSize;
                    dimsFloatChunk[1] = 1;
                    
                    double fillValueFloat = 0;
                    H5::DSetCreatPropList creationFloatDSPList;
                    creationFloatDSPList.setChunk(2, dimsFloatChunk);
                    creationFloatDSPList.setShuffle();
                    creationFloatDSPList.setDeflate(deflate);
                    creationFloatDSPList.setFillValue( H5::PredType::NATIVE_DOUBLE, &fillValueFloat);
                    
                    floatDataset = new H5::DataSet(keaImg->createDataSet((bandPathBase + KEA_ATT_FLOAT_DATA), H5::PredType::IEEE_F64LE, floatDataSpace, creationFloatDSPList));
                    floatDataSpace.close();
                }
                
                if(this->numStringFields > 0)
                {
                    // Create the string
                    hsize_t initDimsString[2];
                    initDimsString[0] = this->attRows->size();
                    initDimsString[1] = this->numStringFields;
                    hsize_t maxDimsString[2];
                    maxDimsString[0] = H5S_UNLIMITED;
                    maxDimsString[1] = H5S_UNLIMITED;
                    H5::DataSpace stringDataSpace = H5::DataSpace(2, initDimsString, maxDimsString);
                    
                    hsize_t dimsStringChunk[2];
                    dimsStringChunk[0] = chunkSize;
                    dimsStringChunk[1] = 1;
                    
                    KEAString fillValueStr = KEAString();
                    fillValueStr.str = const_cast<char*>(std::string("").c_str());
                    H5::DSetCreatPropList creationStringDSPList;
                    creationStringDSPList.setChunk(2, dimsStringChunk);
                    creationStringDSPList.setShuffle();
                    creationStringDSPList.setDeflate(deflate);
                    creationStringDSPList.setFillValue(*strTypeMem, &fillValueStr);
                    strDataset = new H5::DataSet(keaImg->createDataSet((bandPathBase + KEA_ATT_STRING_DATA), *strTypeDisk, stringDataSpace, creationStringDSPList));
                    stringDataSpace.close();
                }
                
                // Create Neighbours dataset
                hsize_t initDimsNeighboursDS[1];
                initDimsNeighboursDS[0] = this->attRows->size();
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
                        
            // WRITE DATA INTO THE STRUCTURE.
            size_t numOfBlocks = 0;
            numOfBlocks = floor(((double)attRows->size()/chunkSize));
            size_t remainRows = attRows->size() - (numOfBlocks * chunkSize);
                        
            int *boolData = nullptr;
            if(this->numBoolFields > 0)
            {
                boolData = new int[this->numBoolFields*chunkSize];
            }
            int64_t *intData = nullptr;
            if(this->numIntFields > 0)
            {
                intData = new int64_t[this->numIntFields*chunkSize];
            }
            double *floatData = nullptr;
            if(this->numFloatFields > 0)
            {
                floatData = new double[this->numFloatFields*chunkSize];
            }
            KEAString *stringData = nullptr;
            if(this->numStringFields > 0)
            {
                stringData = new KEAString[this->numStringFields*chunkSize];
            }
            
            VarLenFieldHDF *neighbourVals = new VarLenFieldHDF[chunkSize];
            
            if(numOfBlocks > 0)
            {
                hsize_t boolDataOffset[2];
                boolDataOffset[0] = 0;
                boolDataOffset[1] = 0;
                hsize_t boolDataDims[2];
                boolDataDims[0] = chunkSize;
                boolDataDims[1] = this->numBoolFields;
                
                hsize_t intDataOffset[2];
                intDataOffset[0] = 0;
                intDataOffset[1] = 0;
                hsize_t intDataDims[2];
                intDataDims[0] = chunkSize;
                intDataDims[1] = this->numIntFields;
                
                hsize_t floatDataOffset[2];
                floatDataOffset[0] = 0;
                floatDataOffset[1] = 0;
                hsize_t floatDataDims[2];
                floatDataDims[0] = chunkSize;
                floatDataDims[1] = this->numFloatFields;
                
                hsize_t stringDataOffset[2];
                stringDataOffset[0] = 0;
                stringDataOffset[1] = 0;
                hsize_t stringDataDims[2];
                stringDataDims[0] = chunkSize;
                stringDataDims[1] = this->numStringFields;
                
                hsize_t neighboursDataOffset[1];
                neighboursDataOffset[0] = 0;
                hsize_t neighboursDataDims[1];
                neighboursDataDims[0] = chunkSize;
                H5::DataType intVarLenMemDT = H5::VarLenType(&H5::PredType::NATIVE_HSIZE);
                H5::DataSpace memNeighboursDataspace = H5::DataSpace(1, neighboursDataDims);
                
                size_t rowOff = 0;
                KEAATTFeature *keaFeat = nullptr;
                for(size_t n = 0; n < numOfBlocks; ++n)
                {
                    rowOff = n * chunkSize;
                    
                    for(size_t i = 0; i < chunkSize; ++i)
                    {
                        keaFeat = attRows->at(rowOff+i);
                        for(size_t j = 0; j < this->numBoolFields; ++j)
                        {
                            boolData[(i*this->numBoolFields)+j] = keaFeat->boolFields->at(j);
                        }
                        for(size_t j = 0; j < this->numIntFields; ++j)
                        {
                            intData[(i*this->numIntFields)+j] = keaFeat->intFields->at(j);
                        }
                        for(size_t j = 0; j < this->numFloatFields; ++j)
                        {
                            floatData[(i*this->numFloatFields)+j] = keaFeat->floatFields->at(j);
                        }
                        for(size_t j = 0; j < this->numStringFields; ++j)
                        {
                            stringData[(i*this->numStringFields)+j].str = const_cast<char*>(keaFeat->strFields->at(j).c_str());
                        }
                        
                        neighbourVals[i].length = 0;
                        neighbourVals[i].p = nullptr;
                        if(keaFeat->neighbours->size() > 0)
                        {
                            neighbourVals[i].length = keaFeat->neighbours->size();
                            neighbourVals[i].p = new hsize_t[keaFeat->neighbours->size()];
                            for(unsigned int k = 0; k < keaFeat->neighbours->size(); ++k)
                            {
                                ((hsize_t*)neighbourVals[i].p)[k] = keaFeat->neighbours->at(k);
                            }
                        }
                    }
                                        
                    if(this->numBoolFields > 0)
                    {
                        boolDataOffset[0] = rowOff;
                        boolDataOffset[1] = 0;
                        
                        H5::DataSpace boolWriteDataSpace = boolDataset->getSpace();
                        boolWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolDataDims, boolDataOffset);
                        H5::DataSpace newBoolDataspace = H5::DataSpace(2, boolDataDims);
                        boolDataset->write(boolData, H5::PredType::NATIVE_INT, newBoolDataspace, boolWriteDataSpace);
                        newBoolDataspace.close();
                        boolWriteDataSpace.close();
                    }
                    
                    if(this->numIntFields > 0)
                    {
                        intDataOffset[0] = rowOff;
                        intDataOffset[1] = 0;
                        
                        H5::DataSpace intWriteDataSpace = intDataset->getSpace();
                        intWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intDataDims, intDataOffset);
                        H5::DataSpace newIntDataspace = H5::DataSpace(2, intDataDims);
                        intDataset->write(intData, H5::PredType::NATIVE_INT64, newIntDataspace, intWriteDataSpace);
                        intWriteDataSpace.close();
                        newIntDataspace.close();
                    }
                    
                    if(this->numFloatFields > 0)
                    {
                        floatDataOffset[0] = rowOff;
                        floatDataOffset[1] = 0;
                        
                        H5::DataSpace floatWriteDataSpace = floatDataset->getSpace();
                        floatWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatDataDims, floatDataOffset);
                        H5::DataSpace newFloatDataspace = H5::DataSpace(2, floatDataDims);
                        floatDataset->write(floatData, H5::PredType::NATIVE_DOUBLE, newFloatDataspace, floatWriteDataSpace);
                        floatWriteDataSpace.close();
                        newFloatDataspace.close();
                    }
                    
                    if(this->numStringFields > 0)
                    {
                        stringDataOffset[0] = rowOff;
                        stringDataOffset[1] = 0;
                        
                        H5::DataSpace stringWriteDataSpace = strDataset->getSpace();
                        stringWriteDataSpace.selectHyperslab(H5S_SELECT_SET, stringDataDims, stringDataOffset);
                        H5::DataSpace newStringDataspace = H5::DataSpace(2, stringDataDims);
                        strDataset->write(stringData, *strTypeMem, newStringDataspace, stringWriteDataSpace);
                        stringWriteDataSpace.close();
                        newStringDataspace.close();
                    }
                    
                    neighboursDataOffset[0] = rowOff;
                    
                    H5::DataSpace neighboursWriteDataSpace = neighboursDataset->getSpace();
                    neighboursWriteDataSpace.selectHyperslab(H5S_SELECT_SET, neighboursDataDims, neighboursDataOffset);
                    neighboursDataset->write(neighbourVals, intVarLenMemDT, memNeighboursDataspace, neighboursWriteDataSpace);
                    neighboursWriteDataSpace.close();
                    
                    for(size_t i = 0; i < chunkSize; ++i)
                    {
                        if(neighbourVals[i].length > 0)
                        {
                            neighbourVals[i].length = 0;
                            delete[] ((hsize_t*)neighbourVals[i].p);
                        }
                    }
                }
                memNeighboursDataspace.close();
            }
            
            if(remainRows > 0)
            {
                size_t rowOff = numOfBlocks * chunkSize;
                //size_t strOff = 0;
                
                hsize_t boolDataOffset[2];
                boolDataOffset[0] = rowOff;
                boolDataOffset[1] = 0;
                hsize_t boolDataDims[2];
                boolDataDims[0] = remainRows;
                boolDataDims[1] = this->numBoolFields;
                
                hsize_t intDataOffset[2];
                intDataOffset[0] = rowOff;
                intDataOffset[1] = 0;
                hsize_t intDataDims[2];
                intDataDims[0] = remainRows;
                intDataDims[1] = this->numIntFields;
                
                hsize_t floatDataOffset[2];
                floatDataOffset[0] = rowOff;
                floatDataOffset[1] = 0;
                hsize_t floatDataDims[2];
                floatDataDims[0] = remainRows;
                floatDataDims[1] = this->numFloatFields;
                
                hsize_t stringDataOffset[2];
                stringDataOffset[0] = rowOff;
                stringDataOffset[1] = 0;
                hsize_t stringDataDims[2];
                stringDataDims[0] = remainRows;
                stringDataDims[1] = this->numStringFields;
                
                hsize_t neighboursDataOffset[1];
                neighboursDataOffset[0] = rowOff;
                hsize_t neighboursDataDims[1];
                neighboursDataDims[0] = remainRows;
                H5::DataType intVarLenMemDT = H5::VarLenType(&H5::PredType::NATIVE_HSIZE);
                
                KEAATTFeature *keaFeat = nullptr;
                for(size_t i = 0; i < remainRows; ++i)
                {
                    keaFeat = attRows->at(rowOff+i);
                    for(size_t j = 0; j < this->numBoolFields; ++j)
                    {
                        boolData[(i*this->numBoolFields)+j] = keaFeat->boolFields->at(j);
                    }
                    for(size_t j = 0; j < this->numIntFields; ++j)
                    {
                        intData[(i*this->numIntFields)+j] = keaFeat->intFields->at(j);
                    }
                    for(size_t j = 0; j < this->numFloatFields; ++j)
                    {
                        floatData[(i*this->numFloatFields)+j] = keaFeat->floatFields->at(j);
                    }
                    for(size_t j = 0; j < this->numStringFields; ++j)
                    {
                        stringData[(i*this->numStringFields)+j].str = const_cast<char*>(keaFeat->strFields->at(j).c_str());
                    }
                    neighbourVals[i].length = 0;
                    neighbourVals[i].p = nullptr;
                    if(keaFeat->neighbours->size() > 0)
                    {
                        neighbourVals[i].length = keaFeat->neighbours->size();
                        neighbourVals[i].p = new hsize_t[keaFeat->neighbours->size()];
                        for(unsigned int k = 0; k < keaFeat->neighbours->size(); ++k)
                        {
                            ((hsize_t*)neighbourVals[i].p)[k] = keaFeat->neighbours->at(k);
                        }
                    }
                }
                                
                if(this->numBoolFields > 0)
                {
                    H5::DataSpace boolWriteDataSpace = boolDataset->getSpace();
                    boolWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolDataDims, boolDataOffset);
                    H5::DataSpace newBoolDataspace = H5::DataSpace(2, boolDataDims);
                    boolDataset->write(boolData, H5::PredType::NATIVE_INT, newBoolDataspace, boolWriteDataSpace);
                    boolWriteDataSpace.close();
                    newBoolDataspace.close();
                }
                
                if(this->numIntFields > 0)
                {
                    H5::DataSpace intWriteDataSpace = intDataset->getSpace();
                    intWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intDataDims, intDataOffset);
                    H5::DataSpace newIntDataspace = H5::DataSpace(2, intDataDims);
                    intDataset->write(intData, H5::PredType::NATIVE_INT64, newIntDataspace, intWriteDataSpace);
                    newIntDataspace.close();
                    intWriteDataSpace.close();
                }
                
                if(this->numFloatFields > 0)
                {
                    H5::DataSpace floatWriteDataSpace = floatDataset->getSpace();
                    floatWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatDataDims, floatDataOffset);
                    H5::DataSpace newFloatDataspace = H5::DataSpace(2, floatDataDims);
                    floatDataset->write(floatData, H5::PredType::NATIVE_DOUBLE, newFloatDataspace, floatWriteDataSpace);
                    newFloatDataspace.close();
                    floatWriteDataSpace.close();
                }
                
                if(this->numStringFields > 0)
                {
                    H5::DataSpace stringWriteDataSpace = strDataset->getSpace();
                    stringWriteDataSpace.selectHyperslab(H5S_SELECT_SET, stringDataDims, stringDataOffset);
                    H5::DataSpace newStringDataspace = H5::DataSpace(2, stringDataDims);
                    strDataset->write(stringData, *strTypeMem, newStringDataspace, stringWriteDataSpace);
                    newStringDataspace.close();
                    stringWriteDataSpace.close();
                }
                                
                H5::DataSpace neighboursWriteDataSpace = neighboursDataset->getSpace();
                neighboursWriteDataSpace.selectHyperslab(H5S_SELECT_SET, neighboursDataDims, neighboursDataOffset);
                H5::DataSpace newNeighboursDataspace = H5::DataSpace(1, neighboursDataDims);
                neighboursDataset->write(neighbourVals, intVarLenMemDT, newNeighboursDataspace, neighboursWriteDataSpace);
                newNeighboursDataspace.close();
                neighboursWriteDataSpace.close();
                
                for(size_t i = 0; i < remainRows; ++i)
                {
                    if(neighbourVals[i].length > 0)
                    {
                        neighbourVals[i].length = 0;
                        delete[] ((size_t*)neighbourVals[i].p);
                    }
                }
            }
            
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
            
            // WRITE THE ATT SIZE USED TO THE FILE.
            hsize_t sizeDataOffset[1];
            sizeDataOffset[0] = 0;
            hsize_t sizeDataDims[1];
            sizeDataDims[0] = 5;
            
            H5::DataSet sizeDataset = keaImg->openDataSet(bandPathBase + KEA_ATT_SIZE_HEADER);
            H5::DataSpace sizeWriteDataSpace = sizeDataset.getSpace();
            sizeWriteDataSpace.selectHyperslab(H5S_SELECT_SET, sizeDataDims, sizeDataOffset);
            H5::DataSpace newSizeDataspace = H5::DataSpace(1, sizeDataDims);
            
            attSize[0] = this->attRows->size();
            attSize[1] = this->numBoolFields;
            attSize[2] = this->numIntFields;
            attSize[3] = this->numFloatFields;
            attSize[4] = this->numStringFields;
            
            sizeDataset.write(attSize, H5::PredType::NATIVE_HSIZE, newSizeDataspace, sizeWriteDataSpace);
            sizeDataset.close();
            sizeWriteDataSpace.close();
            newSizeDataspace.close();
            
            if(this->numBoolFields > 0)
            {
                delete[] boolData;
                boolDataset->close();
                delete boolDataset;
            }
            if(this->numIntFields > 0)
            {
                delete[] intData;
                intDataset->close();
                delete intDataset;
            }
            if(this->numFloatFields > 0)
            {
                delete[] floatData;
                floatDataset->close();
                delete floatDataset;
            }
            if(this->numStringFields > 0)
            {
                delete[] stringData;
                strDataset->close();
                delete strDataset;
            }
            delete[] neighbourVals;
            delete[] attSize;
            
            delete strTypeMem;
            delete strTypeDisk;
            
            neighboursDataset->close();
            delete neighboursDataset;
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
    
    KEAAttributeTable* KEAAttributeTableInMem::createKeaAtt(H5::H5File *keaImg, const std::shared_ptr<kealib::kea_mutex>& mutex, unsigned int band)
    {
        // Create instance of class to populate and return.
        KEAAttributeTableInMem *att = new KEAAttributeTableInMem(mutex);
        
        std::string bandPathBase = KEA_DATASETNAME_BAND + uint2Str(band);
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
            catch (const H5::Exception &e) 
            {
                throw KEAIOException("The attribute table size field is not present.");
            }
            
            if(attSize[0] > 0)
            {
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
                catch(const H5::Exception &e) 
                {
                    throw KEAIOException("The attribute table size field is not present.");
                }                
                
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
                    catch( const H5::Exception &e )
                    {
                        throw KEAIOException(e.getDetailMsg());
                    }
                }
                
                // Reserve space in vector.
                att->attRows->reserve(attSize[0]);
                
                size_t numOfBlocks = 0;
                numOfBlocks = floor(((double)attSize[0]/chunkSize));
                size_t remainRows = attSize[0] - (numOfBlocks * chunkSize);
                
                KEAATTFeature *feat = nullptr;
                size_t cFid = 0;
                size_t rowOff = 0;
                
                H5::DataSet boolDataset;
                H5::DataSpace boolDataspace;
                H5::DataSpace boolFieldsMemspace;
                int *boolVals = nullptr;
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
                int64_t *intVals = nullptr;
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
                double *floatVals = nullptr;
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
                KEAString *stringVals = nullptr;
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
                
                /* Neighbours */
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
            }
            
            delete[] attSize;
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
    
    KEAAttributeTableInMem::~KEAAttributeTableInMem()
    {
        for(auto iterFeats = attRows->begin(); iterFeats != attRows->end(); ++iterFeats)
        {
            this->deleteKeaFeature(*iterFeats);
        }
        delete attRows;
    }
    
}
