/*
 *  KEAAttributeTable.cpp
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

#include "libkea/KEAAttributeTable.h"

HIGHFIVE_REGISTER_TYPE(kealib::KEAAttributeIdx, kealib::KEAAttributeTable::createAttibuteIdxCompType)

namespace kealib{
    
    KEAAttributeTable::KEAAttributeTable(KEAATTType keaAttType, const std::shared_ptr<kealib::kea_mutex>& mutex)
        : KEABase(mutex)
    {
        attType = keaAttType;
        numBoolFields = 0;
        numIntFields = 0;
        numFloatFields = 0;
        numStringFields = 0;
        numOfCols = 0;
        numRows = 0;
        
        fields = new std::map<std::string, KEAATTField>();
    }
    
    KEAATTType KEAAttributeTable::getKEAATTType() const
    {
        return this->attType;
    }
    
    bool KEAAttributeTable::getBoolField(size_t fid, const std::string &name) const
    {
        throw KEAATTException("Unimplemented");
    }
    
    int64_t KEAAttributeTable::getIntField(size_t fid, const std::string &name) const
    {
        throw KEAATTException("Unimplemented");
    }
    
    double KEAAttributeTable::getFloatField(size_t fid, const std::string &name) const
    {
        throw KEAATTException("Unimplemented");
    }
    
    std::string KEAAttributeTable::getStringField(size_t fid, const std::string &name) const
    {
        throw KEAATTException("Unimplemented");
    }
    
    struct tm KEAAttributeTable::getDateTimeField(size_t fid, const std::string &name) const
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::setBoolField(size_t fid, const std::string &name, bool value)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::setIntField(size_t fid, const std::string &name, int64_t value)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::setFloatField(size_t fid, const std::string &name, double value)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::setStringField(size_t fid, const std::string &name, const std::string &value)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::setDateTimeField(size_t fid, const std::string &name, const struct tm &value)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::setBoolValue(const std::string &name, bool value)
    {
        try 
        {
            KEAATTField field = this->getField(name);
            if(field.dataType != kea_att_bool)
            {
                std::string message = std::string("Field \'") + name + std::string("\' is not of type boolean.");
                throw KEAATTException(message);
            }
            
            this->setBoolValue(field.idx, value);
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTable::setIntValue(const std::string &name, int64_t value)
    {
        try 
        {
            KEAATTField field = this->getField(name);
            if(field.dataType != kea_att_int)
            {
                std::string message = std::string("Field \'") + name + std::string("\' is not of type integer.");
                throw KEAATTException(message);
            }
            
            this->setIntValue(field.idx, value);
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTable::setFloatValue(const std::string &name, double value)
    {
        try 
        {
            KEAATTField field = this->getField(name);
            if(field.dataType != kea_att_float)
            {
                std::string message = std::string("Field \'") + name + std::string("\' is not of type float.");
                throw KEAATTException(message);
            }
            
            this->setFloatValue(field.idx, value);
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTable::setStringValue(const std::string &name, const std::string &value)
    {
        try 
        {
            KEAATTField field = this->getField(name);
            if(field.dataType != kea_att_bool)
            {
                std::string message = std::string("Field \'") + name + std::string("\' is not of type string.");
                throw KEAATTException(message);
            }
            
            this->setStringValue(field.idx, value);
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTable::setDateTimeValue(const std::string &name, const struct tm &value)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::setBoolValue(size_t colIdx, bool value)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::setIntValue(size_t colIdx, int64_t value)
    {
         throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::setFloatValue(size_t colIdx, double value)
    {
         throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::setStringValue(size_t colIdx, const std::string &value)
    {
        throw KEAATTException("Unimplemented");
    }

    void KEAAttributeTable::setDateTimeValue(size_t colIdx, const struct tm &value)
    {
        throw KEAATTException("Unimplemented");
    }
    
    bool KEAAttributeTable::getBoolField(size_t fid, size_t colIdx) const
    {
        throw KEAATTException("Unimplemented");
    }
    
    int64_t KEAAttributeTable::getIntField(size_t fid, size_t colIdx) const
    {
        throw KEAATTException("Unimplemented");
    }
    
    double KEAAttributeTable::getFloatField(size_t fid, size_t colIdx) const
    {
        throw KEAATTException("Unimplemented");
    }

    std::string KEAAttributeTable::getStringField(size_t fid, size_t colIdx) const
    {
        throw KEAATTException("Unimplemented");
    }
    
    struct tm KEAAttributeTable::getDateTimeField(size_t fid, size_t colIdx) const
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::getBoolFields(size_t startfid, size_t len, size_t colIdx, bool *pbBuffer) const
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::getIntFields(size_t startfid, size_t len, size_t colIdx, int64_t *pnBuffer) const
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::getFloatFields(size_t startfid, size_t len, size_t colIdx, double *pfBuffer) const
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::getStringFields(size_t startfid, size_t len, size_t colIdx, std::vector<std::string> *psBuffer) const
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::getDateTimeFields(size_t startfid, size_t len, size_t colIdx, struct tm *pBuffer) const
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::getNeighbours(size_t startfid, size_t len, std::vector<std::vector<size_t>* > *neighbours) const
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::setBoolField(size_t fid, size_t colIdx, bool value)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::setIntField(size_t fid, size_t colIdx, int64_t value)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::setFloatField(size_t fid, size_t colIdx, double value)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::setStringField(size_t fid, size_t colIdx, const std::string &value)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::setDateTimeField(size_t fid, size_t colIdx, const struct tm &value)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::setBoolFields(size_t startfid, size_t len, size_t colIdx, bool *pbBuffer)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::setIntFields(size_t startfid, size_t len, size_t colIdx, int64_t *pnBuffer)
    {
        throw KEAATTException("Unimplemented");
    }

    void KEAAttributeTable::setFloatFields(size_t startfid, size_t len, size_t colIdx, double *pfBuffer)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::setStringFields(size_t startfid, size_t len, size_t colIdx, std::vector<std::string> *papszStrList)
    {
        throw KEAATTException("Unimplemented");
    }

    void KEAAttributeTable::setDateTimeFields(size_t startfid, size_t len, size_t colIdx, struct tm *pBuffer)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::setNeighbours(size_t startfid, size_t len, std::vector<std::vector<size_t>* > *neighbours)
    {
        throw KEAATTException("Unimplemented");
    }
    
    KEAATTFeature* KEAAttributeTable::getFeature(size_t fid) const
    {
        throw KEAATTException("Unimplemented");
    }
    
    KEAFieldDataType KEAAttributeTable::getDataFieldType(const std::string &name) const
    {
        auto iterField = fields->find(name);
        if(iterField == fields->end())
        {
            std::string message = std::string("Field \'") + name + std::string("\' is not within the attribute table.");
            throw KEAATTException(message);
        }
        
        return (*iterField).second.dataType;
    }
    
    size_t KEAAttributeTable::getFieldIndex(const std::string &name) const
    {
        auto iterField = fields->find(name);
        if(iterField == fields->end())
        {
            std::string message = std::string("Field \'") + name + std::string("\' is not within the attribute table.");
            throw KEAATTException(message);
        }
        
        return (*iterField).second.idx;
    }
    
    KEAATTField KEAAttributeTable::getField(const std::string &name) const
    {
        auto iterField = fields->find(name);
        if(iterField == fields->end())
        {
            std::string message = std::string("Field \'") + name + std::string("\' is not within the attribute table.");
            throw KEAATTException(message);
        }
        
        return (*iterField).second;
    }
    
    KEAATTField KEAAttributeTable::getField(size_t globalColIdx) const
    {
        KEAATTField field;
        bool found = false;
        for(auto iterField = fields->begin(); iterField != fields->end(); ++iterField)
        {
            if((*iterField).second.colNum == globalColIdx)
            {
                field = (*iterField).second;
                found = true;
                break;
            }
        }
        
        if(!found)
        {
            throw KEAATTException("Global column index could not be found.");
        }
        
        return field;
    }
    
    std::vector<std::string> KEAAttributeTable::getFieldNames() const
    {
        std::vector<std::string> names;
        for(auto iterField = fields->begin(); iterField != fields->end(); ++iterField)
        {
            names.push_back((*iterField).first);
        }
        return names;
    }
    
    bool KEAAttributeTable::hasField(const std::string &name) const
    {
        bool found = false;
        if(fields->count(name) > 0)
        {
            found = true;
        }
        return found;
    }
    
    size_t KEAAttributeTable::getNumBoolFields() const
    {
        return this->numBoolFields;
    }
    
    size_t KEAAttributeTable::getNumIntFields() const
    {
        return this->numIntFields;
    }
    
    size_t KEAAttributeTable::getNumFloatFields() const
    {
        return this->numFloatFields;
    }
    
    size_t KEAAttributeTable::getNumStringFields() const
    {
        return this->numStringFields;
    }
    
    size_t KEAAttributeTable::getNumDateTimeFields() const
    {
        throw KEAATTException("Unimplemented");
    }
    
    size_t KEAAttributeTable::getSize() const
    {
        return numRows;
    }
    
    size_t KEAAttributeTable::getTotalNumOfCols() const
    {
        return this->numBoolFields + this->numIntFields + this->numFloatFields + this->numStringFields;
    }
    
    size_t KEAAttributeTable::getMaxGlobalColIdx() const
    {
        return numOfCols;
    }
    
    std::string KEAAttributeTable::getBandPathBase() const
    {
        return bandPathBase;
    }
    
    size_t KEAAttributeTable::getChunkSize() const
    {
        return chunkSize;
    }
    
    void KEAAttributeTable::addRows(size_t numRows)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::addAttBoolField(const std::string &name, bool val, std::string usage)
    {
        try 
        {
            // Check whether the name already exists. Names need to be unique.
            if(fields->count(name) > 0)
            {
                std::string message = std::string("Field \'") + name + std::string("\' is already within the attribute table.");
                throw KEAATTException(message);
            }
            
            // Create Field
            KEAATTField nField;
            nField.name = name;
            nField.dataType = kea_att_bool;
            nField.idx = numBoolFields;
            nField.usage = usage;
            nField.colNum = numOfCols;
            
            // Get the implementation to created the new field
            this->addAttBoolField(nField, val);
            
            // If no expection thrown added to header.
            fields->insert(std::pair<std::string, KEAATTField>(name, nField));
            ++numBoolFields;
            ++numOfCols;
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTable::addAttIntField(const std::string &name, int64_t val, std::string usage)
    {
        try 
        {
            // Check whether the name already exists. Names need to be unique.
            if(fields->count(name) > 0)
            {
                std::string message = std::string("Field \'") + name + std::string("\' is already within the attribute table.");
                throw KEAATTException(message);
            }
            
            // Create Field
            KEAATTField nField;
            nField.name = name;
            nField.dataType = kea_att_int;
            nField.idx = numIntFields;
            nField.usage = usage;
            nField.colNum = numOfCols;
            
            // Get the implementation to created the new field
            this->addAttIntField(nField, val);
            
            // If no expection thrown added to header.
            fields->insert(std::pair<std::string, KEAATTField>(name, nField));
            ++numIntFields;
            ++numOfCols;
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTable::addAttFloatField(const std::string &name, double val, std::string usage)
    {
        try 
        {
            // Check whether the name already exists. Names need to be unique.
            if(fields->count(name) > 0)
            {
                std::string message = std::string("Field \'") + name + std::string("\' is already within the attribute table.");
                throw KEAATTException(message);
            }
            
            // Create Field
            KEAATTField nField;
            nField.name = name;
            nField.dataType = kea_att_float;
            nField.idx = numFloatFields;
            nField.usage = usage;
            nField.colNum = numOfCols;
            
            // Get the implementation to created the new field
            this->addAttFloatField(nField, val);
            
            // If no expection thrown added to header.
            fields->insert(std::pair<std::string, KEAATTField>(name, nField));
            ++numFloatFields;
            ++numOfCols;
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTable::addAttStringField(const std::string &name, const std::string &val, std::string usage)
    {
        try 
        {
            // Check whether the name already exists. Names need to be unique.
            if(fields->count(name) > 0)
            {
                std::string message = std::string("Field \'") + name + std::string("\' is already within the attribute table.");
                throw KEAATTException(message);
            }
            
            // Create Field
            KEAATTField nField;
            nField.name = name;
            nField.dataType = kea_att_string;
            nField.idx = numStringFields;
            nField.usage = usage;
            nField.colNum = numOfCols;
            
            // Get the implementation to created the new field
            this->addAttStringField(nField, val);
            
            // If no expection thrown added to header.
            fields->insert(std::pair<std::string, KEAATTField>(name, nField));
            ++numStringFields;
            ++numOfCols;
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTable::addAttDateTimeField(const std::string &name, const struct tm &val, std::string usage)
    {
        throw KEAATTException("Unimplemented");
    }
    
    // not needed?
    void KEAAttributeTable::addFields(std::vector<KEAATTField*> *inFields)
    {
        try 
        {
            KEAATTField field;
            for(auto iterFields = inFields->begin(); iterFields != inFields->end(); ++iterFields)
            {
                if((*iterFields)->dataType == kea_att_bool)
                {
                    this->addAttBoolField((*iterFields)->name, false, (*iterFields)->usage);
                }
                else if((*iterFields)->dataType == kea_att_int)
                {
                    this->addAttIntField((*iterFields)->name, 0, (*iterFields)->usage);
                }
                else if((*iterFields)->dataType == kea_att_float)
                {
                    this->addAttFloatField((*iterFields)->name, 0.0, (*iterFields)->usage);
                }
                else if((*iterFields)->dataType == kea_att_string)
                {
                    this->addAttStringField((*iterFields)->name, "", (*iterFields)->usage);
                }
                else
                {
                    throw KEAATTException("Data type was not recognised.");
                }
                
                field = this->getField((*iterFields)->name);
                (*iterFields)->idx = field.idx;
                (*iterFields)->colNum = field.colNum;
            }
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
    }
    
    // not needed?
    void KEAAttributeTable::addFields(std::vector<KEAATTField> inFields)
    {
        try 
        {
            for(auto iterFields = inFields.begin(); iterFields != inFields.end(); ++iterFields)
            {
                if((*iterFields).dataType == kea_att_bool)
                {
                    this->addAttBoolField((*iterFields).name, false, (*iterFields).usage);
                }
                else if((*iterFields).dataType == kea_att_int)
                {
                    this->addAttIntField((*iterFields).name, 0, (*iterFields).usage);
                }
                else if((*iterFields).dataType == kea_att_float)
                {
                    this->addAttFloatField((*iterFields).name, 0.0, (*iterFields).usage);
                }
                else if((*iterFields).dataType == kea_att_string)
                {
                    this->addAttStringField((*iterFields).name, "", (*iterFields).usage);
                }
                else
                {
                    throw KEAATTException("Data type was not recognised.");
                }
            }
        }
        catch (const KEAATTException &e)
        {
            throw e;
        }
    }
    
    // move to mem?
    KEAATTFeature* KEAAttributeTable::createKeaFeature()
    {
        KEAATTFeature *feat = new KEAATTFeature();
        feat->boolFields = new std::vector<bool>();
        for(size_t i = 0; i < this->numBoolFields; ++i)
        {
            feat->boolFields->push_back(false);
        }
        feat->intFields = new std::vector<int64_t>();
        for(size_t i = 0; i < this->numIntFields; ++i)
        {
            feat->intFields->push_back(0);
        }
        feat->floatFields = new std::vector<double>();
        for(size_t i = 0; i < this->numFloatFields; ++i)
        {
            feat->floatFields->push_back(0.0);
        }
        feat->strFields = new std::vector<std::string>();
        for(size_t i = 0; i < this->numStringFields; ++i)
        {
            feat->strFields->push_back("");
        }
        feat->neighbours = new std::vector<size_t>();
        feat->fid = 0;
        
        return feat;
    }
    
    void KEAAttributeTable::deleteKeaFeature(KEAATTFeature *feat)
    {
        delete feat->boolFields;
        delete feat->intFields;
        delete feat->floatFields;
        delete feat->strFields;
        delete feat->neighbours;
        delete feat;
    }
    
    void KEAAttributeTable::copyRAT(const KEAAttributeTable *pFrom, KEAAttributeTable *pTo)
    {
        try
        {
            // add the fields
            for(auto iterField = pFrom->fields->begin(); iterField != pFrom->fields->end(); ++iterField)
            {
                // TODO: haven't worried about copying fill value over
                if((*iterField).second.dataType == kea_att_bool)
                {
                    pTo->addAttBoolField((*iterField).second.name, false, (*iterField).second.usage);
                }
                else if((*iterField).second.dataType == kea_att_int)
                {
                    pTo->addAttIntField((*iterField).second.name, 0, (*iterField).second.usage);
                }
                else if((*iterField).second.dataType == kea_att_float)
                {
                    pTo->addAttFloatField((*iterField).second.name, 0.0, (*iterField).second.usage);
                }
                else if((*iterField).second.dataType == kea_att_string)
                {
                    pTo->addAttStringField((*iterField).second.name, "", (*iterField).second.usage);
                }
            }
            
            pTo->addRows(pFrom->getSize());
            
            // copy the data
            size_t numOfBlocks = std::floor(((double)pTo->numRows / pTo->chunkSize));
            size_t remainRows = pTo->numRows - (numOfBlocks * pTo->chunkSize);
            bool *boolVals = nullptr;
            if(pFrom->getNumBoolFields() > 0)
            {
                boolVals = new bool[pTo->chunkSize];
            }
            int64_t *intVals = nullptr;
            if(pFrom->getNumIntFields() > 0 )
            {
                intVals = new int64_t[pTo->chunkSize];
            }
            double *floatVals = nullptr;
            if(pFrom->getNumFloatFields() > 0)
            {
                floatVals = new double[pTo->chunkSize];
            }
            std::vector<std::string> stringVals;
            
            if(numOfBlocks > 0)
            {
                for(size_t n = 0; n < numOfBlocks; ++n)
                {
                    size_t rowOff = n * pTo->chunkSize;
                    for( size_t f = 0; f < pFrom->getNumBoolFields(); f++ )
                    {
                        pFrom->getBoolFields(rowOff, pTo->chunkSize, f, boolVals);
                        pTo->setBoolFields(rowOff, pTo->chunkSize, f, boolVals);
                    }
                    for( size_t f = 0; f < pFrom->getNumIntFields(); f++ )
                    {
                        pFrom->getIntFields(rowOff, pTo->chunkSize, f, intVals);
                        pTo->setIntFields(rowOff, pTo->chunkSize, f, intVals);
                    }
                    for( size_t f = 0; f < pFrom->getNumFloatFields(); f++ )
                    {
                        pFrom->getFloatFields(rowOff, pTo->chunkSize, f, floatVals);
                        pTo->setFloatFields(rowOff, pTo->chunkSize, f, floatVals);
                    }
                    for( size_t f = 0; f < pFrom->getNumStringFields(); f++ )
                    {
                        pFrom->getStringFields(rowOff, pTo->chunkSize, f, &stringVals);
                        pTo->setStringFields(rowOff, pTo->chunkSize, f, &stringVals);
                    }
                }
            }            
            if(remainRows > 0)
            {
                // use same buffers as above
                size_t rowOff = numOfBlocks * pTo->chunkSize;
                for( size_t f = 0; f < pFrom->getNumBoolFields(); f++ )
                {
                    pFrom->getBoolFields(rowOff, remainRows, f, boolVals);
                    pTo->setBoolFields(rowOff, remainRows, f, boolVals);
                }
                for( size_t f = 0; f < pFrom->getNumIntFields(); f++ )
                {
                    pFrom->getIntFields(rowOff, remainRows, f, intVals);
                    pTo->setIntFields(rowOff, remainRows, f, intVals);
                }
                for( size_t f = 0; f < pFrom->getNumFloatFields(); f++ )
                {
                    pFrom->getFloatFields(rowOff, remainRows, f, floatVals);
                    pTo->setFloatFields(rowOff, remainRows, f, floatVals);
                }
                for( size_t f = 0; f < pFrom->getNumStringFields(); f++ )
                {
                    pFrom->getStringFields(rowOff, remainRows, f, &stringVals);
                    pTo->setStringFields(rowOff, remainRows, f, &stringVals);
                }
            }
            delete[] boolVals;
            delete[] intVals;
            delete[] floatVals;
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

    
    void KEAAttributeTable::printAttributeTableHeaderInfo()
    {
        if(this->attType == kea_att_mem)
        {
            std::cout << "Using an in memory attribute table\n";
        }
        else if(this->attType == kea_att_file)
        {
            std::cout << "Using an file attribute table\n";
        }
        else
        {
            std::cout << "This is an unknown implementation\n";
        }
        std::cout << "Number of Boolean Fields: " << this->numBoolFields << std::endl;
        std::cout << "Number of Integer Fields: " << this->numIntFields << std::endl;
        std::cout << "Number of Float Fields: " << this->numFloatFields << std::endl;
        std::cout << "Number of String Fields: " << this->numStringFields << std::endl;
        
        std::cout << "Fields:\n";
        for(auto iterField = fields->begin(); iterField != fields->end(); ++iterField)
        {
            std::cout << "Field: " << (*iterField).second.name << " DATA TYPE:";
            if((*iterField).second.dataType == kea_att_bool)
            {
                std::cout << " boolean ";
            }
            else if((*iterField).second.dataType == kea_att_int)
            {
                std::cout << " integer ";
            }
            else if((*iterField).second.dataType == kea_att_float)
            {
                std::cout << " float ";
            }
            else if((*iterField).second.dataType == kea_att_string)
            {
                std::cout << " string ";
            }
            else
            {
                std::cout << " UNKNOWN!! ";
            }
            
            std::cout << " USAGE: \'" << (*iterField).second.usage << "\' Global Index: " << (*iterField).second.colNum << std::endl;
        }
        
        std::cout << "Max. Column Index: " << this->numOfCols << std::endl;
    }

    HighFive::CompoundType KEAAttributeTable::createAttibuteIdxCompType()
    {
        try
        {
            // note HOFFSET - layout of struct is not packed so we need to tell HighFive the actual offsets or it will
            // calculate different...
            std::vector<HighFive::CompoundType::member_def> members;
            members.push_back(HighFive::CompoundType::member_def(KEA_ATT_NAME_FIELD, HighFive::VariableLengthStringType(), HOFFSET(KEAAttributeIdx, name)));
            members.push_back(HighFive::CompoundType::member_def(KEA_ATT_INDEX_FIELD, HighFive::AtomicType<uint32_t>(), HOFFSET(KEAAttributeIdx, idx)));
            members.push_back(HighFive::CompoundType::member_def(KEA_ATT_USAGE_FIELD, HighFive::VariableLengthStringType(), HOFFSET(KEAAttributeIdx, usage)));
            members.push_back(HighFive::CompoundType::member_def(KEA_ATT_COLNUM_FIELD, HighFive::AtomicType<uint32_t>(), HOFFSET(KEAAttributeIdx, colNum)));
            return HighFive::CompoundType(members, sizeof(KEAAttributeIdx));
        }
        catch( const HighFive::Exception &e)
        {
            throw kealib::KEAIOException(e.what());
        }
    }

    HighFive::CompoundType KEAAttributeTable::createKeaStringCompType()
    {
        try
        {
            std::vector<HighFive::CompoundType::member_def> members;
            members.push_back(HighFive::CompoundType::member_def(KEA_ATT_STRING_FIELD, HighFive::VariableLengthStringType(), HOFFSET(KEAAttString, str)));
            return HighFive::CompoundType(members);
        }
        catch( const HighFive::Exception &e)
        {
            throw kealib::KEAIOException(e.what());
        }
    }

    KEAAttributeTable* KEAAttributeTable::createKeaAtt(HighFive::File *keaImg, const std::shared_ptr<kealib::kea_mutex>& mutex, unsigned int band, unsigned int chunkSizeIn)
    {
        // Create instance of class to populate and return.
        std::string bandPathBase = KEA_DATASETNAME_BAND + uint2Str(band);
        // no lock needed - should be done by caller
        KEAAttributeTable *pAtt = nullptr;
        
        try
        {
            // Read header size.
            std::vector<uint64_t> attSize;
            try
            {
                auto datasetAttSize = keaImg->getDataSet( bandPathBase + KEA_ATT_SIZE_HEADER );
                datasetAttSize.read(attSize);
            }
            catch (const HighFive::Exception &e)
            {
                throw KEAIOException("The attribute table size field is not present.");
            }
            
            // READ ATTRIBUTE TABLE FROM KEA IMAGE BAND...
            
            // READ THE CHUNK SIZE - default to given
            size_t chunkSize = chunkSizeIn;
            try
            {
                uint32_t hChunkSize = 0;
                auto datasetAttSize = keaImg->getDataSet( bandPathBase + KEA_ATT_CHUNKSIZE_HEADER );
                datasetAttSize.read(hChunkSize);
                if(hChunkSize > 0)
                {
                    chunkSize = hChunkSize;
                }
            }
            catch(const HighFive::Exception &e)
            {
                throw KEAIOException("The attribute table size field is not present.");
            }
            
            pAtt = new KEAAttributeTable(KEAATTType::kea_att_undefined, mutex);
            pAtt->numRows = attSize[0];
            pAtt->bandPathBase = bandPathBase;
            pAtt->chunkSize = chunkSize;
            
            // READ TABLE HEADERS
            auto fieldCompTypeMem = KEAAttributeTable::createAttibuteIdxCompType();
            
            bool firstColNum = true;
            
            // READ BOOLEAN HEADERS
            pAtt->numBoolFields = attSize[1];
            if(pAtt->numBoolFields > 0)
            {
                try
                {
                    auto boolFieldsDataset = keaImg->getDataSet( bandPathBase + KEA_ATT_BOOL_FIELDS_HEADER );
                    
                    KEAAttributeIdx *inFields = new KEAAttributeIdx[pAtt->numBoolFields];
                    boolFieldsDataset.read_raw(inFields);
                    
                    KEAATTField field;
                    for(unsigned int i = 0; i < pAtt->numBoolFields; ++i)
                    {
                        field = KEAATTField();
                        field.name = std::string(inFields[i].name);
                        free(inFields[i].name);
                        field.dataType = kea_att_bool;
                        field.idx = inFields[i].idx;
                        field.usage = std::string(inFields[i].usage);
                        free(inFields[i].usage);
                        field.colNum = inFields[i].colNum;
                        
                        if(firstColNum)
                        {
                            // TODO: can this be made clearer?
                            if(field.colNum == 0)
                            {
                                pAtt->numOfCols = 1;
                            }
                            else
                            {
                                pAtt->numOfCols = field.colNum + 1;
                            }
                            firstColNum = false;
                        }
                        else if(field.colNum >= pAtt->numOfCols)
                        {
                            pAtt->numOfCols = field.colNum + 1;
                        }
                        
                        pAtt->fields->insert(std::pair<std::string, KEAATTField>(field.name, field));
                    }
                    
                    delete[] inFields;
                }
                catch( const HighFive::Exception &e )
                {
                    throw KEAIOException(e.what());
                }
            }
            
            // READ INTEGER HEADERS
            pAtt->numIntFields = attSize[2];
            if(pAtt->numIntFields > 0)
            {
                try
                {
                    auto intFieldsDataset = keaImg->getDataSet( bandPathBase + KEA_ATT_INT_FIELDS_HEADER );
                    KEAAttributeIdx *inFields = new KEAAttributeIdx[pAtt->numIntFields];
                    
                    intFieldsDataset.read_raw(inFields);
                    
                    KEAATTField field;
                    for(unsigned int i = 0; i < pAtt->numIntFields; ++i)
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
                            // TODO: can this be made clearer?
                            if(field.colNum == 0)
                            {
                                pAtt->numOfCols = 1;
                            }
                            else
                            {
                                pAtt->numOfCols = field.colNum + 1;
                            }
                            firstColNum = false;
                        }
                        else if(field.colNum >= pAtt->numOfCols)
                        {
                            pAtt->numOfCols = field.colNum + 1;
                        }
                        
                        
                        pAtt->fields->insert(std::pair<std::string, KEAATTField>(field.name, field));
                    }
                    
                    delete[] inFields;
                }
                catch( const HighFive::Exception &e )
                {
                    throw KEAIOException(e.what());
                }
            }
            
            // READ FLOAT HEADERS
            pAtt->numFloatFields = attSize[3];
            if(pAtt->numFloatFields > 0)
            {
                try
                {
                    auto floatFieldsDataset = keaImg->getDataSet( bandPathBase + KEA_ATT_FLOAT_FIELDS_HEADER );
                    KEAAttributeIdx *inFields = new KEAAttributeIdx[pAtt->numFloatFields];
                    floatFieldsDataset.read_raw(inFields);
                    
                    KEAATTField field;
                    for(unsigned int i = 0; i < pAtt->numFloatFields; ++i)
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
                            // TODO: can this be made clearer?
                            if(field.colNum == 0)
                            {
                                pAtt->numOfCols = 1;
                            }
                            else
                            {
                                pAtt->numOfCols = field.colNum + 1;
                            }
                            firstColNum = false;
                        }
                        else if(field.colNum >= pAtt->numOfCols)
                        {
                            pAtt->numOfCols = field.colNum + 1;
                        }
                        
                        pAtt->fields->insert(std::pair<std::string, KEAATTField>(field.name, field));
                    }
                    
                    delete[] inFields;
                }
                catch( const HighFive::Exception &e )
                {
                    throw KEAIOException(e.what());
                }
            }
            
            // READ STRING HEADERS
            pAtt->numStringFields = attSize[4];
            if(pAtt->numStringFields > 0)
            {
                try
                {
                    auto strFieldsDataset = keaImg->getDataSet( bandPathBase + KEA_ATT_STRING_FIELDS_HEADER );
                    KEAAttributeIdx *inFields = new KEAAttributeIdx[pAtt->numStringFields];
                    strFieldsDataset.read_raw(inFields);

                    KEAATTField field;
                    for(unsigned int i = 0; i < pAtt->numStringFields; ++i)
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
                            // TODO: can this be made clearer?
                            if(field.colNum == 0)
                            {
                                pAtt->numOfCols = 1;
                            }
                            else
                            {
                                pAtt->numOfCols = field.colNum + 1;
                            }
                            firstColNum = false;
                        }
                        else if(field.colNum >= pAtt->numOfCols)
                        {
                            pAtt->numOfCols = field.colNum + 1;
                        }
                        
                        
                        pAtt->fields->insert(std::pair<std::string, KEAATTField>(field.name, field));
                    }
                    
                    delete[] inFields;
                }
                catch( const HighFive::Exception &e )
                {
                    throw KEAIOException(e.what());
                }
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
    
    void KEAAttributeTable::addAttBoolField(KEAATTField field, bool val)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::addAttIntField(KEAATTField field, int64_t val)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::addAttFloatField(KEAATTField field, float val)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::addAttStringField(KEAATTField field, const std::string &val)
    {
        throw KEAATTException("Unimplemented");
    }

    // for future versions of KEA
    void KEAAttributeTable::addAttDateTimeField(KEAATTField field, const struct tm &val)
    {
        throw KEAATTException("Unimplemented");
    }
        
    KEAAttributeTable::~KEAAttributeTable()
    {
        delete fields;
    }

    void KEAAttributeTable::destroyAttributeTable(KEAAttributeTable *pTable)
    {
        delete pTable;
    }
    
    // helper for variable length structures - HighFive doesn't currently support
    void KEAAttributeTable::readNeighbours(const HighFive::DataSet &dataset, size_t startfid, size_t len, std::vector<std::vector<size_t>* > *neighbours) const
    {
    
        if(!neighbours->empty())
        {
            for(auto iterNeigh = neighbours->begin(); iterNeigh != neighbours->end(); ++iterNeigh)
            {
                delete *iterNeigh;
            }
            neighbours->clear();
        }
        neighbours->reserve(len);
    
        hid_t mem_vlen_type = H5Tvlen_create(H5T_NATIVE_HSIZE);
        if( mem_vlen_type == H5I_INVALID_HID)
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Tcreate");
        } 

        hid_t dataspace_id = H5Dget_space(dataset.getId());
        if( dataspace_id == H5I_INVALID_HID)
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Tcreate");
        }
        hsize_t neighboursOffset[1];
        neighboursOffset[0] = startfid;
        hsize_t neighboursCount[1];
        neighboursCount[0] = len;
        if( H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, neighboursOffset, nullptr, neighboursCount, nullptr) < 0 )
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Sselect_hyperslab");
        }
        
        hsize_t neighboursDimsRead[1];
        neighboursDimsRead[0] = len;
        hid_t memspace_id = H5Screate_simple(1, neighboursDimsRead, nullptr);
        if( memspace_id == H5I_INVALID_HID)
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Screate_simple");
        }

        hsize_t neighboursOffset_out[1];
        neighboursOffset_out[0] = 0;
        hsize_t neighboursCount_out[1];
        neighboursCount_out[0] = len;

        if( H5Sselect_hyperslab(memspace_id, H5S_SELECT_SET, neighboursOffset_out, nullptr, neighboursCount_out, nullptr) < 0 )
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Sselect_hyperslab");
        }
        
        hvl_t *neighbourVals = (hvl_t *)malloc(len * sizeof(hvl_t));
        if( H5Dread(dataset.getId(), mem_vlen_type, memspace_id, dataspace_id, H5P_DEFAULT, neighbourVals) < 0 )
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Dread");
        }

        for(size_t i = 0; i < len; ++i)
        {
            neighbours->push_back(new std::vector<size_t>());
            if(neighbourVals[i].len > 0)
            {
                neighbours->back()->reserve(neighbourVals[i].len);
                for(hsize_t n = 0; n < neighbourVals[i].len; ++n)
                {
                    neighbours->back()->push_back(((size_t*)neighbourVals[i].p)[n]);
                }
            }
        }
        
        if( H5Treclaim(mem_vlen_type, dataspace_id, H5P_DEFAULT, neighbourVals) < 0 )
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Treclaim");
        }
        free(neighbourVals);
        H5Tclose(mem_vlen_type);
        H5Sclose(dataspace_id);
        H5Sclose(memspace_id);
    }
    
    // helper for variable length structures - HighFive doesn't currently support
    void KEAAttributeTable::writeNeighbours(const HighFive::DataSet &dataset, size_t startfid, size_t len, const std::vector<std::vector<size_t>* > *neighbours)
    {
        hid_t mem_vlen_type = H5Tvlen_create(H5T_NATIVE_HSIZE);
        if( mem_vlen_type == H5I_INVALID_HID)
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Tcreate");
        } 

        hid_t dataspace_id = H5Dget_space(dataset.getId());
        if( dataspace_id == H5I_INVALID_HID)
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Tcreate");
        }
        hsize_t neighboursOffset[1];
        neighboursOffset[0] = startfid;
        hsize_t neighboursCount[1];
        neighboursCount[0] = len;
        if( H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, neighboursOffset, nullptr, neighboursCount, nullptr) < 0 )
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Sselect_hyperslab");
        }

        hsize_t neighboursOffset_in[1];
        neighboursOffset_in[0] = 0;
        hsize_t neighboursCount_in[1];
        neighboursCount_in[0] = len;
        
        hsize_t neighboursDimsRead[1];
        neighboursDimsRead[0] = len;
        hid_t memspace_id = H5Screate_simple(1, neighboursDimsRead, nullptr);
        if( memspace_id == H5I_INVALID_HID)
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Screate_simple");
        }

        if( H5Sselect_hyperslab(memspace_id, H5S_SELECT_SET, neighboursOffset_in, nullptr, neighboursCount_in, nullptr) < 0 )
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Sselect_hyperslab");
        }

        // copy the neighbours into HDF5 structure
        hvl_t *neighbourVals = (hvl_t *)malloc(len * sizeof(hvl_t));
        size_t i = 0;
        for(auto iterClumps = neighbours->begin(); iterClumps != neighbours->end(); ++iterClumps)
        {
            neighbourVals[i].len = 0;
            neighbourVals[i].p = nullptr;
            if((*iterClumps)->size() > 0)
            {
                neighbourVals[i].len = (*iterClumps)->size();
                neighbourVals[i].p = new hsize_t[(*iterClumps)->size()];
                for(unsigned int k = 0; k < (*iterClumps)->size(); ++k)
                {
                    ((hsize_t*)neighbourVals[i].p)[k] = (*iterClumps)->at(k);
                }
            }
            
            ++i;
        }
        
        if( H5Dwrite(dataset.getId(), mem_vlen_type, memspace_id, dataspace_id, H5P_DEFAULT, neighbourVals) < 0 )
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Dwrite");
        }
        
        for(size_t i = 0; i < len; ++i)
        {
            if(neighbourVals[i].len > 0)
            {
                neighbourVals[i].len = 0;
                delete[] ((hsize_t*)neighbourVals[i].p);
            }
        }

        free(neighbourVals);
        H5Tclose(mem_vlen_type);
        H5Sclose(dataspace_id);
        H5Sclose(memspace_id);
    }

    // workaround for HighFive::DataSet(hid_t) constructor being protected
    class KEAHighFiveDataset : public HighFive::DataSet
    {
    public:
        KEAHighFiveDataset(hid_t id) : HighFive::DataSet(id)
        {
            
        }
          
    };
    
    // helper for variable length structures - HighFive doesn't currently support
    HighFive::DataSet KEAAttributeTable::createNeighboursDataset(HighFive::File *keaImg, const std::string &datasetname, unsigned int deflate)
    {
        hsize_t initDimsNeighboursDS[1];
        initDimsNeighboursDS[0] = this->getSize();
        hsize_t maxDimsNeighboursDS[1];
        maxDimsNeighboursDS[0] = H5S_UNLIMITED;
        
        hsize_t dimsNeighboursChunk[1];
        dimsNeighboursChunk[0] = chunkSize;
        
        hid_t mem_vlen_type = H5Tvlen_create(H5T_NATIVE_HSIZE);
        if( mem_vlen_type == H5I_INVALID_HID)
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Tcreate");
        }
        
        hid_t memspace_id = H5Screate_simple(1, initDimsNeighboursDS, maxDimsNeighboursDS);
        if( memspace_id == H5I_INVALID_HID)
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Screate_simple");
        }
        
        hid_t plist_id = H5Pcreate(H5P_DATASET_CREATE);
        if( plist_id == H5I_INVALID_HID )
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Pcreate");
        }
        if( H5Pset_chunk(plist_id, 1, dimsNeighboursChunk) < 0 )
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Pset_chunk");
        }
        if( H5Pset_shuffle(plist_id) < 0 )
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Pset_shuffle");
        }
        if( H5Pset_deflate(plist_id, deflate) < 0)
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Pset_deflate");
        }
        hvl_t neighboursDataFillVal[1];
        neighboursDataFillVal[0].p = nullptr;
        neighboursDataFillVal[0].len = 0;
        if( H5Pset_fill_value(plist_id, mem_vlen_type, &neighboursDataFillVal) < 0)
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Pset_fill_value");
        }
        
        hid_t dataset_id = H5Dcreate1(keaImg->getId(), datasetname.c_str(), mem_vlen_type, memspace_id, plist_id);
        if( dataset_id == H5I_INVALID_HID )
        {
    		H5Eprint(H5E_DEFAULT, stderr);
        	throw KEAIOException("Error in H5Dcreate1");
        }
        
        H5Tclose(mem_vlen_type);
        H5Sclose(memspace_id);
        H5Pclose(plist_id);
        
        return KEAHighFiveDataset(dataset_id);
    }

}
