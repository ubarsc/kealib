/*
 *  extrat.cpp
 *  LibKEA
 *
 *  Created by Sam Gillingham on 09/02/2023.
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
 
#include <stdlib.h>
#include <string>
#include <exception>
#include <algorithm>
#include <unordered_map>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "awkward/LayoutBuilder.h"

#include "libkea/KEAImageIO.h"

#ifdef WIN32
    #include <Windows.h>
    #define DEFAULT_GDAL "gdal.dll"
#else
    #include <dlfcn.h>
    #ifdef __APPLE__
        #define DEFAULT_GDAL "libgdal.dylib"
    #else
        #define DEFAULT_GDAL "libgdal.so"
    #endif
#endif

template<class PRIMITIVE, class BUILDER>
using ListOffsetBuilder = awkward::LayoutBuilder::ListOffset<PRIMITIVE, BUILDER>;
template<class PRIMITIVE>
using NumpyBuilder = awkward::LayoutBuilder::Numpy<PRIMITIVE>;


// GLOBAL function pointers we use
// set to values in SetFunctionPointers() (set on first use)
void* (*pGetDriver)(void*) = nullptr;
const char * (*pGetDescription)(void*) = nullptr;
void *(*pGetInternalHandle)(void *, const char*) = nullptr;
bool IsFakeGDAL = false;
void SetFunctionPointers();

// Exception class. Wrapped by Python.
class PyKeaLibException : public std::exception
{
public:
    PyKeaLibException(std::string msg)
    {
        m_msg = msg;    
    }
    virtual const char* what() const throw()
    {
        return m_msg.c_str();
    }
private:
    std::string m_msg;
};

/* Taken from TuiView */
/* bit of a hack here - this is what a SWIG object looks
 like. It is only defined in the source file so we copy it here*/
typedef struct {
  PyObject_HEAD
  void *ptr;
  /* the rest aren't needed (and a hassle to define here)
  swig_type_info *ty;
  int own;
  PyObject *next;*/
} SwigPyObject;

/* in the ideal world I would use SWIG_ConvertPtr etc
 but to avoid the whole dependence on SWIG headers
 and needing to reach into the GDAL source to get types etc
 I happen to know the 'this' attribute is a pointer to 
 GDALDatasetShadow which is actually a pointer
 to a SwigPyObject whose ptr field is a pointer to a
 GDALDataset. Phew. 
 given a python object this function returns the underlying
 pointer. Returns nullptr on failure (exception string already set)*/
void *getUnderlyingPtrFromSWIGPyObject(pybind11::object &input/*, PyObject *pException*/)
{
    // get the "this" attribute
    auto thisAttr = input.attr("this");
    
    bool IsFakeHandle = pybind11::hasattr(input, "is_fake");
    if(IsFakeGDAL && !IsFakeHandle)
    {
        throw PyKeaLibException("Attempted to use fake GDAL with real dataset");
    }
    else if( !IsFakeGDAL && IsFakeHandle)
    {
        throw PyKeaLibException("Attempted to use real GDAL with fake dataset");
    }

    /* convert this to a SwigPyObject*/
    /* Need a C style cast here - nothing else works */
    SwigPyObject *pSwigThisAttr = (SwigPyObject*)thisAttr.ptr();

    /* get the ptr field*/
    void *pUnderlying = pSwigThisAttr->ptr;
    if( pUnderlying == nullptr )
    {
        throw PyKeaLibException("Underlying Pointer is nullptr");
    }

    return pUnderlying;
}

// helper function to delete the contained neighbour vectors from the 
// KEA data structure.
void freeNeighbourLists(std::vector<std::vector<size_t>* > *pNeighbours)
{
    /* frees all the sub vectors of pNeighbours */
    for( auto itr = pNeighbours->begin();
            itr != pNeighbours->end(); itr++)
    {
        delete *itr;
    }
}

// helper function to get the underlying KEA KEAImageIO object from GDAL
// given a dataset.
kealib::KEAImageIO *getImageIOFromDataset(pybind11::object &dataset)
{
    // load libgdal if it hasn't already been done
    // (doing this here rather than on import so testing package
    // import can happen without gdal)
    // probably should do some sort of thread safe thing here.
    if( pGetDriver == nullptr )
    {
        SetFunctionPointers();
    }

    void *pPtr = getUnderlyingPtrFromSWIGPyObject(dataset);
    
    // now do the actual calls with our function pointers 
    void *pDriver = (*pGetDriver)(pPtr);
    if( !pDriver )
    {
        throw PyKeaLibException("GDALGetDatasetDriver returned nullptr");
    }
    const char *pszName = (*pGetDescription)(pDriver);
    if( !pszName )
    {
        throw PyKeaLibException("GDALGetDescription returned nullptr");
    }
    if( (strlen(pszName) != 3) || (pszName[0] != 'K') || (pszName[1] != 'E') || (pszName[2] != 'A'))
    {
        throw PyKeaLibException("This function only works on KEA files");
    }

    kealib::KEAImageIO *pImageIO = (kealib::KEAImageIO*)(*pGetInternalHandle)(pPtr, nullptr);
    if( pImageIO == nullptr )
    {
        throw PyKeaLibException("GDALGetInternalHandle returned nullptr");
    }

    return pImageIO;
}

template <typename T>
pybind11::object snapshot_builder(const T& builder)
{
    // How much memory to allocate?
    std::map<std::string, size_t> names_nbytes = {};
    builder.buffer_nbytes(names_nbytes);

    // Allocate memory
    std::map<std::string, void*> buffers = {};
    for(auto it : names_nbytes) {
        uint8_t* ptr = new uint8_t[it.second];
        buffers[it.first] = (void*)ptr;
    }

    // Write non-contiguous contents to memory
    builder.to_buffers(buffers);
    auto from_buffers = pybind11::module_::import("awkward").attr("from_buffers");

    // Build Python dictionary containing arrays
    // dtypes not important here as long as they match the underlying buffer
    // as Awkward Array calls `frombuffer` to convert to the correct type
    pybind11::dict container;
    for (auto it: buffers) {
        
        pybind11::capsule free_when_done(it.second, [](void *data) {
            uint8_t* dataPtr = reinterpret_cast<uint8_t*>(data);
            delete[] dataPtr;
        });

        uint8_t* data = reinterpret_cast<uint8_t*>(it.second);
        container[pybind11::str(it.first)] = pybind11::array_t<uint8_t>(
            {names_nbytes[it.first]},
            {sizeof(uint8_t)},
            data,
            free_when_done
        );
    }
    return from_buffers(builder.form(), builder.length(), container);
    
}

// Get the neighbours for the given dataset and band. The
// neighbours are returned as an awkward array. The reading
// of neighbours starts at the startfid row in the RAT and
// stops after 'length' rows.
pybind11::object getNeighbours(pybind11::object &dataset, uint32_t nBand, int &startfid, int &length)
{
    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset);
    
    // Note: ListOffsetBuilder doesn't work with size_t/uint64_t
    ListOffsetBuilder<int64_t, NumpyBuilder<size_t>> builder;

    try
    {
        kealib::KEAAttributeTable *pRAT = pImageIO->getAttributeTable(kealib::kea_att_file, nBand);
        if( pRAT == nullptr )
        {
            throw PyKeaLibException("No Attribute table in this file");
        }

        /* Read the neighbours */
        std::vector<std::vector<size_t>* > neighbours;
        pRAT->getNeighbours(startfid, length, &neighbours);

        for( auto itr = neighbours.begin(); itr != neighbours.end(); itr++ )
        {
            auto& subbuilder = builder.begin_list();
            
            std::vector<size_t>* pSubVect = *itr;
            subbuilder.extend(pSubVect->data(), pSubVect->size());
            
            builder.end_list();
        }
        
        freeNeighbourLists(&neighbours);
    }
    catch(const kealib::KEAException &e)
    {
        throw PyKeaLibException(e.what());
    }

    return snapshot_builder(builder);
}

template<typename T>
void copyToNeighbours(std::vector<std::vector<size_t>* > &cppneighbours,
    pybind11::object &offsets, pybind11::object &data)
{
    auto offsetsArray = offsets.cast<pybind11::array_t<int64_t>>();
    auto dataArray = data.cast<pybind11::array_t<T>>();
    for( size_t n = 0; n < cppneighbours.size(); n++ )
    {
        auto startIdx = offsetsArray.at(n);
        auto endIdx = offsetsArray.at(n + 1);
        auto thisLength = endIdx - startIdx;
        auto *pVec = new std::vector<size_t>(thisLength);
        for( auto Idx = 0; Idx < thisLength; Idx++ )
        {
            pVec->at(Idx) = dataArray.at(startIdx + Idx);
        }
        cppneighbours.at(n) = pVec;
    }
}

// Set the neighbours for the given dataset and band. The
// neighbours are to be given as an awkward array and the writing
// starts at the startfid row in the table.
void setNeighbours(pybind11::object &dataset, uint32_t nBand, 
        int startfid, pybind11::object &neighbours)
{
    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset);

    try
    {
        kealib::KEAAttributeTable *pRAT = pImageIO->getAttributeTable(kealib::kea_att_file, nBand);
        if( pRAT == nullptr )
        {
            throw PyKeaLibException("No Attribute table in this file");
        }


        auto to_buffers = pybind11::module_::import("awkward").attr("to_buffers");
        auto result_tuple = to_buffers(neighbours).cast<pybind11::tuple>();
        
        auto form = result_tuple[0];
        auto formDict = form.attr("to_dict")().cast<pybind11::dict>();
        auto classType = formDict[pybind11::str("class")].cast<std::string>();
        if( classType != "ListOffsetArray")
        {
            throw PyKeaLibException("Only accept ListOffsetArrays");
        }
        auto offsetsType = formDict[pybind11::str("offsets")].cast<std::string>();
        if( offsetsType != "i64")
        {
            throw PyKeaLibException("Only accept i64 ListOffsetArrays");
        }
        auto offsetsKey = formDict[pybind11::str("form_key")].cast<std::string>();
        
        auto content = formDict[pybind11::str("content")].cast<pybind11::dict>();
        auto contentClass = content[pybind11::str("class")].cast<std::string>();
        if( contentClass != "NumpyArray" )
        {
            throw PyKeaLibException("Only accept NumpyArray for Content");
        }
        auto contentPrimitive = content[pybind11::str("primitive")].cast<std::string>();
        auto contentKey = content[pybind11::str("form_key")].cast<std::string>();

        auto length = result_tuple[1].cast<int64_t>();

        pybind11::dict container = result_tuple[2];
        pybind11::object offsetsArray = pybind11::none();
        pybind11::object dataArray = pybind11::none();
        for(auto item: container)
        {
            auto key = item.first.cast<std::string>();
            if( key.rfind(offsetsKey, 0) != std::string::npos )
            {
                offsetsArray = item.second.cast<pybind11::object>();
            }
            else if( key.rfind(contentKey, 0) != std::string::npos )
            {
                dataArray = item.second.cast<pybind11::object>();
            }
        }
        
        if( offsetsArray.is_none() || dataArray.is_none() )
        {
            throw PyKeaLibException("Couldn't find offsets or data");
        }
        
        std::vector<std::vector<size_t>* > cppneighbours(length);
        
        if(contentPrimitive == "uint8")
            copyToNeighbours<uint8_t>(cppneighbours, offsetsArray, dataArray);
        else if( contentPrimitive == "int8")
            copyToNeighbours<int8_t>(cppneighbours, offsetsArray, dataArray);
        else if( contentPrimitive == "uint16")
            copyToNeighbours<uint16_t>(cppneighbours, offsetsArray, dataArray);
        else if( contentPrimitive == "int16" )
            copyToNeighbours<int16_t>(cppneighbours, offsetsArray, dataArray);
        else if( contentPrimitive ==  "uint32" )
            copyToNeighbours<uint32_t>(cppneighbours, offsetsArray, dataArray);
        else if( contentPrimitive == "int32" )
            copyToNeighbours<int32_t>(cppneighbours, offsetsArray, dataArray);
        else if( contentPrimitive == "uint64" )
            copyToNeighbours<uint64_t>(cppneighbours, offsetsArray, dataArray);
        else if( contentPrimitive == "int64" )
            copyToNeighbours<int64_t>(cppneighbours, offsetsArray, dataArray);
        else if( contentPrimitive == "float32" )
            copyToNeighbours<float>(cppneighbours, offsetsArray, dataArray);
        else if( contentPrimitive == "float64" )
            copyToNeighbours<double>(cppneighbours, offsetsArray, dataArray);
        else
            throw PyKeaLibException("Unknown data type");
        
        
        pRAT->setNeighbours(startfid, cppneighbours.size(), &cppneighbours);
        
        freeNeighbourLists(&cppneighbours);
    }
    catch(const kealib::KEAException &e)
    {
        throw PyKeaLibException(e.what());
    }
}

// Function to add a column to the RAT of a KEA file.
// The user will need to close the dataset and open again to see the
// new column
void addField(pybind11::object &dataset, uint32_t nBand, 
        const std::string& name, kealib::KEAFieldDataType nType, 
        pybind11::object &initVal, const std::string &usage)
{
    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset);

    try
    {
        kealib::KEAAttributeTable *pRAT = pImageIO->getAttributeTable(kealib::kea_att_file, nBand);
        if( pRAT == nullptr )
        {
            throw PyKeaLibException("No Attribute table in this file");
        }
        
        if( nType == kealib::kea_att_bool)
        {
            pRAT->addAttBoolField(name, initVal.cast<bool>(), usage);
        }
        else if( nType == kealib::kea_att_int)
        {
            pRAT->addAttIntField(name, initVal.cast<int64_t>(), usage);
        }
        else if( nType == kealib::kea_att_float)
        {
            pRAT->addAttFloatField(name, initVal.cast<double>(), usage);
        }
        else if( nType == kealib::kea_att_string)
        {
            pRAT->addAttStringField(name, initVal.cast<std::string>(), usage);
        }
        else 
        {
            throw PyKeaLibException("Unsupported Type");
        }

    }
    catch(const kealib::KEAException &e)
    {
        throw PyKeaLibException(e.what());
    }
}

size_t getNumFields(pybind11::object &dataset, uint32_t nBand)
{
    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset);

    try
    {
        kealib::KEAAttributeTable *pRAT = pImageIO->getAttributeTable(kealib::kea_att_file, nBand);
        if( pRAT == nullptr )
        {
            throw PyKeaLibException("No Attribute table in this file");
        }
        return pRAT->getTotalNumOfCols();
    }
    catch(const kealib::KEAException &e)
    {
        throw PyKeaLibException(e.what());
    }
}

pybind11::object getFieldByName(pybind11::object &dataset, uint32_t nBand, const std::string &name)
{
    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset);

    try
    {
        kealib::KEAAttributeTable *pRAT = pImageIO->getAttributeTable(kealib::kea_att_file, nBand);
        if( pRAT == nullptr )
        {
            throw PyKeaLibException("No Attribute table in this file");
        }
        return pybind11::cast(pRAT->getField(name));
    }
    catch(const kealib::KEAException &e)
    {
        throw PyKeaLibException(e.what());
    }
}

pybind11::object getFieldByIdx(pybind11::object &dataset, uint32_t nBand, int idx)
{
    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset);

    try
    {
        kealib::KEAAttributeTable *pRAT = pImageIO->getAttributeTable(kealib::kea_att_file, nBand);
        if( pRAT == nullptr )
        {
            throw PyKeaLibException("No Attribute table in this file");
        }
        return pybind11::cast(pRAT->getField(idx));
    }
    catch(const kealib::KEAException &e)
    {
        throw PyKeaLibException(e.what());
    }
}

size_t getSize(pybind11::object &dataset, uint32_t nBand)
{
    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset);

    try
    {
        kealib::KEAAttributeTable *pRAT = pImageIO->getAttributeTable(kealib::kea_att_file, nBand);
        if( pRAT == nullptr )
        {
            throw PyKeaLibException("No Attribute table in this file");
        }
        return pRAT->getSize();    
    }
    catch(const kealib::KEAException &e)
    {
        throw PyKeaLibException(e.what());
    }
}

void addRows(pybind11::object &dataset, uint32_t nBand, size_t numRows)
{
    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset);

    try
    {
        kealib::KEAAttributeTable *pRAT = pImageIO->getAttributeTable(kealib::kea_att_file, nBand);
        if( pRAT == nullptr )
        {
            throw PyKeaLibException("No Attribute table in this file");
        }
        pRAT->addRows(numRows);    
    }
    catch(const kealib::KEAException &e)
    {
        throw PyKeaLibException(e.what());
    }
}

pybind11::object getField(pybind11::object &dataset, uint32_t nBand, kealib::KEAATTField field,
    size_t startfid, size_t len)
{
    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset);

    try
    {
        kealib::KEAAttributeTable *pRAT = pImageIO->getAttributeTable(kealib::kea_att_file, nBand);
        if( pRAT == nullptr )
        {
            throw PyKeaLibException("No Attribute table in this file");
        }
        
        if(field.dataType == kealib::kea_att_bool)
        {
            auto result = pybind11::array_t<bool>(len);
            pybind11::buffer_info buf = result.request();
            pRAT->getBoolFields(startfid, len, field.idx, static_cast<bool*>(buf.ptr));
            return result;
        }
        else if(field.dataType == kealib::kea_att_int)
        {
            auto result = pybind11::array_t<int64_t>(len);
            pybind11::buffer_info buf = result.request();
            pRAT->getIntFields(startfid, len, field.idx, static_cast<int64_t*>(buf.ptr));
            return result;
        }
        else if(field.dataType == kealib::kea_att_float)
        {
            auto result = pybind11::array_t<double>(len);
            pybind11::buffer_info buf = result.request();
            pRAT->getFloatFields(startfid, len, field.idx, static_cast<double*>(buf.ptr));
            return result;
        }
        else if(field.dataType == kealib::kea_att_string)
        {
            ListOffsetBuilder<int64_t, NumpyBuilder<uint8_t>> builder;
            builder.content().set_parameters("\"__array__\": \"char\"");
            std::vector<std::string> buffer;
            pRAT->getStringFields(startfid, len, field.idx, &buffer);
            for( auto itr = buffer.begin(); itr != buffer.end(); itr++)
            {
                auto& subbuilder = builder.begin_list();
                const auto& str = *itr; 
                subbuilder.extend(reinterpret_cast<uint8_t*>(const_cast<char*>(str.c_str())), str.size());
                builder.end_list();
            }
            return snapshot_builder(builder);
        }
        else
        {
            throw PyKeaLibException("Unknown field type");
        }
    }
    catch(const kealib::KEAException &e)
    {
        throw PyKeaLibException(e.what());
    }
}

void setField(pybind11::object &dataset, uint32_t nBand, kealib::KEAATTField field,
    size_t startfid, pybind11::object &data)
{
    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset);

    try
    {
        kealib::KEAAttributeTable *pRAT = pImageIO->getAttributeTable(kealib::kea_att_file, nBand);
        if( pRAT == nullptr )
        {
            throw PyKeaLibException("No Attribute table in this file");
        }
        
        if(field.dataType == kealib::kea_att_bool)
        {
            auto dataArray = data.cast<pybind11::array_t<bool>>();
            pybind11::buffer_info buf = dataArray.request();
            pRAT->setBoolFields(startfid, dataArray.size(), field.idx, static_cast<bool*>(buf.ptr));
        }
        else if(field.dataType == kealib::kea_att_int)
        {
            auto dataArray = data.cast<pybind11::array_t<int64_t>>();
            pybind11::buffer_info buf = dataArray.request();
            pRAT->setIntFields(startfid, dataArray.size(), field.idx, static_cast<int64_t*>(buf.ptr));
        }
        else if(field.dataType == kealib::kea_att_float)
        {
            auto dataArray = data.cast<pybind11::array_t<double>>();
            pybind11::buffer_info buf = dataArray.request();
            pRAT->setFloatFields(startfid, dataArray.size(), field.idx, static_cast<double*>(buf.ptr));
        }
        else if(field.dataType == kealib::kea_att_string)
        {
            auto to_buffers = pybind11::module_::import("awkward").attr("to_buffers");
            auto result_tuple = to_buffers(data).cast<pybind11::tuple>();
            
            auto form = result_tuple[0];
            auto formDict = form.attr("to_dict")().cast<pybind11::dict>();
            auto classType = formDict[pybind11::str("class")].cast<std::string>();
            if( classType != "ListOffsetArray")
            {
                throw PyKeaLibException("Only accept ListOffsetArrays");
            }
            auto offsetsType = formDict[pybind11::str("offsets")].cast<std::string>();
            if( offsetsType != "i64")
            {
                throw PyKeaLibException("Only accept i64 ListOffsetArrays");
            }
            auto offsetsKey = formDict[pybind11::str("form_key")].cast<std::string>();
            
            auto content = formDict[pybind11::str("content")].cast<pybind11::dict>();
            auto contentClass = content[pybind11::str("class")].cast<std::string>();
            if( contentClass != "NumpyArray" )
            {
                throw PyKeaLibException("Only accept NumpyArray for Content");
            }
            auto contentPrimitive = content[pybind11::str("primitive")].cast<std::string>();
            if( contentPrimitive != "uint8" )
            {
                throw PyKeaLibException("Only accept NumpyArray of uint8 for Content");
            }
            auto contentKey = content[pybind11::str("form_key")].cast<std::string>();
    
            auto length = result_tuple[1].cast<int64_t>();
    
            pybind11::dict container = result_tuple[2];
            pybind11::object offsetsArray = pybind11::none();
            pybind11::object dataArray = pybind11::none();
            for(auto item: container)
            {
                auto key = item.first.cast<std::string>();
                if( key.rfind(offsetsKey, 0) != std::string::npos )
                {
                    offsetsArray = item.second.cast<pybind11::object>();
                }
                else if( key.rfind(contentKey, 0) != std::string::npos )
                {
                    dataArray = item.second.cast<pybind11::object>();
                }
            }
            
            if( offsetsArray.is_none() || dataArray.is_none() )
            {
                throw PyKeaLibException("Couldn't find offsets or data");
            }
            
            std::vector<std::string> stringList(length);
            auto offsetsArrayNumpy = offsetsArray.cast<pybind11::array_t<int64_t>>();
            auto dataArrayNumpy = dataArray.cast<pybind11::array_t<char>>();
            for(int64_t n = 0; n < length; n++)
            {
                auto startIdx = offsetsArrayNumpy.at(n);
                auto endIdx = offsetsArrayNumpy.at(n + 1);
                auto thisLength = endIdx - startIdx;
                auto str = std::string(dataArrayNumpy.data(startIdx), thisLength);
                stringList.at(n) = str;
            }
            pRAT->setStringFields(startfid, length, field.idx, &stringList);
        }
        else
        {
            throw PyKeaLibException("Unknown field type");
        }
    }
    catch(const kealib::KEAException &e)
    {
        throw PyKeaLibException(e.what());
    }
}

pybind11::object getSpatialInfo(pybind11::object &dataset)
{
    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset);
    try
    {
        auto spatialInfo = *(pImageIO->getSpatialInfo());
        return pybind11::cast(spatialInfo);
    }
    catch(const kealib::KEAException &e)
    {
        throw PyKeaLibException(e.what());
    }
}

kealib::KEADataType getImageBandDataType(pybind11::object &dataset, uint32_t nBand)
{
    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset);
    try
    {
        return pImageIO->getImageBandDataType(nBand);
    }
    catch(const kealib::KEAException &e)
    {
        throw PyKeaLibException(e.what());
    }
}

uint32_t getNumOfImageBands(pybind11::object &dataset)
{
    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset);
    try
    {
        return pImageIO->getNumOfImageBands();
    }
    catch(const kealib::KEAException &e)
    {
        throw PyKeaLibException(e.what());
    }
}

double getNoDataValue(pybind11::object &dataset, uint32_t nBand)
{
    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset);
    try
    {
        double dNoData;
        pImageIO->getNoDataValue(nBand, &dNoData, kealib::kea_64float);
        return dNoData;
    }
    catch(const kealib::KEAException &e)
    {
        throw PyKeaLibException(e.what());
    }
}

pybind11::object getImageBlock(pybind11::object &dataset, uint32_t nBand, 
    uint64_t col, uint64_t row, uint64_t xsize, uint64_t ysize)
{
    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset);
    try
    {
        auto dtype = pImageIO->getImageBandDataType(nBand);
        
        if( dtype == kealib::kea_8int)
        {
            auto result = pybind11::array_t<int8_t>({ysize, xsize});
            pybind11::buffer_info buf = result.request();
            pImageIO->readImageBlock2Band(nBand, buf.ptr, col, row, xsize, ysize, xsize, ysize, dtype);
            return result;
        }
        else if( dtype == kealib::kea_8uint)
        {
            auto result = pybind11::array_t<uint8_t>({ysize, xsize});
            pybind11::buffer_info buf = result.request();
            pImageIO->readImageBlock2Band(nBand, buf.ptr, col, row, xsize, ysize, xsize, ysize, dtype);
            return result;
        }
        else if( dtype == kealib::kea_16int)
        {
            auto result = pybind11::array_t<int16_t>({ysize, xsize});
            pybind11::buffer_info buf = result.request();
            pImageIO->readImageBlock2Band(nBand, buf.ptr, col, row, xsize, ysize, xsize, ysize, dtype);
            return result;
        }
        else if( dtype == kealib::kea_16uint)
        {
            auto result = pybind11::array_t<uint16_t>({ysize, xsize});
            pybind11::buffer_info buf = result.request();
            pImageIO->readImageBlock2Band(nBand, buf.ptr, col, row, xsize, ysize, xsize, ysize, dtype);
            return result;
        }
        else if( dtype == kealib::kea_32int)
        {
            auto result = pybind11::array_t<int32_t>({ysize, xsize});
            pybind11::buffer_info buf = result.request();
            pImageIO->readImageBlock2Band(nBand, buf.ptr, col, row, xsize, ysize, xsize, ysize, dtype);
            return result;
        }
        else if( dtype == kealib::kea_32uint)
        {
            auto result = pybind11::array_t<uint32_t>({ysize, xsize});
            pybind11::buffer_info buf = result.request();
            pImageIO->readImageBlock2Band(nBand, buf.ptr, col, row, xsize, ysize, xsize, ysize, dtype);
            return result;
        }
        else if( dtype == kealib::kea_64int)
        {
            auto result = pybind11::array_t<int64_t>({ysize, xsize});
            pybind11::buffer_info buf = result.request();
            pImageIO->readImageBlock2Band(nBand, buf.ptr, col, row, xsize, ysize, xsize, ysize, dtype);
            return result;
        }
        else if( dtype == kealib::kea_64uint)
        {
            auto result = pybind11::array_t<uint64_t>({ysize, xsize});
            pybind11::buffer_info buf = result.request();
            pImageIO->readImageBlock2Band(nBand, buf.ptr, col, row, xsize, ysize, xsize, ysize, dtype);
            return result;
        }
        else if( dtype == kealib::kea_32float)
        {
            auto result = pybind11::array_t<float>({ysize, xsize});
            pybind11::buffer_info buf = result.request();
            pImageIO->readImageBlock2Band(nBand, buf.ptr, col, row, xsize, ysize, xsize, ysize, dtype);
            return result;
        }
        else if( dtype == kealib::kea_64float)
        {
            auto result = pybind11::array_t<double>({ysize, xsize});
            pybind11::buffer_info buf = result.request();
            pImageIO->readImageBlock2Band(nBand, buf.ptr, col, row, xsize, ysize, xsize, ysize, dtype);
            return result;
        }
        else
        {
            throw PyKeaLibException("Unsupported data type");
        }
    }
    catch(const kealib::KEAException &e)
    {
        throw PyKeaLibException(e.what());
    }
} 


// class that holds the neighbours and accumulates new neighbours
// from given 2d numpy arrays
class NeighbourAccumulator
{
public:
    NeighbourAccumulator(pybind11::array &hist, pybind11::object &dataset, 
                    uint32_t nBand, bool fourConnected=true);
    ~NeighbourAccumulator();
    
    void addArray(pybind11::array &arr);
    
private:
    // templated member function to find all neighbours from given 
    // 2d numby array. Type should match that of the numpy array.
    template<typename T>
    void addNeighbours(pybind11::array_t<T> input)
    {
        T nTypeIgnore = (T)m_ignore;
        ssize_t nYSize = input.shape(0);
        ssize_t nXSize = input.shape(1);
        
        // note: starting/ending one pixel in
        // so overlap of one must be selected
        // otherwise everything will be out of sync with the
        // histogram.
        for( ssize_t y = 1; y < (nYSize-1); y++ )
        {
            for( ssize_t x = 1; x < (nXSize-1); x++ )
            {
                T val = input.at(y, x);
                if( val != nTypeIgnore )
                {
                    std::vector<size_t> *pVec = nullptr;
                    auto found = m_neighbourMap.find(val);
                    if( found != m_neighbourMap.end() )
                    {
                        // already in our map - grab it
                        pVec = found->second;
                    }
                    else
                    {
                        // not in our map - add it
                        pVec = new std::vector<size_t>();
                        m_neighbourMap.insert({val, pVec});
                    }
                
                    // check neighbouring pixels - left/right and up/down
                    for( ssize_t x_off = -1; x_off < 2; x_off++ )
                    {
                        for( ssize_t y_off = -1; y_off < 2; y_off++ )
                        {
                            // don't worry about the middle pixel 
                            // - is the one we are trying to find neighbours for.
                            if( (x_off != 0) || (y_off != 0) )
                            {
                                // if 8 connected we consider all pixels
                                // otherwise only the horizontal and vertical ones.
                                if( !m_fourConnected || ((x_off == 0) || (y_off == 0)) )
                                {
                                    // don't need to check within the image here as we are 
                                    // starting/ending one pixel in from the edge (see above)
                                    ssize_t x_idx = x + x_off;
                                    ssize_t y_idx = y + y_off;
                                    T other_val = input.at(y_idx, x_idx);
                                    if( (val != other_val) && (other_val != nTypeIgnore ) )
                                    {
                                        // check we don't already have this pixel
                                        if( std::find(pVec->begin(), pVec->end(), other_val) == pVec->end() )
                                        {
                                            // not already in there...
                                            pVec->push_back(other_val);
                                        }
                                    }
                                }
                            }
                        }
                    }
                    
                    // decrement our copy of the histogram. 
                    m_pHistogram[val]--;
                    if( m_pHistogram[val] == 0 )
                    {
                        // need to write this one out - have visited each occurrance of it
                        // and searched for neighbours.
                        // Create a vector wrapping pVec
                        //fprintf(stderr, "Writing neighbours for %d\n", (int)val);
                        std::vector<std::vector<size_t>* > cppneighbours(1);
                        cppneighbours.at(0) = pVec;
                        try
                        {
                            m_pRAT->setNeighbours(val, 1, &cppneighbours);
                        }
                        catch(const kealib::KEAException &e)
                        {
                            throw PyKeaLibException(e.what());
                        }
                        
                        // remove it from our map and delete it.
                        m_neighbourMap.erase(val);
                        delete pVec;
                    }
                }
            }
        }
    }
    
    // templated member function to create and copy
    // the user supplied histogram into a known type.
    template<typename T>
    void copyHistogram(pybind11::array_t<T> input)
    {
        ssize_t nSize = input.size();
        m_pHistogram = new uint64_t[nSize];
        for( ssize_t i = 0; i < nSize; i++ )
        {
            T val = input.at(i);
            m_pHistogram[i] = val;
        }
    }
    
    // map keyed on image values where the value is a vector of neighbours
    std::unordered_map<size_t, std::vector<size_t>* > m_neighbourMap;
    // refernce to the RAT 
    kealib::KEAAttributeTable *m_pRAT;
    // our copy of the user supplied histogram
    uint64_t *m_pHistogram;
    // the ignore value obtained from the file
    double m_ignore;
    // whether to consider just 4 neighbours or 8.
    bool m_fourConnected;
};

// Constructor. Copy the histogram into our array and obtain 
// a reference to the RAT and get the ignore value
NeighbourAccumulator::NeighbourAccumulator(pybind11::array &hist, 
                    pybind11::object &dataset, 
                    uint32_t nBand, bool fourConnected/*=true*/)
    : m_fourConnected(fourConnected)
{
    if( hist.ndim() != 1 )
    {
        throw PyKeaLibException("Only support 1D arrays");
    }

    if(pybind11::isinstance<pybind11::array_t<int8_t>>(hist))
    {
        copyHistogram(hist.cast<pybind11::array_t<int8_t>>());
    }
    else if(pybind11::isinstance<pybind11::array_t<uint8_t>>(hist))
    {
        copyHistogram(hist.cast<pybind11::array_t<uint8_t>>());
    }
    else if(pybind11::isinstance<pybind11::array_t<int16_t>>(hist))
    {
        copyHistogram(hist.cast<pybind11::array_t<int16_t>>());
    }
    else if(pybind11::isinstance<pybind11::array_t<uint16_t>>(hist))
    {
        copyHistogram(hist.cast<pybind11::array_t<uint16_t>>());
    }
    else if(pybind11::isinstance<pybind11::array_t<int32_t>>(hist))
    {
        copyHistogram(hist.cast<pybind11::array_t<int32_t>>());
    }
    else if(pybind11::isinstance<pybind11::array_t<uint32_t>>(hist))
    {
        copyHistogram(hist.cast<pybind11::array_t<uint32_t>>());
    }
    else if(pybind11::isinstance<pybind11::array_t<int64_t>>(hist))
    {
        copyHistogram(hist.cast<pybind11::array_t<int64_t>>());
    }
    else if(pybind11::isinstance<pybind11::array_t<uint64_t>>(hist))
    {
        copyHistogram(hist.cast<pybind11::array_t<uint64_t>>());
    }
    else if(pybind11::isinstance<pybind11::array_t<float>>(hist))
    {
        copyHistogram(hist.cast<pybind11::array_t<float>>());
    }
    else if(pybind11::isinstance<pybind11::array_t<double>>(hist))
    {
        copyHistogram(hist.cast<pybind11::array_t<double>>());
    }
    else
    {
        throw PyKeaLibException("Unsupported data type");
    }    

    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset);
    
    try
    {
        pImageIO->getNoDataValue(nBand, &m_ignore, kealib::kea_64float);
    
        m_pRAT = pImageIO->getAttributeTable(kealib::kea_att_file, nBand);
        if( m_pRAT == nullptr )
        {
            throw PyKeaLibException("No Attribute table in this file");
        }
    }
    catch(const kealib::KEAException &e)
    {
        throw PyKeaLibException(e.what());
    }
}

// destructor. Delete our copy of the histogram and 
// check if anything is left in our map - should be 
// all written out if the histogram is correct.
NeighbourAccumulator::~NeighbourAccumulator()
{
    delete[] m_pHistogram;
    
    for( auto itr = m_neighbourMap.begin(); itr != m_neighbourMap.end(); itr++)
    {
        fprintf(stderr, "Neighbours not written for %d\n", (int)itr->first);
        delete itr->second;
    }
}

// get all neighbours from a 2d array
void NeighbourAccumulator::addArray(pybind11::array &arr)
{
    if( arr.ndim() != 2 )
    {
        throw PyKeaLibException("Only support 2D arrays");
    }

    if(pybind11::isinstance<pybind11::array_t<int8_t>>(arr))
    {
        addNeighbours(arr.cast<pybind11::array_t<int8_t>>());
    }
    else if(pybind11::isinstance<pybind11::array_t<uint8_t>>(arr))
    {
        addNeighbours(arr.cast<pybind11::array_t<uint8_t>>());
    }
    else if(pybind11::isinstance<pybind11::array_t<int16_t>>(arr))
    {
        addNeighbours(arr.cast<pybind11::array_t<int16_t>>());
    }
    else if(pybind11::isinstance<pybind11::array_t<uint16_t>>(arr))
    {
        addNeighbours(arr.cast<pybind11::array_t<uint16_t>>());
    }
    else if(pybind11::isinstance<pybind11::array_t<int32_t>>(arr))
    {
        addNeighbours(arr.cast<pybind11::array_t<int32_t>>());
    }
    else if(pybind11::isinstance<pybind11::array_t<uint32_t>>(arr))
    {
        addNeighbours(arr.cast<pybind11::array_t<uint32_t>>());
    }
    else if(pybind11::isinstance<pybind11::array_t<int64_t>>(arr))
    {
        addNeighbours(arr.cast<pybind11::array_t<int64_t>>());
    }
    else if(pybind11::isinstance<pybind11::array_t<uint64_t>>(arr))
    {
        addNeighbours(arr.cast<pybind11::array_t<uint64_t>>());
    }
    else
    {
        throw PyKeaLibException("Unsupported data type");
    }    
}

// sets global function pointers to GDAL functions we use
// - looked up at runtime to avoid compile time dependency on GDAL 
void SetFunctionPointers()
{
    const char *pszLibName = getenv("KEALIB_LIBGDAL");
    if( !pszLibName )
    {
        pszLibName = DEFAULT_GDAL;
    }
    
#ifdef WIN32
    HMODULE hModule = LoadLibraryA(pszLibName);
    if( hModule == NULL )
    {
        std::ostringstream stringStream;
        stringStream << "Unable to open ";
        stringStream << pszLibName;
        stringStream << " set the KEALIB_LIBGDAL env var to override";
        throw pybind11::import_error(stringStream.str());
    }
    
    pGetDriver = (void* (*)(void*))GetProcAddress(hModule, "GDALGetDatasetDriver");
    if( !pGetDriver )
    {
        throw pybind11::import_error("Unable fo find function GDALGetDatasetDriver");
    }
    
    pGetDescription = (const char*(*)(void*))GetProcAddress(hModule, "GDALGetDescription");
    if( !pGetDriver )
    {
        throw pybind11::import_error("Unable fo find function GDALGetDescription");
    }

    pGetInternalHandle = (void *(*)(void *, const char*))GetProcAddress(hModule, "GDALGetInternalHandle");
    if( ! pGetInternalHandle)
    {
        throw pybind11::import_error("Unable fo find function GDALGetInternalHandle");
    }
    
    // just test for the symbol existing
    IsFakeGDAL = (GetProcAddress(hModule, "IsFakeGDAL") != NULL);

#else
    char *error;
    void *libHandle = dlopen(pszLibName, RTLD_LAZY);
    if( !libHandle )
    {
        std::ostringstream stringStream;
        stringStream << "Unable to open ";
        stringStream << pszLibName;
        stringStream << " set the KEALIB_LIBGDAL env var to override";
        //fprintf(stderr, "%s\n", stringStream.str());
        throw pybind11::import_error(stringStream.str());
    }
    dlerror();  // clear any existing error
    
    pGetDriver = (void* (*)(void*)) dlsym(libHandle, "GDALGetDatasetDriver");
    error = dlerror();
    if( error != NULL )
    {
        throw pybind11::import_error(error);
    }
    
    pGetDescription = (const char*(*)(void*)) dlsym(libHandle, "GDALGetDescription");
    error = dlerror();
    if( error != NULL )
    {
        throw pybind11::import_error(error);
    }
    
    pGetInternalHandle = (void *(*)(void *, const char*)) dlsym(libHandle, "GDALGetInternalHandle");
    error = dlerror();
    if( error != NULL )
    {
        throw pybind11::import_error(error);
    }

    // just test for the symbol existing
    // Note: other sense from Windows
    dlsym(libHandle, "IsFakeGDAL");
    IsFakeGDAL = (dlerror() == NULL);  // there was no error

#endif
}

bool IsFakeGDALFunction()
{
    return IsFakeGDAL;
}

// Main mondule initialisation

PYBIND11_MODULE(extrat, m) {
    m.doc() = "Module that contains helpers for accessing kealib features not available from GDAL";

    m.def("IsFakeGDAL", &IsFakeGDALFunction);
    m.def("getNeighbours", &getNeighbours,
        "Return all the neighbours for the given dataset and band from startfid "
        "returing length features. Returned value is an awkward array",
        pybind11::arg("dataset"), pybind11::arg("band"), pybind11::arg("startfid"),
        pybind11::arg("length"));
    m.def("setNeighbours", &setNeighbours,
        "Set the neighbours for the given dataset band from startfid. "
        "neighbours should be an awkward array",
        pybind11::arg("dataset"), pybind11::arg("band"), pybind11::arg("startfid"),
        pybind11::arg("neighbours"));
    m.def("addField", &addField,
        "Add a column for the given dataset band. "
        "GDAL doesn't support creation of boolean columns, but "
        "presents boolean columns as integers and allows you to "
        "read and write them converting to/from integers. "
        "You will have to re-open the dataset with GDAL to see "
        "the new column.", pybind11::arg("dataset"), pybind11::arg("band"),
        pybind11::arg("name"), pybind11::arg("type"), pybind11::arg("initval"), 
        pybind11::arg("usage"));
    m.def("getNumFields", &getNumFields,
        "Get the total number of fields", 
        pybind11::arg("dataset"), pybind11::arg("band"));
    m.def("getFieldByName", &getFieldByName,
        "Return the named field",
        pybind11::arg("dataset"), pybind11::arg("band"), pybind11::arg("name"));
    m.def("getFieldByIdx", &getFieldByIdx,
        "Return the named index",
        pybind11::arg("dataset"), pybind11::arg("band"), pybind11::arg("idx"));
    m.def("getSize", &getSize,
        "Get the size of the RAT",
        pybind11::arg("dataset"), pybind11::arg("band"));
    m.def("addRows", &addRows,
        "Add rows to the RAT",
        pybind11::arg("dataset"), pybind11::arg("band"), pybind11::arg("nrows"));
    m.def("getField", &getField, 
        "Read rows of a column",
        pybind11::arg("dataset"), pybind11::arg("band"), pybind11::arg("field"),
        pybind11::arg("startfid"), pybind11::arg("len"));
    m.def("setField", &setField, 
        "Write rows of a column",
        pybind11::arg("dataset"), pybind11::arg("band"), pybind11::arg("field"),
        pybind11::arg("startfid"), pybind11::arg("data"));
    m.def("getSpatialInfo", &getSpatialInfo,
        "Get the KEAImageSpatialInfo object for the image", pybind11::arg("dataset"));
    m.def("getImageBandDataType", &getImageBandDataType,
        "Get the datatype of the band",
        pybind11::arg("dataset"), pybind11::arg("band"));
    m.def("getNumOfImageBands", &getNumOfImageBands, pybind11::arg("dataset"));
    m.def("getNoDataValue", &getNoDataValue, pybind11::arg("dataset"),
        pybind11::arg("band"));
    m.def("getImageBlock", &getImageBlock, "Get a block of data from the band",
        pybind11::arg("dataset"), pybind11::arg("band"), pybind11::arg("col"),
        pybind11::arg("row"), pybind11::arg("xsize"), pybind11::arg("ysize"));
        
    pybind11::class_<NeighbourAccumulator>(m, "NeighbourAccumulator")
        .def(pybind11::init<pybind11::array&, pybind11::object&, uint32_t>(),
            "Construct a NeighbourAccumulator to find all the neighbours "
            "of pixels of arrays passed to addArray(). hist should be a "
            "histogram of the imagery data. The neighbours are written "
            "to the RAT of the given band of the dataset.",
            pybind11::arg("hist"), pybind11::arg("dataset"), pybind11::arg("band"))
        .def(pybind11::init<pybind11::array&, pybind11::object&, uint32_t, bool>(),
            "Like the other constructor, but fourConnected controls whether there "
            "are 4 or 8 neighbours for a pixel.",
            pybind11::arg("hist"), pybind11::arg("dataset"), pybind11::arg("band"),
            pybind11::arg("fourConnected"))
        .def("addArray", &NeighbourAccumulator::addArray, 
            "Process a 2D array (from an image) adding all neighbours found. "
            "Very important: a one pixel overlap should be given between "
            "multiple tiles. Around the outside of the image, the ignore value "
            "should be used to fill in. ",
            pybind11::arg("array"));
            
    pybind11::class_<kealib::KEAATTField>(m, "KEAATTField")
        .def(pybind11::init<>())
        .def("__repr__", 
            [](const kealib::KEAATTField &f)
            {
                std::ostringstream stringStream;
                stringStream << "<extrat.KEAATTField name=\"";
                stringStream << f.name;
                stringStream << "\" dataType=";
                stringStream << f.dataType;
                stringStream << " idx=";
                stringStream << f.idx;
                stringStream << " usage=\"";
                stringStream << f.usage;
                stringStream << "\" colNum=";
                stringStream << f.colNum;
                stringStream << ">";
                return stringStream.str();
            })
        .def_readwrite("name", &kealib::KEAATTField::name)
        .def_readwrite("dataType", &kealib::KEAATTField::dataType)
        .def_readwrite("idx", &kealib::KEAATTField::idx)
        .def_readwrite("usage", &kealib::KEAATTField::usage)
        .def_readwrite("colNum", &kealib::KEAATTField::colNum);
        
    pybind11::enum_<kealib::KEAFieldDataType>(m, "KEAFieldDataType")
        .value("NA", kealib::KEAFieldDataType::kea_att_na)
        .value("Bool", kealib::KEAFieldDataType::kea_att_bool)
        .value("Int", kealib::KEAFieldDataType::kea_att_int)
        .value("Float", kealib::KEAFieldDataType::kea_att_float)
        .value("String", kealib::KEAFieldDataType::kea_att_string)
        .export_values();

    pybind11::enum_<kealib::KEADataType>(m, "KEADataType")
        .value("undefined", kealib::kea_undefined)
        .value("t8int", kealib::kea_8int)
        .value("t16int", kealib::kea_16int)
        .value("t32int", kealib::kea_32int)
        .value("t64int", kealib::kea_64int)
        .value("t8uint", kealib::kea_8uint)
        .value("t16uint", kealib::kea_16uint)
        .value("t32uint", kealib::kea_32uint)
        .value("t64uint", kealib::kea_64uint)
        .value("t32float", kealib::kea_32float)
        .value("t64float", kealib::kea_64float)
        .export_values();
        
    pybind11::class_<kealib::KEAImageSpatialInfo>(m, "KEAImageSpatialInfo")
        .def(pybind11::init<>())
        .def("__repr__",
            [](const kealib::KEAImageSpatialInfo &s)
            {
                 std::ostringstream stringStream;
                 stringStream << "<extrat.KEAImageSpatialInfo wktString=\"";
                 stringStream << s.wktString;
                 stringStream << "\" tlX=";
                 stringStream << s.tlX;
                 stringStream << " tlY=";
                 stringStream << s.tlY;
                 stringStream << " xRes=";
                 stringStream << s.xRes;
                 stringStream << " yRes=";
                 stringStream << s.yRes;
                 stringStream << " xRot=";
                 stringStream << s.xRot;
                 stringStream << " yRot=";
                 stringStream << s.yRot;
                 stringStream << " xSize=";
                 stringStream << s.xSize;
                 stringStream << " ySize=";
                 stringStream << s.ySize;
                 stringStream << ">";
                 return stringStream.str();            
            })
        .def_readwrite("wktString", &kealib::KEAImageSpatialInfo::wktString)
        .def_readwrite("tlX", &kealib::KEAImageSpatialInfo::tlX)
        .def_readwrite("tlY", &kealib::KEAImageSpatialInfo::tlY)
        .def_readwrite("xRes", &kealib::KEAImageSpatialInfo::xRes)
        .def_readwrite("yRes", &kealib::KEAImageSpatialInfo::yRes)
        .def_readwrite("xRot", &kealib::KEAImageSpatialInfo::xRot)
        .def_readwrite("yRot", &kealib::KEAImageSpatialInfo::yRot)
        .def_readwrite("xSize", &kealib::KEAImageSpatialInfo::xSize)
        .def_readwrite("ySize", &kealib::KEAImageSpatialInfo::ySize);

    pybind11::register_exception<PyKeaLibException>(m, "KeaLibException");
}

