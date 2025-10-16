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
HIGHFIVE_REGISTER_TYPE(kealib::KEAAttString, kealib::KEAAttributeTable::createKeaStringCompType)

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
    
    void KEAAttributeTable::exportToKeaFile(HighFive::File *keaImg, unsigned int band, unsigned int chunkSize, unsigned int deflate)
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
    
    void KEAAttributeTable::exportToASCII(const std::string &outputFile)
    {
        throw KEAATTException("Unimplemented");
    }
    
    void KEAAttributeTable::printAttributeTableHeaderInfo()
    {
        if(this->attType == kea_att_mem)
        {
            std::cout << "Using an in memory attribute table\n";
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
            std::vector<HighFive::CompoundType::member_def> members;
            members.push_back(HighFive::CompoundType::member_def(KEA_ATT_NAME_FIELD, HighFive::VariableLengthStringType()));
            members.push_back(HighFive::CompoundType::member_def(KEA_ATT_INDEX_FIELD, HighFive::AtomicType<uint32_t>()));
            members.push_back(HighFive::CompoundType::member_def(KEA_ATT_USAGE_FIELD, HighFive::VariableLengthStringType()));
            members.push_back(HighFive::CompoundType::member_def(KEA_ATT_COLNUM_FIELD, HighFive::AtomicType<uint32_t>()));
            return HighFive::CompoundType(members);
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
            members.push_back(HighFive::CompoundType::member_def(KEA_ATT_STRING_FIELD, HighFive::VariableLengthStringType()));
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
            catch(const H5::Exception &e)
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
        
    KEAAttributeTable::~KEAAttributeTable()
    {
        delete fields;
    }

    void KEAAttributeTable::destroyAttributeTable(KEAAttributeTable *pTable)
    {
        delete pTable;
    }
}
