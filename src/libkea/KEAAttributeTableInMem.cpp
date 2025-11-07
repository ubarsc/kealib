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

            // memory space for reading
            HighFive::DataSet boolDataset; 
            uint8_t *boolVals = nullptr;
            if(pAtt->numBoolFields > 0)
            {
                boolVals = new uint8_t[pAtt->chunkSize * pAtt->numBoolFields];
                boolDataset = keaImg->getDataSet(pAtt->bandPathBase + KEA_ATT_BOOL_DATA);
            }
            HighFive::DataSet intDataset;
            int64_t *intVals = nullptr;
            if(pAtt->numIntFields > 0 )
            {
                intVals = new int64_t[pAtt->chunkSize * pAtt->numIntFields];
                intDataset = keaImg->getDataSet(pAtt->bandPathBase + KEA_ATT_INT_DATA);
            }
            HighFive::DataSet floatDataset;
            double *floatVals = nullptr;
            if(pAtt->numFloatFields > 0)
            {
                floatVals = new double[pAtt->chunkSize * pAtt->numFloatFields];
                floatDataset = keaImg->getDataSet(pAtt->bandPathBase + KEA_ATT_FLOAT_DATA);
            }
            HighFive::DataSet stringDataset;
            KEAAttString *stringVals = nullptr;
            if(pAtt->numStringFields > 0)
            {
                stringVals = new KEAAttString[pAtt->chunkSize * pAtt->numStringFields];
                stringDataset = keaImg->getDataSet(pAtt->bandPathBase + KEA_ATT_STRING_DATA);
            }
                                    
            if(numOfBlocks > 0)
            {
            
                for(size_t n = 0; n < numOfBlocks; ++n)
                {
                    rowOff = n * pAtt->chunkSize;
                    
                    // Read data.
                    //neighboursOffset[0] = rowOff;
                    //neighboursDataspace.selectHyperslab( H5S_SELECT_SET, neighboursCount, neighboursOffset );
                    //neighboursDataset.read(neighbourVals, intVarLenMemDT, neighboursMemspace, neighboursDataspace);
                    
                    if(pAtt->numBoolFields > 0)
                    {
                    	std::vector<size_t> startOffset = {rowOff, 0};
    			        std::vector<size_t> bufSize = {pAtt->chunkSize, pAtt->numBoolFields};
                        boolDataset.select(startOffset, bufSize).read_raw(boolVals);
                    }
                    
                    if(pAtt->numIntFields > 0)
                    {
                    	std::vector<size_t> startOffset = {rowOff, 0};
    			        std::vector<size_t> bufSize = {pAtt->chunkSize, pAtt->numIntFields};
                        intDataset.select(startOffset, bufSize).read_raw(intVals);
                    }
                    
                    if(pAtt->numFloatFields > 0)
                    {
                    	std::vector<size_t> startOffset = {rowOff, 0};
    			        std::vector<size_t> bufSize = {pAtt->chunkSize, pAtt->numFloatFields};
                        floatDataset.select(startOffset, bufSize).read_raw(floatVals);
                    }
                    
                    if(pAtt->numStringFields > 0)
                    {
                    	std::vector<size_t> startOffset = {rowOff, 0};
    			        std::vector<size_t> bufSize = {pAtt->chunkSize, pAtt->numStringFields};
                        stringDataset.select(startOffset, bufSize).read_raw(stringVals);
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
            }
                    
            if(remainRows > 0)
            {
                // use same buffers as above
            
                rowOff = numOfBlocks * pAtt->chunkSize;
                // Read data.
                /*neighboursOffset[0] = rowOff;
                neighboursCount[0] = remainRows;
                neighboursDataspace.selectHyperslab( H5S_SELECT_SET, neighboursCount, neighboursOffset );
                
                neighboursDimsRead[0] = remainRows;
                neighboursMemspace = H5::DataSpace( 1, neighboursDimsRead );
                neighboursDataset.read(neighbourVals, intVarLenMemDT, neighboursMemspace, neighboursDataspace);*/
                
                if(pAtt->numBoolFields > 0)
                {
    				std::vector<size_t> startOffset = {rowOff, 0};
    				std::vector<size_t> bufSize = {remainRows, pAtt->numBoolFields};
                    boolDataset.select(startOffset, bufSize).read_raw(boolVals);
                }
                
                if(pAtt->numIntFields > 0)
                {
    				std::vector<size_t> startOffset = {rowOff, 0};
    				std::vector<size_t> bufSize = {remainRows, pAtt->numIntFields};
                    intDataset.select(startOffset, bufSize).read_raw(intVals);
                }
                
                if(pAtt->numFloatFields > 0)
                {
    				std::vector<size_t> startOffset = {rowOff, 0};
    				std::vector<size_t> bufSize = {remainRows, pAtt->numFloatFields};
                    floatDataset.select(startOffset, bufSize).read_raw(floatVals);
                }
                
                if(pAtt->numStringFields > 0)
                {
    				std::vector<size_t> startOffset = {rowOff, 0};
    				std::vector<size_t> bufSize = {remainRows, pAtt->numStringFields};
                    stringDataset.select(startOffset, bufSize).read_raw(stringVals);
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
            }    

            delete[] boolVals;
            delete[] intVals;
            delete[] floatVals;
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
