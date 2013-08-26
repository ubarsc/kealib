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

namespace kealib{
    
    KEAAttributeTable::KEAAttributeTable(KEAATTType keaAttType)
    {
        attType = keaAttType;
        numBoolFields = 0;
        numIntFields = 0;
        numFloatFields = 0;
        numStringFields = 0;
        numOfCols = 0;
        
        fields = new std::map<std::string, KEAATTField>();
    }
    
    KEAATTType KEAAttributeTable::getKEAATTType() const
    {
        return this->attType;
    }
    
    void KEAAttributeTable::setBoolValue(const std::string &name, bool value) throw(KEAATTException)
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
        catch (KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTable::setIntValue(const std::string &name, int64_t value) throw(KEAATTException)
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
        catch (KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTable::setFloatValue(const std::string &name, double value) throw(KEAATTException)
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
        catch (KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTable::setStringValue(const std::string &name, const std::string &value) throw(KEAATTException)
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
        catch (KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTable::setBoolValue(size_t colIdx, bool value) throw(KEAATTException)
    {
        if(colIdx > numBoolFields)
        {
            std::string message = std::string("Requested boolean column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        throw KEAATTException("Setting all has not be implemented yet as needs an iterator...");
    }
    
    void KEAAttributeTable::setIntValue(size_t colIdx, int64_t value) throw(KEAATTException)
    {
        if(colIdx > numIntFields)
        {
            std::string message = std::string("Requested integer column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        throw KEAATTException("Setting all has not be implemented yet as needs an iterator...");
    }
    
    void KEAAttributeTable::setFloatValue(size_t colIdx, double value) throw(KEAATTException)
    {
        if(colIdx > numFloatFields)
        {
            std::string message = std::string("Requested float column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        throw KEAATTException("Setting all has not be implemented yet as needs an iterator...");
    }
    
    void KEAAttributeTable::setStringValue(size_t colIdx, const std::string &value) throw(KEAATTException)
    {
        if(colIdx > numStringFields)
        {
            std::string message = std::string("Requested string column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        throw KEAATTException("Setting all has not be implemented yet as needs an iterator...");
    }
    
    KEAFieldDataType KEAAttributeTable::getDataFieldType(const std::string &name) const throw(KEAATTException)
    {
        std::map<std::string, KEAATTField>::iterator iterField = fields->find(name);
        if(iterField == fields->end())
        {
            std::string message = std::string("Field \'") + name + std::string("\' is not within the attribute table.");
            throw KEAATTException(message);
        }
        
        return (*iterField).second.dataType;
    }
    
    size_t KEAAttributeTable::getFieldIndex(const std::string &name) const throw(KEAATTException)
    {
        std::map<std::string, KEAATTField>::iterator iterField = fields->find(name);
        if(iterField == fields->end())
        {
            std::string message = std::string("Field \'") + name + std::string("\' is not within the attribute table.");
            throw KEAATTException(message);
        }
        
        return (*iterField).second.idx;
    }
    
    KEAATTField KEAAttributeTable::getField(const std::string &name) const throw(KEAATTException)
    {
        std::map<std::string, KEAATTField>::iterator iterField = fields->find(name);
        if(iterField == fields->end())
        {
            std::string message = std::string("Field \'") + name + std::string("\' is not within the attribute table.");
            throw KEAATTException(message);
        }
        
        return (*iterField).second;
    }
    
    KEAATTField KEAAttributeTable::getField(size_t globalColIdx) const throw(KEAATTException)
    {
        KEAATTField field;
        bool found = false;
        for(std::map<std::string, KEAATTField>::iterator iterField = fields->begin(); iterField != fields->end(); ++iterField)
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
        for(std::map<std::string, KEAATTField>::iterator iterField = fields->begin(); iterField != fields->end(); ++iterField)
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
    
    size_t KEAAttributeTable::getTotalNumOfCols() const
    {
        return this->numBoolFields + this->numIntFields + this->numFloatFields + this->numStringFields;
    }
    
    size_t KEAAttributeTable::getMaxGlobalColIdx() const
    {
        return numOfCols;
    }
    
    void KEAAttributeTable::addAttBoolField(const std::string &name, bool val, std::string usage) throw(KEAATTException)
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
        catch (KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTable::addAttIntField(const std::string &name, int64_t val, std::string usage) throw(KEAATTException)
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
        catch (KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTable::addAttFloatField(const std::string &name, double val, std::string usage) throw(KEAATTException)
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
        catch (KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTable::addAttStringField(const std::string &name, const std::string &val, std::string usage) throw(KEAATTException)
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
        catch (KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTable::addFields(std::vector<KEAATTField*> *inFields) throw(KEAATTException)
    {
        try 
        {
            KEAATTField field;
            for(std::vector<KEAATTField*>::iterator iterFields = inFields->begin(); iterFields != inFields->end(); ++iterFields)
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
        catch (KEAATTException &e)
        {
            throw e;
        }
    }
    
    void KEAAttributeTable::addFields(std::vector<KEAATTField> inFields) throw(KEAATTException)
    {
        try 
        {
            for(std::vector<KEAATTField>::iterator iterFields = inFields.begin(); iterFields != inFields.end(); ++iterFields)
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
        catch (KEAATTException &e)
        {
            throw e;
        }
    }
    
    KEAATTFeature* KEAAttributeTable::createKeaFeature() throw(KEAATTException)
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
    
    void KEAAttributeTable::deleteKeaFeature(KEAATTFeature *feat) throw(KEAATTException)
    {
        delete feat->boolFields;
        delete feat->intFields;
        delete feat->floatFields;
        delete feat->strFields;
        delete feat->neighbours;
        delete feat;
    }
    
    void KEAAttributeTable::exportToASCII(const std::string &outputFile)throw(KEAATTException, KEAIOException)
    {
        
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
        for(std::map<std::string, KEAATTField>::iterator iterField = fields->begin(); iterField != fields->end(); ++iterField)
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
    
    H5::CompType* KEAAttributeTable::createAttibuteIdxCompTypeDisk() throw(KEAATTException)
    {
        try
        {
            H5::StrType strTypeDisk(0, H5T_VARIABLE);
            
            H5::CompType *attIdxDataType = new H5::CompType( sizeof(KEAAttributeIdx) );
            attIdxDataType->insertMember(KEA_ATT_NAME_FIELD, HOFFSET(KEAAttributeIdx, name), strTypeDisk);
            attIdxDataType->insertMember(KEA_ATT_INDEX_FIELD, HOFFSET(KEAAttributeIdx, idx), H5::PredType::STD_U32LE);
            attIdxDataType->insertMember(KEA_ATT_USAGE_FIELD, HOFFSET(KEAAttributeIdx, usage), strTypeDisk);
            attIdxDataType->insertMember(KEA_ATT_COLNUM_FIELD, HOFFSET(KEAAttributeIdx, colNum), H5::PredType::STD_U32LE);
            return attIdxDataType;
        }
        catch( H5::FileIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
    }
    
    H5::CompType* KEAAttributeTable::createAttibuteIdxCompTypeMem() throw(KEAATTException)
    {
        try
        {
            H5::StrType strTypeMem(0, H5T_VARIABLE);
            
            H5::CompType *attIdxDataType = new H5::CompType( sizeof(KEAAttributeIdx) );
            attIdxDataType->insertMember(KEA_ATT_NAME_FIELD, HOFFSET(KEAAttributeIdx, name), strTypeMem);
            attIdxDataType->insertMember(KEA_ATT_INDEX_FIELD, HOFFSET(KEAAttributeIdx, idx), H5::PredType::NATIVE_UINT);
            attIdxDataType->insertMember(KEA_ATT_USAGE_FIELD, HOFFSET(KEAAttributeIdx, usage), strTypeMem);
            attIdxDataType->insertMember(KEA_ATT_COLNUM_FIELD, HOFFSET(KEAAttributeIdx, colNum), H5::PredType::NATIVE_UINT);
            return attIdxDataType;
        }
        catch( H5::FileIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
    }
    
    H5::CompType* KEAAttributeTable::createKeaStringCompTypeDisk() throw(KEAATTException)
    {
        try
        {
            H5::StrType strTypeDisk(0, H5T_VARIABLE);
            
            H5::CompType *keaStrDataType = new H5::CompType( sizeof(KEAString) );
            keaStrDataType->insertMember(KEA_ATT_STRING_FIELD, HOFFSET(KEAString, str), strTypeDisk);
            return keaStrDataType;
        }
        catch( H5::FileIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
    }
    
    H5::CompType* KEAAttributeTable::createKeaStringCompTypeMem() throw(KEAATTException)
    {
        try
        {
            H5::StrType strTypeMem(0, H5T_VARIABLE);
            
            H5::CompType *keaStrDataType = new H5::CompType( sizeof(KEAString) );
            keaStrDataType->insertMember(KEA_ATT_STRING_FIELD, HOFFSET(KEAString, str), strTypeMem);
            return keaStrDataType;
        }
        catch( H5::FileIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataSetIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataSpaceIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
        catch( H5::DataTypeIException &e )
        {
            throw KEAATTException(e.getDetailMsg());
        }
    }
        
    KEAAttributeTable::~KEAAttributeTable()
    {
        delete fields;
    }
    
}
