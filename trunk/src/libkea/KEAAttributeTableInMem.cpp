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

namespace libkea{
    
    KEAAttributeTableInMem::KEAAttributeTableInMem() : KEAAttributeTable(kea_att_mem)
    {
        attRows = new std::vector<KEAATTFeature*>();
    }
    
    bool KEAAttributeTableInMem::getBoolField(size_t fid, std::string name) throw(KEAATTException)
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
    
    long KEAAttributeTableInMem::getIntField(size_t fid, std::string name) throw(KEAATTException)
    {
        long value = 0;
        try 
        {
            KEAATTField field = this->getField(name);
            if(field.dataType != kea_att_bool)
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
    
    double KEAAttributeTableInMem::getFloatField(size_t fid, std::string name) throw(KEAATTException)
    {
        double value = false;
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
    
    std::string KEAAttributeTableInMem::getStringField(size_t fid, std::string name) throw(KEAATTException)
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
            
            value = this->getBoolField(fid, field.idx);
        }
        catch (KEAATTException &e)
        {
            throw e;
        }
        return value;
    }
    
    void KEAAttributeTableInMem::setBoolField(size_t fid, std::string name, bool value) throw(KEAATTException)
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
    
    void KEAAttributeTableInMem::setIntField(size_t fid, std::string name, long value) throw(KEAATTException)
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
    
    void KEAAttributeTableInMem::setFloatField(size_t fid, std::string name, double value) throw(KEAATTException)
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
    
    void KEAAttributeTableInMem::setStringField(size_t fid, std::string name, std::string value) throw(KEAATTException)
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
    
    bool KEAAttributeTableInMem::getBoolField(size_t fid, size_t colIdx) throw(KEAATTException)
    {
        if(fid > attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx > attRows->at(fid)->boolFields->size())
        {
            std::string message = std::string("Requested boolean column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        return attRows->at(fid)->boolFields->at(colIdx);
    }
    
    long KEAAttributeTableInMem::getIntField(size_t fid, size_t colIdx) throw(KEAATTException)
    {
        if(fid > attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx > attRows->at(fid)->boolFields->size())
        {
            std::string message = std::string("Requested integer column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        return attRows->at(fid)->intFields->at(colIdx);
    }
    
    double KEAAttributeTableInMem::getFloatField(size_t fid, size_t colIdx) throw(KEAATTException)
    {
        if(fid > attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx > attRows->at(fid)->boolFields->size())
        {
            std::string message = std::string("Requested float column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        return attRows->at(fid)->floatFields->at(colIdx);
    }
    
    std::string KEAAttributeTableInMem::getStringField(size_t fid, size_t colIdx) throw(KEAATTException)
    {
        if(fid > attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx > attRows->at(fid)->boolFields->size())
        {
            std::string message = std::string("Requested string column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        return attRows->at(fid)->strFields->at(colIdx);
    }
    
    void KEAAttributeTableInMem::setBoolField(size_t fid, size_t colIdx, bool value) throw(KEAATTException)
    {
        if(fid > attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx > attRows->at(fid)->boolFields->size())
        {
            std::string message = std::string("Requested boolean column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        attRows->at(fid)->boolFields->at(colIdx) = value;
    }
    
    void KEAAttributeTableInMem::setIntField(size_t fid, size_t colIdx, long value) throw(KEAATTException)
    {
        if(fid > attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx > attRows->at(fid)->intFields->size())
        {
            std::string message = std::string("Requested integer column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        attRows->at(fid)->intFields->at(colIdx) = value;
    }
    
    void KEAAttributeTableInMem::setFloatField(size_t fid, size_t colIdx, double value) throw(KEAATTException)
    {
        if(fid > attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx > attRows->at(fid)->floatFields->size())
        {
            std::string message = std::string("Requested float column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        attRows->at(fid)->floatFields->at(colIdx) = value;
    }
    
    void KEAAttributeTableInMem::setStringField(size_t fid, size_t colIdx, std::string value) throw(KEAATTException)
    {
        if(fid > attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        if(colIdx > attRows->at(fid)->strFields->size())
        {
            std::string message = std::string("Requested string column (") + sizet2Str(colIdx) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        attRows->at(fid)->strFields->at(colIdx) = value;
    }
    
    KEAATTFeature* KEAAttributeTableInMem::getFeature(size_t fid) throw(KEAATTException)
    {
        if(fid > attRows->size())
        {
            std::string message = std::string("Requested feature (") + sizet2Str(fid) + std::string(") is not within the table.");
            throw KEAATTException(message);
        }
        
        return attRows->at(fid);
    }
        
    size_t KEAAttributeTableInMem::getSize()
    {
        return attRows->size();
    }
    
    void KEAAttributeTableInMem::addAttBoolField(KEAATTField field, bool val) throw(KEAATTException)
    {
        for(std::vector<KEAATTFeature*>::iterator iterFeat = attRows->begin(); iterFeat != attRows->end(); ++iterFeat)
        {
            (*iterFeat)->boolFields->push_back(val);
        }
    }
    
    void KEAAttributeTableInMem::addAttIntField(KEAATTField field, int val) throw(KEAATTException)
    {
        for(std::vector<KEAATTFeature*>::iterator iterFeat = attRows->begin(); iterFeat != attRows->end(); ++iterFeat)
        {
            (*iterFeat)->intFields->push_back(val);
        }
    }
    
    void KEAAttributeTableInMem::addAttFloatField(KEAATTField field, float val) throw(KEAATTException)
    {
        for(std::vector<KEAATTFeature*>::iterator iterFeat = attRows->begin(); iterFeat != attRows->end(); ++iterFeat)
        {
            (*iterFeat)->floatFields->push_back(val);
        }
    }
    
    void KEAAttributeTableInMem::addAttStringField(KEAATTField field, std::string val) throw(KEAATTException)
    {
        for(std::vector<KEAATTFeature*>::iterator iterFeat = attRows->begin(); iterFeat != attRows->end(); ++iterFeat)
        {
            (*iterFeat)->strFields->push_back(val);
        }
    }
    
    void KEAAttributeTableInMem::addRows(size_t numRows)
    {        
        KEAATTFeature *feat = NULL;
        
        for(size_t i = 0; i < numRows; ++i)
        {
            feat = this->createKeaFeature();
            feat->fid = attRows->size();
            attRows->push_back(feat);
        }
    }
    
    void KEAAttributeTableInMem::exportToKeaFile(H5::H5File *keaImg, unsigned int band, unsigned int chunkSize, unsigned int deflate)throw(KEAATTException, KEAIOException)
    {
        std::cout << "EXPORTING TO KEA FILE\n";
        try
        {
            if(attRows->size() == 0)
            {
                throw KEAATTException("There is no attribute table to be saved to the file.");
            }
            
            std::string bandPathBase = KEA_DATASETNAME_BAND + uint2Str(band);
            std::cout << "Band Path Base = " << bandPathBase << std::endl;
            std::cout << "Band Path Base + KEA_ATT_BOOL_DATA = " << (bandPathBase + KEA_ATT_BOOL_DATA) << std::endl;
            
            // Read header size.
            size_t attSize = 0;
            try 
            {   
                hsize_t dimsValue[1];
                dimsValue[0] = 1;
                H5::DataSpace valueDataSpace(1, dimsValue);
                H5::DataSet datasetAttSize = keaImg->openDataSet(bandPathBase + KEA_ATT_SIZE_HEADER );
                datasetAttSize.read(&attSize, H5::PredType::STD_U64LE, valueDataSpace);
                datasetAttSize.close();
                valueDataSpace.close();
            } 
            catch ( H5::Exception &e) 
            {
                throw KEAIOException("The attribute table size field is not present.");
            }
            
            std::cout << "attribute table size = " << attSize << std::endl;
            
            KEAAttributeIdx *boolFields = NULL;
            if(this->numBoolFields > 0)
            {
                boolFields = new KEAAttributeIdx[this->numBoolFields];
            }
            KEAAttributeIdx *intFields = NULL;
            if(this->numIntFields > 0)
            {
                intFields = new KEAAttributeIdx[this->numIntFields];
            }
            KEAAttributeIdx *floatFields = NULL;
            if(this->numFloatFields > 0)
            {
                floatFields = new KEAAttributeIdx[this->numFloatFields];
            }
            KEAAttributeIdx *stringFields = NULL;
            if(this->numStringFields > 0)
            {
                stringFields = new KEAAttributeIdx[this->numStringFields];
            }
            
            unsigned int boolFieldsIdx = 0;
            unsigned int intFieldsIdx = 0;
            unsigned int floatFieldsIdx = 0;
            unsigned int stringFieldIdx = 0;
            for(std::map<std::string, KEAATTField>::iterator iterField = fields->begin(); iterField != fields->end(); ++iterField)
            {
                if((*iterField).second.dataType == kea_att_bool)
                {
                    boolFields[boolFieldsIdx].name = const_cast<char*>((*iterField).second.name.c_str());
                    boolFields[boolFieldsIdx].idx = (*iterField).second.idx;
                    boolFields[boolFieldsIdx].usage = const_cast<char*>((*iterField).second.usage.c_str());
                    ++boolFieldsIdx;
                }
                else if((*iterField).second.dataType == kea_att_int)
                {
                    intFields[intFieldsIdx].name = const_cast<char*>((*iterField).second.name.c_str());
                    intFields[intFieldsIdx].idx = (*iterField).second.idx;
                    intFields[intFieldsIdx].usage = const_cast<char*>((*iterField).second.usage.c_str());
                    ++intFieldsIdx;
                }
                else if((*iterField).second.dataType == kea_att_float)
                {
                    floatFields[floatFieldsIdx].name = const_cast<char*>((*iterField).second.name.c_str());
                    floatFields[floatFieldsIdx].idx = (*iterField).second.idx;
                    floatFields[floatFieldsIdx].usage = const_cast<char*>((*iterField).second.usage.c_str());
                    ++floatFieldsIdx;
                }
                else if((*iterField).second.dataType == kea_att_string)
                {
                    stringFields[stringFieldIdx].name = const_cast<char*>((*iterField).second.name.c_str());
                    stringFields[stringFieldIdx].idx = (*iterField).second.idx;
                    stringFields[stringFieldIdx].usage = const_cast<char*>((*iterField).second.usage.c_str());
                    ++stringFieldIdx;
                }
                else
                {
                    throw KEAATTException("Data type was not recognised.");
                }
            }
            
            
            H5::DataSet *boolDataset;
            H5::DataSet *intDataset;
            H5::DataSet *floatDataset;
            H5::DataSet *strDataset;
            H5::DataSet *neighboursDataset;
            
            H5::StrType strType(0, H5T_VARIABLE);
            size_t strMaxLen = 0;
            if(this->numStringFields > 0)
            {
                for(size_t i = 0; i < attRows->size(); ++i)
                {
                    for(size_t j = 0; j < this->numStringFields; ++j)
                    {
                        if((i == 0) & (i == 0))
                        {
                            strMaxLen = attRows->at(i)->strFields->at(j).length();
                        }
                        else if(attRows->at(i)->strFields->at(j).length() > strMaxLen)
                        {
                            strMaxLen = attRows->at(i)->strFields->at(j).length();
                        }
                    }
                }
            }
            std::cout << "Max. String length is: " << strMaxLen << std::endl;
            
            if(attSize > 0)
            {
                // THERE IS AN EXISTING TABLE AND YOU NEED TO MAKE SURE THEY ARE BUG ENOUGH.
                
            }
            else
            {
                // THERE IS NO EXISTING TABLE AND THE DATA STRUCTURES NEEDS TO BE CREATED.
                std::cout << "Creating new attribute table." << std::endl;
                
                H5::CompType *fieldDtDisk = this->createAttibuteIdxCompTypeDisk();
                if(this->numBoolFields > 0)
                {
                    hsize_t initDimsBoolFieldsDS[1];
                    initDimsBoolFieldsDS[0] = 0;
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
                    
                    hsize_t extendBoolFieldsDatasetTo[1];
                    extendBoolFieldsDatasetTo[0] = this->numBoolFields;
                    boolFieldsDataset.extend( extendBoolFieldsDatasetTo );
                    
                    hsize_t boolFieldsOffset[1];
                    boolFieldsOffset[0] = 0;
                    hsize_t boolFieldsDataDims[1];
                    boolFieldsDataDims[0] = this->numBoolFields;
                    
                    H5::DataSpace boolFieldsWriteDataSpace = boolFieldsDataset.getSpace();
                    boolFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolFieldsDataDims, boolFieldsOffset);
                    H5::DataSpace newBoolFieldsDataspace = H5::DataSpace(1, boolFieldsDataDims);
                    
                    boolFieldsDataset.write(boolFields, *fieldDtDisk, newBoolFieldsDataspace, boolFieldsWriteDataSpace);
                    
                    delete[] boolFields;
                }
                
                if(this->numIntFields > 0)
                {
                    hsize_t initDimsIntFieldsDS[1];
                    initDimsIntFieldsDS[0] = 0;
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
                    
                    hsize_t extendIntFieldsDatasetTo[1];
                    extendIntFieldsDatasetTo[0] = this->numIntFields;
                    intFieldsDataset.extend( extendIntFieldsDatasetTo );
                    
                    hsize_t intFieldsOffset[1];
                    intFieldsOffset[0] = 0;
                    hsize_t intFieldsDataDims[1];
                    intFieldsDataDims[0] = this->numIntFields;
                    
                    H5::DataSpace intFieldsWriteDataSpace = intFieldsDataset.getSpace();
                    intFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intFieldsDataDims, intFieldsOffset);
                    H5::DataSpace newIntFieldsDataspace = H5::DataSpace(1, intFieldsDataDims);
                    
                    intFieldsDataset.write(intFields, *fieldDtDisk, newIntFieldsDataspace, intFieldsWriteDataSpace);
                    
                    delete[] intFields;
                }
                
                if(this->numFloatFields > 0)
                {
                    hsize_t initDimsFloatFieldsDS[1];
                    initDimsFloatFieldsDS[0] = 0;
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
                    
                    hsize_t extendFloatFieldsDatasetTo[1];
                    extendFloatFieldsDatasetTo[0] = this->numFloatFields;
                    floatFieldsDataset.extend( extendFloatFieldsDatasetTo );
                    
                    hsize_t floatFieldsOffset[1];
                    floatFieldsOffset[0] = 0;
                    hsize_t floatFieldsDataDims[1];
                    floatFieldsDataDims[0] = this->numFloatFields;
                    
                    H5::DataSpace floatFieldsWriteDataSpace = floatFieldsDataset.getSpace();
                    floatFieldsWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatFieldsDataDims, floatFieldsOffset);
                    H5::DataSpace newFloatFieldsDataspace = H5::DataSpace(1, floatFieldsDataDims);
                    
                    floatFieldsDataset.write(floatFields, *fieldDtDisk, newFloatFieldsDataspace, floatFieldsWriteDataSpace);
                    
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
                    
                    stringFieldsDataset.write(stringFields, *fieldDtDisk, newStringFieldsDataspace, stringFieldsWriteDataSpace);
                    
                    delete[] stringFields;
                }
                delete fieldDtDisk;
                
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
                    creationBoolDSPList.setFillValue( H5::PredType::STD_I8LE, &fillValueBool);
                    
                    boolDataset = new H5::DataSet(keaImg->createDataSet((bandPathBase + KEA_ATT_BOOL_DATA), H5::PredType::STD_I8LE, boolDataSpace, creationBoolDSPList));
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
                    
                    long fillValueInt = 0;
                    H5::DSetCreatPropList creationIntDSPList;
                    creationIntDSPList.setChunk(2, dimsIntChunk);
                    creationIntDSPList.setShuffle();
                    creationIntDSPList.setDeflate(deflate);
                    creationIntDSPList.setFillValue( H5::PredType::STD_I64LE, &fillValueInt);
                    
                    intDataset = new H5::DataSet(keaImg->createDataSet((bandPathBase + KEA_ATT_INT_DATA), H5::PredType::STD_I64LE, intDataSpace, creationIntDSPList));
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
                    creationFloatDSPList.setFillValue( H5::PredType::IEEE_F64LE, &fillValueFloat);
                    
                    floatDataset = new H5::DataSet(keaImg->createDataSet((bandPathBase + KEA_ATT_FLOAT_DATA), H5::PredType::IEEE_F64LE, floatDataSpace, creationFloatDSPList));
                }
                
                if(this->numStringFields > 0)
                {
                    // Create the string
                    hsize_t initDimsString[3];
                    initDimsString[0] = strMaxLen;
                    initDimsString[1] = this->attRows->size();
                    initDimsString[2] = this->numStringFields;
                    hsize_t maxDimsString[3];
                    maxDimsString[0] = H5S_UNLIMITED;
                    maxDimsString[1] = H5S_UNLIMITED;
                    maxDimsString[2] = H5S_UNLIMITED;
                    H5::DataSpace stringDataSpace = H5::DataSpace(3, initDimsString, maxDimsString);
                    
                    hsize_t dimsStringChunk[3];
                    dimsStringChunk[0] = chunkSize;
                    dimsStringChunk[1] = chunkSize;
                    dimsStringChunk[2] = 1;
                    
                    const char *fillValueStr = std::string("").c_str();
                    H5::DSetCreatPropList creationStringDSPList;
                    creationStringDSPList.setChunk(3, dimsStringChunk);
                    creationStringDSPList.setShuffle();
                    creationStringDSPList.setDeflate(deflate);
                    creationStringDSPList.setFillValue(strType, &fillValueStr);
                    
                    strDataset = new H5::DataSet(keaImg->createDataSet((bandPathBase + KEA_ATT_STRING_DATA), strType, stringDataSpace, creationStringDSPList));
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
                hvl_t neighboursDataFillVal[1];
                neighboursDataFillVal[0].p = NULL;
                neighboursDataFillVal[0].length = 0;
                H5::DSetCreatPropList creationNeighboursDSPList;
                creationNeighboursDSPList.setChunk(1, dimsNeighboursChunk);
                creationNeighboursDSPList.setShuffle();
                creationNeighboursDSPList.setDeflate(deflate);
                creationNeighboursDSPList.setFillValue( intVarLenDiskDT, &neighboursDataFillVal);
                
                neighboursDataset = new H5::DataSet(keaImg->createDataSet((bandPathBase + KEA_ATT_NEIGHBOURS_DATA), intVarLenDiskDT, neighboursDataspace, creationNeighboursDSPList));
            }
            
            // WRITE DATA INTO THE STRUCTURE.
            size_t numOfBlocks = 0;
            numOfBlocks = floor(attRows->size()/chunkSize);
            size_t remainRows = attRows->size() - (numOfBlocks * chunkSize);
            
            
            
            int *boolData = NULL;
            if(this->numBoolFields > 0)
            {
                boolData = new int[this->numBoolFields*chunkSize];
            }
            long *intData = NULL;
            if(this->numIntFields > 0)
            {
                intData = new long[this->numIntFields*chunkSize];
            }
            double *floatData = NULL;
            if(this->numFloatFields > 0)
            {
                floatData = new double[this->numFloatFields*chunkSize];
            }
            char *stringData = NULL;
            char *singleStr = NULL;
            if(this->numStringFields > 0)
            {
                singleStr = new char[strMaxLen];
                stringData = new char[this->numStringFields*chunkSize*strMaxLen];
            }
                        
            hvl_t *neighbourVals = new hvl_t[chunkSize];
            
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
                
                hsize_t stringDataOffset[3];
                stringDataOffset[0] = 0;
                stringDataOffset[1] = 0;
                stringDataOffset[2] = 0;
                hsize_t stringDataDims[3];
                stringDataDims[0] = strMaxLen;
                stringDataDims[1] = chunkSize;
                stringDataDims[2] = this->numStringFields;
                
                hsize_t neighboursDataOffset[1];
                neighboursDataOffset[0] = 0;
                hsize_t neighboursDataDims[1];
                neighboursDataDims[0] = chunkSize;
                H5::DataType intVarLenMemDT = H5::VarLenType(&H5::PredType::NATIVE_UINT64);
                
                
                size_t rowOff = 0;
                size_t strOff = 0;
                for(size_t n = 0; n < numOfBlocks; ++n)
                {
                    rowOff = n * chunkSize;
                    for(size_t i = 0; i < chunkSize; ++i)
                    {
                        for(size_t j = 0; j < this->numBoolFields; ++j)
                        {
                            boolData[(i*this->numBoolFields)+j] = attRows->at(rowOff+i)->boolFields->at(j);
                        }
                        for(size_t j = 0; j < this->numIntFields; ++j)
                        {
                            intData[(i*this->numIntFields)+j] = attRows->at(rowOff+i)->intFields->at(j);
                        }
                        for(size_t j = 0; j < this->numFloatFields; ++j)
                        {
                            floatData[(i*this->numFloatFields)+j] = attRows->at(rowOff+i)->floatFields->at(j);
                        }
                        for(size_t j = 0; j < this->numStringFields; ++j)
                        {
                            strncpy(singleStr, attRows->at(rowOff+i)->strFields->at(j).c_str(), strMaxLen);
                            //std::cout << "singleStr = " << singleStr << std::endl;
                            strOff = (rowOff * (this->numStringFields*strMaxLen)) + (i*(this->numStringFields*strMaxLen)) + (j*strMaxLen);
                            for(size_t k = 0; k < strMaxLen; ++k)
                            {
                                //std::cout << "singleStr[" << k << "] = " << singleStr[k] << std::endl;
                                stringData[strOff+k] = singleStr[k];
                            }
                        }
                    }
                    
                    //std::cout << "STRING: " << stringData << std::endl;
                    
                    if(this->numBoolFields > 0)
                    {
                        boolDataOffset[0] = rowOff;
                        boolDataOffset[1] = 0;
                        
                        H5::DataSpace boolWriteDataSpace = boolDataset->getSpace();
                        boolWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolDataDims, boolDataOffset);
                        H5::DataSpace newBoolDataspace = H5::DataSpace(2, boolDataDims);
                        
                        boolDataset->write(boolData, H5::PredType::NATIVE_INT, newBoolDataspace, boolWriteDataSpace);
                    }
                    
                    if(this->numIntFields > 0)
                    {
                        intDataOffset[0] = rowOff;
                        intDataOffset[1] = 0;
                        
                        H5::DataSpace intWriteDataSpace = intDataset->getSpace();
                        intWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intDataDims, intDataOffset);
                        H5::DataSpace newIntDataspace = H5::DataSpace(2, intDataDims);
                        
                        intDataset->write(intData, H5::PredType::NATIVE_LONG, newIntDataspace, intWriteDataSpace);
                    }
                    
                    if(this->numFloatFields > 0)
                    {
                        floatDataOffset[0] = rowOff;
                        floatDataOffset[1] = 0;
                        
                        H5::DataSpace floatWriteDataSpace = floatDataset->getSpace();
                        floatWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatDataDims, floatDataOffset);
                        H5::DataSpace newFloatDataspace = H5::DataSpace(2, floatDataDims);
                        
                        floatDataset->write(floatData, H5::PredType::NATIVE_DOUBLE, newFloatDataspace, floatWriteDataSpace);
                    }
                    
                    if(this->numStringFields > 0)
                    {
                        stringDataOffset[0] = 0;
                        stringDataOffset[1] = rowOff;
                        stringDataOffset[2] = 0;
                        
                        H5::DataSpace stringWriteDataSpace = strDataset->getSpace();
                        stringWriteDataSpace.selectHyperslab(H5S_SELECT_SET, stringDataDims, stringDataOffset);
                        H5::DataSpace newStringDataspace = H5::DataSpace(3, stringDataDims);
                        
                        //strDataset->write(stringData, strType, newStringDataspace, stringWriteDataSpace);
                    }
                    
                    
                }                
            }
            
            if(remainRows > 0)
            {
                hsize_t boolDataOffset[2];
                boolDataOffset[0] = 0;
                boolDataOffset[1] = 0;
                hsize_t boolDataDims[2];
                boolDataDims[0] = remainRows;
                boolDataDims[1] = this->numBoolFields;
                
                hsize_t intDataOffset[2];
                intDataOffset[0] = 0;
                intDataOffset[1] = 0;
                hsize_t intDataDims[2];
                intDataDims[0] = remainRows;
                intDataDims[1] = this->numIntFields;
                
                hsize_t floatDataOffset[2];
                floatDataOffset[0] = 0;
                floatDataOffset[1] = 0;
                hsize_t floatDataDims[2];
                floatDataDims[0] = remainRows;
                floatDataDims[1] = this->numFloatFields;
                
                hsize_t stringDataOffset[3];
                stringDataOffset[0] = 0;
                stringDataOffset[1] = 0;
                stringDataOffset[2] = 0;
                hsize_t stringDataDims[3];
                stringDataDims[0] = strMaxLen;
                stringDataDims[1] = remainRows;
                stringDataDims[2] = this->numStringFields;
                
                hsize_t neighboursDataOffset[1];
                neighboursDataOffset[0] = 0;
                hsize_t neighboursDataDims[1];
                neighboursDataDims[0] = chunkSize;
                H5::DataType intVarLenMemDT = H5::VarLenType(&H5::PredType::NATIVE_UINT64);
                
                
                size_t rowOff = numOfBlocks * chunkSize;
                size_t strOff = 0;
                
                for(size_t i = 0; i < remainRows; ++i)
                {
                    for(size_t j = 0; j < this->numBoolFields; ++j)
                    {
                        boolData[(i*this->numBoolFields)+j] = attRows->at(rowOff+i)->boolFields->at(j);
                    }
                    for(size_t j = 0; j < this->numIntFields; ++j)
                    {
                        intData[(i*this->numIntFields)+j] = attRows->at(rowOff+i)->intFields->at(j);
                    }
                    for(size_t j = 0; j < this->numFloatFields; ++j)
                    {
                        floatData[(i*this->numFloatFields)+j] = attRows->at(rowOff+i)->floatFields->at(j);
                    }
                    for(size_t j = 0; j < this->numStringFields; ++j)
                    {
                        strncpy(singleStr, attRows->at(rowOff+i)->strFields->at(j).c_str(), strMaxLen);
                        //std::cout << "singleStr = " << singleStr << std::endl;
                        strOff = (rowOff * (this->numStringFields*strMaxLen)) + (i*(this->numStringFields*strMaxLen)) + (j*strMaxLen);
                        for(size_t k = 0; k < strMaxLen; ++k)
                        {
                            //std::cout << "singleStr[" << k << "] = " << singleStr[k] << std::endl;
                            stringData[strOff+k] = singleStr[k];
                        }
                    }
                }
                
                //std::cout << "STRING: " << stringData << std::endl;
                
                if(this->numBoolFields > 0)
                {
                    boolDataOffset[0] = rowOff;
                    boolDataOffset[1] = 0;
                    
                    H5::DataSpace boolWriteDataSpace = boolDataset->getSpace();
                    boolWriteDataSpace.selectHyperslab(H5S_SELECT_SET, boolDataDims, boolDataOffset);
                    H5::DataSpace newBoolDataspace = H5::DataSpace(2, boolDataDims);
                    
                    boolDataset->write(boolData, H5::PredType::NATIVE_INT, newBoolDataspace, boolWriteDataSpace);
                }
                
                if(this->numIntFields > 0)
                {
                    intDataOffset[0] = rowOff;
                    intDataOffset[1] = 0;
                    
                    H5::DataSpace intWriteDataSpace = intDataset->getSpace();
                    intWriteDataSpace.selectHyperslab(H5S_SELECT_SET, intDataDims, intDataOffset);
                    H5::DataSpace newIntDataspace = H5::DataSpace(2, intDataDims);
                    
                    intDataset->write(intData, H5::PredType::NATIVE_LONG, newIntDataspace, intWriteDataSpace);
                }
                
                if(this->numFloatFields > 0)
                {
                    floatDataOffset[0] = rowOff;
                    floatDataOffset[1] = 0;
                    
                    H5::DataSpace floatWriteDataSpace = floatDataset->getSpace();
                    floatWriteDataSpace.selectHyperslab(H5S_SELECT_SET, floatDataDims, floatDataOffset);
                    H5::DataSpace newFloatDataspace = H5::DataSpace(2, floatDataDims);
                    
                    floatDataset->write(floatData, H5::PredType::NATIVE_DOUBLE, newFloatDataspace, floatWriteDataSpace);
                }
                
                if(this->numStringFields > 0)
                {
                    stringDataOffset[0] = 0;
                    stringDataOffset[1] = rowOff;
                    stringDataOffset[2] = 0;
                    
                    H5::DataSpace stringWriteDataSpace = strDataset->getSpace();
                    stringWriteDataSpace.selectHyperslab(H5S_SELECT_SET, stringDataDims, stringDataOffset);
                    H5::DataSpace newStringDataspace = H5::DataSpace(3, stringDataDims);
                    
                    //strDataset->write(stringData, strType, newStringDataspace, stringWriteDataSpace);
                }

            }
            
            if(this->numBoolFields > 0)
            {
                delete[] boolData;
            }
            if(this->numIntFields > 0)
            {
                delete[] intData;
            }
            if(this->numFloatFields > 0)
            {
                delete[] floatData;
            }
            if(this->numStringFields > 0)
            {
                delete[] stringData;
                delete[] singleStr;
            }
            
            delete boolDataset;
            delete intDataset;
            delete floatDataset;
            delete strDataset;
            delete neighboursDataset;
        }
        catch (KEAATTException &e)
        {
            throw e;
        }
        catch (KEAIOException &e)
        {
            throw e;
        }
    }
    
    KEAAttributeTable* KEAAttributeTableInMem::createKeaAtt(H5::H5File *keaImg, unsigned int band)throw(KEAATTException, KEAIOException)
    {
        // Create instance of class to populate and return.
        KEAAttributeTableInMem *att = new KEAAttributeTableInMem();
        
        try
        {
            // Read header size.
            size_t attSize = 0;
            try 
            {   
                hsize_t dimsValue[1];
                dimsValue[0] = 1;
                H5::DataSpace valueDataSpace(1, dimsValue);
                H5::DataSet datasetAttSize = keaImg->openDataSet( KEA_DATASETNAME_BAND + uint2Str(band) + KEA_ATT_SIZE_HEADER );
                datasetAttSize.read(&attSize, H5::PredType::STD_U64LE, valueDataSpace);
                datasetAttSize.close();
                valueDataSpace.close();
            } 
            catch ( H5::Exception &e) 
            {
                throw KEAIOException("The attribute table size field is not present.");
            }
            
            if(attSize > 0)
            {
                // READ ATTRIBUTE TABLE FROM KEA IMAGE BAND...
                
            }
        }
        catch (KEAATTException &e)
        {
            throw e;
        }
        catch (KEAIOException &e)
        {
            throw e;
        }
        
        return att;
    }
    
    KEAAttributeTableInMem::~KEAAttributeTableInMem()
    {
        
    }
    
}