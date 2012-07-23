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

#include "H5Cpp.h"

#include "libkea/KEACommon.h"
#include "libkea/KEAException.h"

namespace libkea{
    
    enum KEAATTType
    {
        kea_att_undefined = 0,
        kea_att_mem = 1,
        kea_att_cached = 2
    };
    
    struct KEAATTFeature
    {
        size_t fid;
        std::vector<bool> *boolFields;
        std::vector<long> *intFields;
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
    
    class DllExport KEAAttributeTable
    {
    public:
        KEAAttributeTable(KEAATTType keaAttType);
        virtual KEAATTType getKEAATTType();
        
        virtual bool getBoolField(size_t fid, std::string name) throw(KEAATTException)=0;
        virtual long getIntField(size_t fid, std::string name) throw(KEAATTException)=0;
        virtual double getFloatField(size_t fid, std::string name) throw(KEAATTException)=0;
        virtual std::string getStringField(size_t fid, std::string name) throw(KEAATTException)=0;
        
        virtual void setBoolField(size_t fid, std::string name, bool value) throw(KEAATTException)=0;
        virtual void setIntField(size_t fid, std::string name, long value) throw(KEAATTException)=0;
        virtual void setFloatField(size_t fid, std::string name, double value) throw(KEAATTException)=0;
        virtual void setStringField(size_t fid, std::string name, std::string value) throw(KEAATTException)=0;
        
        virtual void setBoolValue(std::string name, bool value) throw(KEAATTException);
        virtual void setIntValue(std::string name, long value) throw(KEAATTException);
        virtual void setFloatValue(std::string name, double value) throw(KEAATTException);
        virtual void setStringValue(std::string name, std::string value) throw(KEAATTException);
        
        virtual bool getBoolField(size_t fid, size_t colIdx) throw(KEAATTException)=0;
        virtual long getIntField(size_t fid, size_t colIdx) throw(KEAATTException)=0;
        virtual double getFloatField(size_t fid, size_t colIdx) throw(KEAATTException)=0;
        virtual std::string getStringField(size_t fid, size_t colIdx) throw(KEAATTException)=0;
        
        virtual void setBoolField(size_t fid, size_t colIdx, bool value) throw(KEAATTException)=0;
        virtual void setIntField(size_t fid, size_t colIdx, long value) throw(KEAATTException)=0;
        virtual void setFloatField(size_t fid, size_t colIdx, double value) throw(KEAATTException)=0;
        virtual void setStringField(size_t fid, size_t colIdx, std::string value) throw(KEAATTException)=0;
        
        virtual void setBoolValue(size_t colIdx, bool value) throw(KEAATTException);
        virtual void setIntValue(size_t colIdx, long value) throw(KEAATTException);
        virtual void setFloatValue(size_t colIdx, double value) throw(KEAATTException);
        virtual void setStringValue(size_t colIdx, std::string value) throw(KEAATTException);
        
        virtual KEAATTFeature* getFeature(size_t fid) throw(KEAATTException)=0;
        
        virtual void addAttBoolField(std::string name, bool val, std::string usage="") throw(KEAATTException);
        virtual void addAttIntField(std::string name, long val, std::string usage="") throw(KEAATTException);
        virtual void addAttFloatField(std::string name, double val, std::string usage="") throw(KEAATTException);
        virtual void addAttStringField(std::string name, std::string val, std::string usage="") throw(KEAATTException);
        virtual void addFields(std::vector<KEAATTField> fields) throw(KEAATTException);
        
        virtual KEAFieldDataType getDataFieldType(std::string name) throw(KEAATTException);
        virtual size_t getFieldIndex(std::string name) throw(KEAATTException);
        virtual KEAATTField getField(std::string name) throw(KEAATTException);
        virtual KEAATTField getField(size_t globalColIdx) throw(KEAATTException);
        virtual std::vector<std::string> getFieldNames();
        virtual bool hasField(std::string name);
        virtual size_t getNumBoolFields();
        virtual size_t getNumIntFields();
        virtual size_t getNumFloatFields();
        virtual size_t getNumStringFields();
        
        virtual size_t getSize()=0;
        virtual size_t getTotalNumOfCols();
        virtual size_t getMaxGlobalColIdx();
        virtual void addRows(size_t numRows)=0;
        
        virtual void exportToKeaFile(H5::H5File *keaImg, unsigned int band, unsigned int chunkSize=KEA_ATT_CHUNK_SIZE, unsigned int deflate=KEA_DEFLATE)throw(KEAATTException, KEAIOException)=0;
        virtual void exportToASCII(std::string outputFile)throw(KEAATTException, KEAIOException);
        
        virtual ~KEAAttributeTable();
    protected:
        static H5::CompType* createAttibuteIdxCompTypeDisk() throw(KEAATTException);
        static H5::CompType* createAttibuteIdxCompTypeMem() throw(KEAATTException);
        static H5::CompType* createKeaStringCompTypeDisk() throw(KEAATTException);
        static H5::CompType* createKeaStringCompTypeMem() throw(KEAATTException);
        virtual void addAttBoolField(KEAATTField field, bool val) throw(KEAATTException)=0;
        virtual void addAttIntField(KEAATTField field, int val) throw(KEAATTException)=0;
        virtual void addAttFloatField(KEAATTField field, float val) throw(KEAATTException)=0;
        virtual void addAttStringField(KEAATTField field, std::string val) throw(KEAATTException)=0;
        KEAATTFeature* createKeaFeature() throw(KEAATTException);
        
        std::map<std::string, KEAATTField> *fields;
        KEAATTType attType;
        size_t numBoolFields;
        size_t numIntFields;
        size_t numFloatFields;
        size_t numStringFields;
        size_t numOfCols;
    };
}


#endif




