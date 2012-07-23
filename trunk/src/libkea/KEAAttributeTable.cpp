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

namespace libkea{
    
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
    
    KEAATTType KEAAttributeTable::getKEAATTType()
    {
        return this->attType;
    }
    
    void KEAAttributeTable::setBoolValue(std::string name, bool value) throw(KEAATTException)
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
    
    void KEAAttributeTable::setIntValue(std::string name, long value) throw(KEAATTException)
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
    
    void KEAAttributeTable::setFloatValue(std::string name, double value) throw(KEAATTException)
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
    
    void KEAAttributeTable::setStringValue(std::string name, std::string value) throw(KEAATTException)
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
    
    void KEAAttributeTable::setIntValue(size_t colIdx, long value) throw(KEAATTException)
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
    
    void KEAAttributeTable::setStringValue(size_t colIdx, std::string value) throw(KEAATTException)
    {
        if(colIdx > numStringFields)
        {
            std::string message = std::string("Requested string column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        throw KEAATTException("Setting all has not be implemented yet as needs an iterator...");
    }
    
    KEAFieldDataType KEAAttributeTable::getDataFieldType(std::string name) throw(KEAATTException)
    {
        std::map<std::string, KEAATTField>::iterator iterField = fields->find(name);
        if(iterField == fields->end())
        {
            std::string message = std::string("Field \'") + name + std::string("\' is not within the attribute table.");
            throw KEAATTException(message);
        }
        
        return (*iterField).second.dataType;
    }
    
    size_t KEAAttributeTable::getFieldIndex(std::string name) throw(KEAATTException)
    {
        std::map<std::string, KEAATTField>::iterator iterField = fields->find(name);
        if(iterField == fields->end())
        {
            std::string message = std::string("Field \'") + name + std::string("\' is not within the attribute table.");
            throw KEAATTException(message);
        }
        
        return (*iterField).second.idx;
    }
    
    KEAATTField KEAAttributeTable::getField(std::string name) throw(KEAATTException)
    {
        std::map<std::string, KEAATTField>::iterator iterField = fields->find(name);
        if(iterField == fields->end())
        {
            std::string message = std::string("Field \'") + name + std::string("\' is not within the attribute table.");
            throw KEAATTException(message);
        }
        
        return (*iterField).second;
    }
    
    std::vector<std::string> KEAAttributeTable::getFieldNames()
    {
        std::vector<std::string> names;
        for(std::map<std::string, KEAATTField>::iterator iterField = fields->begin(); iterField != fields->end(); ++iterField)
        {
            names.push_back((*iterField).first);
        }
        return names;
    }
    
    bool KEAAttributeTable::hasField(std::string name)
    {
        bool found = false;
        if(fields->count(name) > 0)
        {
            found = true;
        }
        return found;
    }
    
    size_t KEAAttributeTable::getNumBoolFields()
    {
        return this->numBoolFields;
    }
    
    size_t KEAAttributeTable::getNumIntFields()
    {
        return this->numIntFields;
    }
    
    size_t KEAAttributeTable::getNumFloatFields()
    {
        return this->numFloatFields;
    }
    
    size_t KEAAttributeTable::getNumStringFields()
    {
        return this->numStringFields;
    }
    
    void KEAAttributeTable::addAttBoolField(std::string name, bool val, std::string usage) throw(KEAATTException)
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
    
    void KEAAttributeTable::addAttIntField(std::string name, long val, std::string usage) throw(KEAATTException)
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
    
    void KEAAttributeTable::addAttFloatField(std::string name, double val, std::string usage) throw(KEAATTException)
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
    
    void KEAAttributeTable::addAttStringField(std::string name, std::string val, std::string usage) throw(KEAATTException)
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
    
    void KEAAttributeTable::addFields(std::vector<KEAATTField> fields) throw(KEAATTException)
    {
        try 
        {
            for(std::vector<KEAATTField>::iterator iterFields = fields.begin(); iterFields != fields.end(); ++iterFields)
            {
                if((*iterFields).dataType == kea_att_bool)
                {
                    this->addAttBoolField((*iterFields).name, false);
                }
                else if((*iterFields).dataType == kea_att_int)
                {
                    this->addAttIntField((*iterFields).name, 0);
                }
                else if((*iterFields).dataType == kea_att_float)
                {
                    this->addAttFloatField((*iterFields).name, 0);
                }
                else if((*iterFields).dataType == kea_att_string)
                {
                    this->addAttStringField((*iterFields).name, "");
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
        feat->intFields = new std::vector<long>();
        feat->floatFields = new std::vector<double>();
        feat->strFields = new std::vector<std::string>();
        feat->neighbours = new std::vector<size_t>();
        feat->fid = 0;
        
        return feat;
    }
    
    void KEAAttributeTable::exportToASCII(std::string outputFile)throw(KEAATTException, KEAIOException)
    {
        
    }
    
    void KEAAttributeTable::exportToHDF(std::string outputFile)throw(KEAATTException, KEAIOException)
    {
        
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
            
            H5::CompType *keaStrDataType = new H5::CompType( sizeof(KEAAttString) );
            keaStrDataType->insertMember(KEA_ATT_STRING_FIELD, HOFFSET(KEAAttString, str), strTypeDisk);
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
            
            H5::CompType *keaStrDataType = new H5::CompType( sizeof(KEAAttString) );
            keaStrDataType->insertMember(KEA_ATT_STRING_FIELD, HOFFSET(KEAAttString, str), strTypeMem);
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
        
    }
    
}