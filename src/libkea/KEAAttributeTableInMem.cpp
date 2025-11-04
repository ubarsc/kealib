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

HIGHFIVE_REGISTER_TYPE(kealib::KEAAttString, kealib::KEAAttributeTable::createKeaStringCompType)

namespace kealib{
    
    KEAAttributeTableInMem::KEAAttributeTableInMem(KEAAttributeTable *pBaseAtt, const std::shared_ptr<kealib::kea_mutex>& mutex) : KEAAttributeTable(kea_att_mem, mutex)
    {
        numRows = pBaseAtt->getSize();
        numOfCols = pBaseAtt->getMaxGlobalColIdx();
        numBoolFields = pBaseAtt->getNumBoolFields();
        numIntFields = pBaseAtt->getNumIntFields();
        numFloatFields = pBaseAtt->getNumFloatFields();
        numStringFields = pBaseAtt->getNumStringFields();
        bandPathBase = pBaseAtt->getBandPathBase();
        chunkSize = pBaseAtt->getChunkSize();

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
    
    void KEAAttributeTableInMem::exportToKeaFile(HighFive::File *keaImg, unsigned int band, unsigned int chunkSize, unsigned int deflate)
    {
        /*
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
    
    
    KEAAttributeTable* KEAAttributeTableInMem::createKeaAtt(HighFive::File *keaImg, const std::shared_ptr<kealib::kea_mutex>& mutex, unsigned int band)
    {
        // create an instance of the base class with the number of cols etc
        KEAAttributeTable *pBaseAtt = KEAAttributeTable::createKeaAtt(keaImg, mutex, band, 0);
        
        // now create an instance of KEAAttributeTableInMem with the copy constructor
        KEAAttributeTableInMem *pAtt = new KEAAttributeTableInMem(pBaseAtt, mutex);
        delete pBaseAtt;
        
        try
        {
            // Reserve space in vector.
            pAtt->attRows->reserve(pAtt->numRows);
                    
            size_t numOfBlocks = std::floor(((double)pAtt->numRows / pAtt->chunkSize));
            size_t remainRows = pAtt->numRows - (numOfBlocks * pAtt->chunkSize);
                    
            KEAATTFeature *feat = nullptr;
            size_t cFid = 0;
            size_t rowOff = 0;
                                    
            if(numOfBlocks > 0)
            {
            
                uint8_t *boolVals = nullptr;
                if(pAtt->numBoolFields > 0)
                {
                    boolVals = new uint8_t[pAtt->chunkSize * pAtt->numBoolFields];
                }
                int64_t *intVals = nullptr;
                if(pAtt->numIntFields > 0 )
                {
                    intVals = new int64_t[pAtt->chunkSize * pAtt->numIntFields];
                }
                double *floatVals = nullptr;
                if(pAtt->numFloatFields > 0)
                {
                    floatVals = new double[pAtt->chunkSize * pAtt->numFloatFields];
                }
                KEAAttString *stringVals = nullptr;
                if(pAtt->numStringFields > 0)
                {
                    stringVals = new KEAAttString[pAtt->chunkSize * pAtt->numStringFields];
                }
                for(size_t n = 0; n < numOfBlocks; ++n)
                {
                    rowOff = n * pAtt->chunkSize;
                    // Read data.
                    //neighboursOffset[0] = rowOff;
                    //neighboursDataspace.selectHyperslab( H5S_SELECT_SET, neighboursCount, neighboursOffset );
                    //neighboursDataset.read(neighbourVals, intVarLenMemDT, neighboursMemspace, neighboursDataspace);
                    
                    if(pAtt->numBoolFields > 0)
                    {
                    	std::vector<size_t> startOffset = {rowOff, pAtt->numBoolFields};
    			        std::vector<size_t> bufSize = {pAtt->chunkSize, pAtt->numBoolFields};
                        keaImg->getDataSet(pAtt->bandPathBase + KEA_ATT_BOOL_DATA).select(startOffset, bufSize).read_raw(boolVals);
                    }
                    
                    if(pAtt->numIntFields > 0)
                    {
                    	std::vector<size_t> startOffset = {rowOff, pAtt->numIntFields};
    			        std::vector<size_t> bufSize = {pAtt->chunkSize, pAtt->numIntFields};
                        keaImg->getDataSet(pAtt->bandPathBase + KEA_ATT_INT_DATA).select(startOffset, bufSize).read_raw(intVals);
                    }
                    
                    if(pAtt->numFloatFields > 0)
                    {
                    	std::vector<size_t> startOffset = {rowOff, pAtt->numFloatFields};
    			        std::vector<size_t> bufSize = {pAtt->chunkSize, pAtt->numFloatFields};
                        keaImg->getDataSet(pAtt->bandPathBase + KEA_ATT_FLOAT_DATA).select(startOffset, bufSize).read_raw(floatVals);
                    }
                    
                    if(pAtt->numStringFields > 0)
                    {
                    	std::vector<size_t> startOffset = {rowOff, pAtt->numStringFields};
    			        std::vector<size_t> bufSize = {pAtt->chunkSize, pAtt->numStringFields};
                        keaImg->getDataSet(pAtt->bandPathBase + KEA_ATT_STRING_DATA).select(startOffset, bufSize).read_raw(stringVals);
                    }
                    
                    // Write data into KEAATTFeatures
                    for(size_t i = 0; i < pAtt->chunkSize; ++i)
                    {
                        feat = pAtt->createKeaFeature();
                        feat->fid = cFid++;
                        
                        if(pAtt->numBoolFields > 0)
                        {
                            feat->boolFields->reserve(pAtt->numBoolFields);
                            for(hsize_t j = 0; j < pAtt->numBoolFields; ++j)
                            {
                                feat->boolFields->at(j) = boolVals[(i*pAtt->numBoolFields)+j];
                            }
                        }
                        
                        if(pAtt->numIntFields > 0)
                        {
                            feat->intFields->reserve(pAtt->numIntFields);
                            for(hsize_t j = 0; j < pAtt->numIntFields; ++j)
                            {
                                feat->intFields->at(j) = intVals[(i*pAtt->numIntFields)+j];
                            }
                        }
                        
                        if(pAtt->numFloatFields > 0)
                        {
                            feat->floatFields->reserve(pAtt->numFloatFields);
                            for(hsize_t j = 0; j < pAtt->numFloatFields; ++j)
                            {
                                feat->floatFields->at(j) = floatVals[(i*pAtt->numFloatFields)+j];
                            }
                        }
                        
                        if(pAtt->numStringFields > 0)
                        {
                            feat->strFields->reserve(pAtt->numStringFields);
                            for(hsize_t j = 0; j < pAtt->numStringFields; ++j)
                            {
                                feat->strFields->at(j) = std::string(stringVals[(i*pAtt->numStringFields)+j].str);
                                free(stringVals[(i*pAtt->numStringFields)+j].str);
                            }
                        }
                        
                        /*if(neighbourVals[i].length > 0)
                        {
                            feat->neighbours->reserve(neighbourVals[i].length);
                            for(hsize_t n = 0; n < neighbourVals[i].length; ++n)
                            {
                                feat->neighbours->push_back(((size_t*)neighbourVals[i].p)[n]);
                            }
                            delete[] ((size_t*)neighbourVals[i].p);
                        }*/
                                                    
                        pAtt->attRows->push_back(feat);
                    }                        
                }
                delete[] boolVals;
                delete[] intVals;
                delete[] floatVals;
                delete[] stringVals;
            }
                    
            if(remainRows > 0)
            {
                uint8_t *boolVals = nullptr;
                if(pAtt->numBoolFields > 0)
                {
                    boolVals = new uint8_t[remainRows * pAtt->numBoolFields];
                }
                int64_t *intVals = nullptr;
                if(pAtt->numIntFields > 0 )
                {
                    intVals = new int64_t[remainRows * pAtt->numIntFields];
                }
                double *floatVals = nullptr;
                if(pAtt->numFloatFields > 0)
                {
                    floatVals = new double[remainRows * pAtt->numFloatFields];
                }
                KEAAttString *stringVals = nullptr;
                if(pAtt->numStringFields > 0)
                {
                    stringVals = new KEAAttString[remainRows * pAtt->numStringFields];
                }
            
                rowOff = (numOfBlocks*pAtt->chunkSize);
                // Read data.
                /*neighboursOffset[0] = rowOff;
                neighboursCount[0] = remainRows;
                neighboursDataspace.selectHyperslab( H5S_SELECT_SET, neighboursCount, neighboursOffset );
                
                neighboursDimsRead[0] = remainRows;
                neighboursMemspace = H5::DataSpace( 1, neighboursDimsRead );
                neighboursDataset.read(neighbourVals, intVarLenMemDT, neighboursMemspace, neighboursDataspace);*/
                
                if(pAtt->numBoolFields > 0)
                {
    				std::vector<size_t> startOffset = {rowOff, pAtt->numBoolFields};
    				std::vector<size_t> bufSize = {remainRows, pAtt->numBoolFields};
                    keaImg->getDataSet(pAtt->bandPathBase + KEA_ATT_BOOL_DATA).select(startOffset, bufSize).read_raw(boolVals);
                }
                
                if(pAtt->numIntFields > 0)
                {
    				std::vector<size_t> startOffset = {rowOff, pAtt->numIntFields};
    				std::vector<size_t> bufSize = {remainRows, pAtt->numIntFields};
                    keaImg->getDataSet(pAtt->bandPathBase + KEA_ATT_INT_DATA).select(startOffset, bufSize).read_raw(intVals);
                }
                
                if(pAtt->numFloatFields > 0)
                {
    				std::vector<size_t> startOffset = {rowOff, pAtt->numFloatFields};
    				std::vector<size_t> bufSize = {remainRows, pAtt->numFloatFields};
                    keaImg->getDataSet(pAtt->bandPathBase + KEA_ATT_FLOAT_DATA).select(startOffset, bufSize).read_raw(floatVals);
                }
                
                if(pAtt->numStringFields > 0)
                {
    				std::vector<size_t> startOffset = {rowOff, pAtt->numStringFields};
    				std::vector<size_t> bufSize = {remainRows, pAtt->numStringFields};
                    keaImg->getDataSet(pAtt->bandPathBase + KEA_ATT_STRING_DATA).select(startOffset, bufSize).read_raw(stringVals);
                }
                
                // Write data into KEAATTFeatures
                for(size_t i = 0; i < remainRows; ++i)
                {
                    feat = pAtt->createKeaFeature();
                    feat->fid = cFid++;
                    
                    if(pAtt->numBoolFields > 0)
                    {
                        feat->boolFields->reserve(pAtt->numBoolFields);
                        for(hsize_t j = 0; j < pAtt->numBoolFields; ++j)
                        {
                            feat->boolFields->at(j) = boolVals[(i*pAtt->numBoolFields)+j];
                        }
                    }
                    
                    if(pAtt->numIntFields > 0)
                    {
                        feat->intFields->reserve(pAtt->numIntFields);
                        for(hsize_t j = 0; j < pAtt->numIntFields; ++j)
                        {
                            feat->intFields->at(j) = intVals[(i*pAtt->numIntFields)+j];
                        }
                    }
                    
                    if(pAtt->numFloatFields > 0)
                    {
                        feat->floatFields->reserve(pAtt->numFloatFields);
                        for(hsize_t j = 0; j < pAtt->numFloatFields; ++j)
                        {
                            feat->floatFields->at(j) = floatVals[(i*pAtt->numFloatFields)+j];
                        }
                    }
                    
                    if(pAtt->numStringFields > 0)
                    {
                        feat->strFields->reserve(pAtt->numStringFields);
                        for(hsize_t j = 0; j < pAtt->numStringFields; ++j)
                        {
                            feat->strFields->at(j) = std::string(stringVals[(i*pAtt->numStringFields)+j].str);
                            free(stringVals[(i*pAtt->numStringFields)+j].str);
                        }
                    }
                    
                    /*if(neighbourVals[i].length > 0)
                    {
                        feat->neighbours->reserve(neighbourVals[i].length);
                        for(hsize_t n = 0; n < neighbourVals[i].length; ++n)
                        {
                            feat->neighbours->push_back(((size_t*)neighbourVals[i].p)[n]);
                        }
                        delete[] ((size_t*)neighbourVals[i].p);
                    }*/
                    
                    pAtt->attRows->push_back(feat);
                }
                delete[] boolVals;
                delete[] intVals;
                delete[] floatVals;
                delete[] stringVals;
            }    
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
        
        return pAtt;
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
