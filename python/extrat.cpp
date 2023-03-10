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
#include "numpy/arrayobject.h"

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
// set to values in SetFunctionPointers() (called from the init function)
void* (*pGetDriver)(void*) = nullptr;
const char * (*pGetDescription)(void*) = nullptr;
void *(*pGetInternalHandle)(void *, const char*) = nullptr;

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
// given a dataset and band number.
kealib::KEAImageIO *getImageIOFromDataset(pybind11::object &dataset, int nBand)
{
    void *pPtr = getUnderlyingPtrFromSWIGPyObject(dataset);
    
    // now do the actual calls with our function pointers (set on import) 
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
    auto from_buffers = pybind11::module::import("awkward").attr("from_buffers");

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
pybind11::object getNeighbours(pybind11::object &dataset, int nBand, int &startfid, int &length)
{
    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset, nBand);
    
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

        for( size_t n = 0; n < neighbours.size(); n++ )
        {
            auto& subbuilder = builder.begin_list();
            
            std::vector<size_t>* pSubVect = neighbours.at(n);
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
    auto offsetsArray = offsets.cast<pybind11::array_t<T>>();
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
void setNeighbours(pybind11::object &dataset, int nBand, 
        int startfid, pybind11::object &neighbours)
{
    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset, nBand);

    try
    {
        kealib::KEAAttributeTable *pRAT = pImageIO->getAttributeTable(kealib::kea_att_file, nBand);
        if( pRAT == nullptr )
        {
            throw PyKeaLibException("No Attribute table in this file");
        }


        auto to_buffers = pybind11::module::import("awkward").attr("to_buffers");
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

// Function to add a boolean column to the RAT of a KEA file.
// GDAL doesn't support this, but will provide any existing bool columns
// as integers, converting as it goes. 
// The user will need to close the dataset and open again to see the
// new column
void addBoolField(pybind11::object &dataset, int nBand, 
        const std::string& name, bool bInitVal, const std::string &usage)
{
    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset, nBand);

    try
    {
        kealib::KEAAttributeTable *pRAT = pImageIO->getAttributeTable(kealib::kea_att_file, nBand);
        if( pRAT == nullptr )
        {
            throw PyKeaLibException("No Attribute table in this file");
        }
        
        pRAT->addAttBoolField(name, bInitVal, usage);

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
                    int nBand, bool fourConnected=true);
    ~NeighbourAccumulator();
    
    void addArray(pybind11::array &arr);
    
private:
    // templated member function to find all neighbours from given 
    // 2d numby array. Type should match that of the numpy array.
    template<typename T>
    void addNeighbours(PyArrayObject *pInput)
    {
        T nTypeIgnore = (T)m_ignore;
        npy_intp nYSize = PyArray_DIM(pInput, 0);
        npy_intp nXSize = PyArray_DIM(pInput, 1);
        
        // note: starting/ending one pixel in
        // so overlap of one must be selected
        // otherwise everything will be out of sync with the
        // histogram.
        for( npy_intp y = 1; y < (nYSize-1); y++ )
        {
            for( npy_intp x = 1; x < (nXSize-1); x++ )
            {
                T val = *(T*)PyArray_GETPTR2(pInput, y, x);
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
                    for( npy_intp x_off = -1; x_off < 2; x_off++ )
                    {
                        for( npy_intp y_off = -1; y_off < 2; y_off++ )
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
                                    npy_intp x_idx = x + x_off;
                                    npy_intp y_idx = y + y_off;
                                    T other_val = *(T*)PyArray_GETPTR2(pInput, y_idx, x_idx);
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
    void copyHistogram(PyArrayObject *pInput)
    {
        npy_intp nSize = PyArray_DIM(pInput, 0);
        m_pHistogram = new npy_uintp[nSize];
        for( npy_intp i = 0; i < nSize; i++ )
        {
            T val = *(T*)PyArray_GETPTR1(pInput, i);
            m_pHistogram[i] = val;
        }
    }
    
    // map keyed on image values where the value is a vector of neighbours
    std::unordered_map<size_t, std::vector<size_t>* > m_neighbourMap;
    // refernce to the RAT 
    kealib::KEAAttributeTable *m_pRAT;
    // our copy of the user supplied histogram
    npy_uintp *m_pHistogram;
    // the ignore value obtained from the file
    double m_ignore;
    // whether to consider just 4 neighbours or 8.
    bool m_fourConnected;
};

// Constructor. Copy the histogram into our array and obtain 
// a reference to the RAT and get the ignore value
NeighbourAccumulator::NeighbourAccumulator(pybind11::array &hist, 
                    pybind11::object &dataset, 
                    int nBand, bool fourConnected/*=true*/)
    : m_fourConnected(fourConnected)
{
    PyArrayObject *pInput = reinterpret_cast<PyArrayObject*>(hist.ptr());
    int arrayType = PyArray_TYPE(pInput);
    
    // TODO: is it wise to always copy the histogram, or is there
    // a better way?
    switch(arrayType)
    {
        case NPY_INT8:
            copyHistogram <npy_int8> (pInput);
            break;
        case NPY_UINT8:
            copyHistogram <npy_uint8> (pInput);
            break;
        case NPY_INT16:
            copyHistogram <npy_int16> (pInput);
            break;
        case NPY_UINT16:
            copyHistogram <npy_uint16> (pInput);
            break;
        case NPY_INT32:
            copyHistogram <npy_int32> (pInput);
            break;
        case NPY_UINT32:
            copyHistogram <npy_uint32> (pInput);
            break;
        case NPY_INT64:
            copyHistogram <npy_int64> (pInput);
            break;
        case NPY_UINT64:
            copyHistogram <npy_uint64> (pInput);
            break;
        case NPY_FLOAT16:
            copyHistogram <npy_float16> (pInput);
            break;
        case NPY_FLOAT32:
            copyHistogram <npy_float32> (pInput);
            break;
        case NPY_FLOAT64:
            copyHistogram <npy_float64> (pInput);
            break;
        default:
            throw PyKeaLibException("Unsupported data type");
            break;
    }    

    kealib::KEAImageIO *pImageIO = getImageIOFromDataset(dataset, nBand);
    
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

    PyArrayObject *pInput = reinterpret_cast<PyArrayObject*>(arr.ptr());
    
    int arrayType = PyArray_TYPE(pInput);
    
    switch(arrayType)
    {
        case NPY_INT8:
            addNeighbours <npy_int8> (pInput);
            break;
        case NPY_UINT8:
            addNeighbours <npy_uint8> (pInput);
            break;
        case NPY_INT16:
            addNeighbours <npy_int16> (pInput);
            break;
        case NPY_UINT16:
            addNeighbours <npy_uint16> (pInput);
            break;
        case NPY_INT32:
            addNeighbours <npy_int32> (pInput);
            break;
        case NPY_UINT32:
            addNeighbours <npy_uint32> (pInput);
            break;
        case NPY_INT64:
            addNeighbours <npy_int64> (pInput);
            break;
        case NPY_UINT64:
            addNeighbours <npy_uint64> (pInput);
            break;
            
        // note: don't support float types
        default:
            throw PyKeaLibException("Unsupported data type");
            break;
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

#else
    char *error;
    void *libHandle = dlopen(pszLibName, RTLD_LAZY);
    if( !libHandle )
    {
        std::ostringstream stringStream;
        stringStream << "Unable to open ";
        stringStream << pszLibName;
        stringStream << " set the KEALIB_LIBGDAL env var to override";
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
#endif
}

// Main mondule initialisation

PYBIND11_MODULE(extrat, m) {
    // Ensure dependencies are loaded.
    // TODO: currently we need to ensure this is imported in Python
    // otherwise link failures happen...
    pybind11::module::import("awkward");
    
    m.doc() = "Module that contains helpers for accessing kealib features not available from GDAL";

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
    m.def("addBoolField", &addBoolField,
        "Add a boolean column for the given dataset band. "
        "GDAL doesn't support creation of boolean columns, but "
        "presents boolean columns as integers and allows you to "
        "read and write them converting to/from integers. "
        "You will have to re-open the dataset with GDAL to see "
        "the new column.", pybind11::arg("dataset"), pybind11::arg("band"),
        pybind11::arg("name"), pybind11::arg("initval"), pybind11::arg("usage"));
        
    pybind11::class_<NeighbourAccumulator>(m, "NeighbourAccumulator")
        .def(pybind11::init<pybind11::array&, pybind11::object&, int>(),
            "Construct a NeighbourAccumulator to find all the neighbours "
            "of pixels of arrays passed to addArray(). hist should be a "
            "histogram of the imagery data. The neighbours are written "
            "to the RAT of the given band of the dataset.",
            pybind11::arg("hist"), pybind11::arg("dataset"), pybind11::arg("band"))
        .def(pybind11::init<pybind11::array&, pybind11::object&, int, bool>(),
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

    pybind11::register_exception<PyKeaLibException>(m, "KeaLibException");
    
    SetFunctionPointers();
}

