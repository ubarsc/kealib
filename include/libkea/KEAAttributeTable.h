/*
 *  KEAAttributeTable.h
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

#ifndef KEAAttributeTable_H
#define KEAAttributeTable_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <math.h>

#include <highfive/highfive.hpp>

#include "libkea/KEACommon.h"
#include "libkea/KEAException.h"

namespace kealib{
    
    enum KEAATTType
    {
        kea_att_undefined = 0,
        kea_att_mem = 1,
        kea_att_file = 2
    };
    
    struct KEAATTFeature
    {
        size_t fid;
        std::vector<bool> *boolFields;
        std::vector<int64_t> *intFields;
        std::vector<double> *floatFields;
        std::vector<std::string> *strFields;
        std::vector<size_t> *neighbours;
    };
    
    enum KEAFieldDataType
    {
        kea_att_na = 0,
        kea_att_bool = 1,
        kea_att_int = 2,
        kea_att_float = 3,
        kea_att_string = 4
    };
    
    struct KEAATTField
    {
        std::string name;
        KEAFieldDataType dataType;
        size_t idx;
        std::string usage;
        size_t colNum;
    };
    
    struct KEAAttributeIdx
    {
        char *name;
        unsigned int idx;
        char *usage;
        unsigned int colNum;
    };
    
    struct KEAAttString
    {
        char *str;
    };
    
    typedef struct 
    {
        size_t length;
        void   *p;
    } VarLenFieldHDF;
    
    class KEA_EXPORT KEAAttributeTable : public KEABase
    {
    public:
        KEAAttributeTable(KEAATTType keaAttType, const std::shared_ptr<kealib::kea_mutex>& mutex);
        virtual KEAATTType getKEAATTType() const;
        
        virtual bool getBoolField(size_t fid, const std::string &name) const;
        virtual int64_t getIntField(size_t fid, const std::string &name) const;
        virtual double getFloatField(size_t fid, const std::string &name) const;
        virtual std::string getStringField(size_t fid, const std::string &name) const;
        
        virtual void setBoolField(size_t fid, const std::string &name, bool value);
        virtual void setIntField(size_t fid, const std::string &name, int64_t value);
        virtual void setFloatField(size_t fid, const std::string &name, double value);
        virtual void setStringField(size_t fid, const std::string &name, const std::string &value);
        
        // TODO: needed?
        virtual void setBoolValue(const std::string &name, bool value);
        virtual void setIntValue(const std::string &name, int64_t value);
        virtual void setFloatValue(const std::string &name, double value);
        virtual void setStringValue(const std::string &name, const std::string &value);
        
        // TODO: needed?
        virtual bool getBoolField(size_t fid, size_t colIdx) const;
        virtual int64_t getIntField(size_t fid, size_t colIdx) const;
        virtual double getFloatField(size_t fid, size_t colIdx) const;
        virtual std::string getStringField(size_t fid, size_t colIdx) const;

        // RFC40 methods
        virtual void getBoolFields(size_t startfid, size_t len, size_t colIdx, bool *pbBuffer) const;
        virtual void getIntFields(size_t startfid, size_t len, size_t colIdx, int64_t *pnBuffer) const;
        virtual void getFloatFields(size_t startfid, size_t len, size_t colIdx, double *pfBuffer) const;
        virtual void getStringFields(size_t startfid, size_t len, size_t colIdx, std::vector<std::string> *psBuffer) const;
        virtual void getNeighbours(size_t startfid, size_t len, std::vector<std::vector<size_t>* > *neighbours) const;
        
        virtual void setBoolField(size_t fid, size_t colIdx, bool value);
        virtual void setIntField(size_t fid, size_t colIdx, int64_t value);
        virtual void setFloatField(size_t fid, size_t colIdx, double value);
        virtual void setStringField(size_t fid, size_t colIdx, const std::string &value);

        // RFC40 methods
        virtual void setBoolFields(size_t startfid, size_t len, size_t colIdx, bool *pbBuffer);
        virtual void setIntFields(size_t startfid, size_t len, size_t colIdx, int64_t *pnBuffer);
        virtual void setFloatFields(size_t startfid, size_t len, size_t colIdx, double *pfBuffer);
        virtual void setStringFields(size_t startfid, size_t len, size_t colIdx, std::vector<std::string> *papszStrList);
        virtual void setNeighbours(size_t startfid, size_t len, std::vector<std::vector<size_t>* > *neighbours);
        
        // TODO: needed?
        virtual void setBoolValue(size_t colIdx, bool value);
        virtual void setIntValue(size_t colIdx, int64_t value);
        virtual void setFloatValue(size_t colIdx, double value);
        virtual void setStringValue(size_t colIdx, const std::string &value);
        
        virtual KEAATTFeature* getFeature(size_t fid) const;
        
        virtual void addAttBoolField(const std::string &name, bool val, std::string usage="");
        virtual void addAttIntField(const std::string &name, int64_t val, std::string usage="");
        virtual void addAttFloatField(const std::string &name, double val, std::string usage="");
        virtual void addAttStringField(const std::string &name, const std::string &val, std::string usage="");
        virtual void addFields(std::vector<KEAATTField*> *inFields);
        virtual void addFields(std::vector<KEAATTField> inFields);
        
        virtual KEAFieldDataType getDataFieldType(const std::string &name) const;
        virtual size_t getFieldIndex(const std::string &name) const;
        virtual KEAATTField getField(const std::string &name) const;
        virtual KEAATTField getField(size_t globalColIdx) const;
        virtual std::vector<std::string> getFieldNames() const;
        virtual bool hasField(const std::string &name) const;
        virtual size_t getNumBoolFields() const;
        virtual size_t getNumIntFields() const;
        virtual size_t getNumFloatFields() const;
        virtual size_t getNumStringFields() const;
        
        virtual size_t getSize() const;
        virtual size_t getTotalNumOfCols() const;
        virtual size_t getMaxGlobalColIdx() const;
        virtual std::string getBandPathBase() const;
        virtual size_t getChunkSize() const;
        virtual void addRows(size_t numRows);
        
        virtual void exportToKeaFile(HighFive::File *keaImg, unsigned int band, unsigned int chunkSize=KEA_ATT_CHUNK_SIZE, unsigned int deflate=KEA_DEFLATE);
        virtual void exportToASCII(const std::string &outputFile);
        
        virtual void printAttributeTableHeaderInfo();
        
        virtual ~KEAAttributeTable();

        // for cross heap use in Windows
        static void destroyAttributeTable(KEAAttributeTable *pTable);
        
        static HighFive::CompoundType createAttibuteIdxCompType();
        static HighFive::CompoundType createKeaStringCompType();
        
    protected:
        static KEAAttributeTable* createKeaAtt(HighFive::File *keaImg, const std::shared_ptr<kealib::kea_mutex>& mutex, unsigned int band, unsigned int chunkSizeIn);
        virtual void addAttBoolField(KEAATTField field, bool val);
        virtual void addAttIntField(KEAATTField field, int64_t val);
        virtual void addAttFloatField(KEAATTField field, float val);
        virtual void addAttStringField(KEAATTField field, const std::string &val);
        virtual KEAATTFeature* createKeaFeature();
        virtual void deleteKeaFeature(KEAATTFeature *feat);
        
        std::map<std::string, KEAATTField> *fields;
        KEAATTType attType;
        size_t numBoolFields;
        size_t numIntFields;
        size_t numFloatFields;
        size_t numStringFields;
        size_t numOfCols;
        size_t numRows;
        std::string bandPathBase;
        size_t chunkSize;
    };
}


#endif




