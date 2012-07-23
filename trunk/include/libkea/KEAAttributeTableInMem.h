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

#ifndef KEAAttributeTableInMem_H
#define KEAAttributeTableInMem_H

#include <iostream>
#include <string>
#include <vector>

#include "H5Cpp.h"

#include "libkea/KEACommon.h"
#include "libkea/KEAException.h"
#include "libkea/KEAAttributeTable.h"

namespace libkea{
       
    class DllExport KEAAttributeTableInMem : public KEAAttributeTable
    {
    public:
        KEAAttributeTableInMem();
        
        bool getBoolField(size_t fid, std::string name) throw(KEAATTException);
        long getIntField(size_t fid, std::string name) throw(KEAATTException);
        double getFloatField(size_t fid, std::string name) throw(KEAATTException);
        std::string getStringField(size_t fid, std::string name) throw(KEAATTException);
        
        void setBoolField(size_t fid, std::string name, bool value) throw(KEAATTException);
        void setIntField(size_t fid, std::string name, long value) throw(KEAATTException);
        void setFloatField(size_t fid, std::string name, double value) throw(KEAATTException);
        void setStringField(size_t fid, std::string name, std::string value) throw(KEAATTException);
        
        bool getBoolField(size_t fid, size_t colIdx) throw(KEAATTException);
        long getIntField(size_t fid, size_t colIdx) throw(KEAATTException);
        double getFloatField(size_t fid, size_t colIdx) throw(KEAATTException);
        std::string getStringField(size_t fid, size_t colIdx) throw(KEAATTException);
        
        void setBoolField(size_t fid, size_t colIdx, bool value) throw(KEAATTException);
        void setIntField(size_t fid, size_t colIdx, long value) throw(KEAATTException);
        void setFloatField(size_t fid, size_t colIdx, double value) throw(KEAATTException);
        void setStringField(size_t fid, size_t colIdx, std::string value) throw(KEAATTException);
        
        KEAATTFeature* getFeature(size_t fid) throw(KEAATTException);
        
        size_t getSize();
        
        void addAttBoolField(KEAATTField field, bool val) throw(KEAATTException);
        void addAttIntField(KEAATTField field, int val) throw(KEAATTException);
        void addAttFloatField(KEAATTField field, float val) throw(KEAATTException);
        void addAttStringField(KEAATTField field, std::string val) throw(KEAATTException);
        
        void addRows(size_t numRows);
        
        void exportToKeaFile(H5::H5File *keaImg, unsigned int band, unsigned int chunkSize=KEA_ATT_CHUNK_SIZE, unsigned int deflate=KEA_DEFLATE)throw(KEAATTException, KEAIOException);
        
        static KEAAttributeTable* createKeaAtt(H5::H5File *keaImg, unsigned int band)throw(KEAATTException, KEAIOException);
        
        ~KEAAttributeTableInMem();
    protected:
        std::vector<KEAATTFeature*> *attRows;
    };
    
}


#endif




