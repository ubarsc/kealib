/*
 *  pykealib.cpp
 *  LibKEA
 *
 *  Created by Pete Bunting on 02/07/2012.
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
 
#include <string>
#include <exception>

#include <pybind11/pybind11.h>

#include "awkward/builder/ArrayBuilder.h"
#include "awkward/builder/ArrayBuilderOptions.h"
#include "awkward/kernels.h"
#include "awkward/array/NumpyArray.h"

#include "gdal_priv.h"
#include "libkea/KEAImageIO.h"

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
 pointer. Returns NULL on failure (exception string already set)*/
void *getUnderlyingPtrFromSWIGPyObject(pybind11::object &input/*, PyObject *pException*/)
{
    // get the "this" attribute
    auto thisAttr = input.attr("this");

    /* convert this to a SwigPyObject*/
    /* Need a C style cast here - nothing else works */
    SwigPyObject *pSwigThisAttr = (SwigPyObject*)thisAttr.ptr();

    /* get the ptr field*/
    void *pUnderlying = pSwigThisAttr->ptr;
    if( pUnderlying == NULL )
    {
        throw PyKeaLibException("Underlying Pointer is NULL");
    }

    return pUnderlying;
}

void freeNeighbourLists(std::vector<std::vector<size_t>* > *pNeighbours)
{
    /* frees all the sub vectors of pNeighbours */
    for( std::vector<std::vector<size_t>* >::iterator itr = pNeighbours->begin();
            itr != pNeighbours->end(); itr++)
    {
        delete *itr;
    }
}

awkward::ContentPtr getNeighbours(pybind11::object &dataset, int nBand, int &startfid, int &length)
{
    void *pPtr = getUnderlyingPtrFromSWIGPyObject(dataset);

    // cast - and hope!
    GDALDataset *pDataset = (GDALDataset*)pPtr;

    GDALDriver *pDriver = pDataset->GetDriver();
    const char *pszName = pDriver->GetDescription();
    if( (strlen(pszName) != 3) || (pszName[0] != 'K') || (pszName[1] != 'E') || (pszName[2] != 'A'))
    {
        throw PyKeaLibException("This function only works on KEA files");
    }

    kealib::KEAImageIO *pImageIO = (kealib::KEAImageIO*)pDataset->GetInternalHandle(NULL);
    if( pImageIO == NULL )
    {
        throw PyKeaLibException("GetInternalHandle returned NULL");
    }
    
    awkward::ArrayBuilder builder(awkward::ArrayBuilderOptions(1024, 2.0));

    try
    {
        kealib::KEAAttributeTable *pRAT = pImageIO->getAttributeTable(kealib::kea_att_file, nBand);
        if( pRAT == NULL )
        {
            throw PyKeaLibException("No Attribute table in this file");
        }

        /* Read the neighbours */
        std::vector<std::vector<size_t>* > neighbours;
        pRAT->getNeighbours(startfid, length, &neighbours);

        for( size_t n = 0; n < neighbours.size(); n++ )
        {
            builder.beginlist();
            
            std::vector<size_t>* pSubVect = neighbours.at(n);
            for( size_t i = 0; i < pSubVect->size(); i++ )
            {
                builder.integer(pSubVect->at(i));
            }
            
            builder.endlist();
        }
        
        freeNeighbourLists(&neighbours);
    }
    catch(kealib::KEAException &e)
    {
        throw PyKeaLibException(e.what());
    }

    return builder.snapshot();
}

void setNeighbours(pybind11::object &dataset, int nBand, int startfid, awkward::ContentPtr &neighbours)
{
    void *pPtr = getUnderlyingPtrFromSWIGPyObject(dataset);

    // cast - and hope!
    GDALDataset *pDataset = (GDALDataset*)pPtr;

    GDALDriver *pDriver = pDataset->GetDriver();
    const char *pszName = pDriver->GetDescription();
    if( (strlen(pszName) != 3) || (pszName[0] != 'K') || (pszName[1] != 'E') || (pszName[2] != 'A'))
    {
        throw PyKeaLibException("This function only works on KEA files");
    }

    kealib::KEAImageIO *pImageIO = (kealib::KEAImageIO*)pDataset->GetInternalHandle(NULL);
    if( pImageIO == NULL )
    {
        throw PyKeaLibException("GetInternalHandle returned NULL");
    }

    try
    {
        kealib::KEAAttributeTable *pRAT = pImageIO->getAttributeTable(kealib::kea_att_file, nBand);
        if( pRAT == NULL )
        {
            throw PyKeaLibException("No Attribute table in this file");
        }

        int64_t length = neighbours.get()->length();
        std::vector<std::vector<size_t>* > cppneighbours(length);
        for( int64_t n = 0; n < length; n++ )
        {
            awkward::ContentPtr row = neighbours.get()->getitem_at_nowrap(n);
            if( row.get()->classname() != "NumpyArray" ) 
            {
                // this always *seems* to be a NumpyArray class (which is unrelated
                // to an actual numpy array - don't ask), but it appears
                // to be other options. Just do a check for now, and extend this
                // if required.
                throw PyKeaLibException("Currently only handle NumpyArray for rows");
            }
            
            int64_t rowLength = row.get()->length();
            
            std::vector<size_t> *pVec = new std::vector<size_t>(rowLength);
            
            for( int64_t i = 0; i < rowLength; i++ ) 
            {
                awkward::ContentPtr el = row.get()->getitem_at_nowrap(i);
                // if the 'row' was a NumpyArray an element should is a scalar
                // NumpyArray (don't ask).
                awkward::NumpyArray *elnp = dynamic_cast<awkward::NumpyArray*>(el.get());
                if( !elnp->isscalar() )
                {
                    // they must have given us more than a 2d awkward array. Bail.
                    throw PyKeaLibException("Only support 2D awkward arrays");
                }
                    
                // this is all very strange but seems to work...
                size_t val = 0;    
                switch(elnp->dtype())
                {
                    case awkward::util::dtype::int8:
                        val = awkward_NumpyArray8_getitem_at0(static_cast<int8_t*>(elnp->data()));
                        break;
                    case awkward::util::dtype::uint8:
                        val = awkward_NumpyArrayU8_getitem_at0(static_cast<uint8_t*>(elnp->data()));
                        break;
                    case awkward::util::dtype::int16:
                        val = awkward_NumpyArray16_getitem_at0(static_cast<int16_t*>(elnp->data()));
                        break;
                    case awkward::util::dtype::uint16:
                        val = awkward_NumpyArrayU16_getitem_at0(static_cast<uint16_t*>(elnp->data()));
                        break;
                    case awkward::util::dtype::int32:
                        val = awkward_NumpyArray32_getitem_at0(static_cast<int32_t*>(elnp->data()));
                        break;
                    case awkward::util::dtype::uint32:
                        val = awkward_NumpyArrayU32_getitem_at0(static_cast<uint32_t*>(elnp->data()));
                        break;
                    case awkward::util::dtype::int64:
                        val = awkward_NumpyArray64_getitem_at0(static_cast<int64_t*>(elnp->data()));
                        break;
                    case awkward::util::dtype::uint64:
                        val = awkward_NumpyArrayU64_getitem_at0(static_cast<uint64_t*>(elnp->data()));
                        break;
                    case awkward::util::dtype::float32:
                        val = awkward_NumpyArrayfloat32_getitem_at0(static_cast<float*>(elnp->data()));
                        break;
                    case awkward::util::dtype::float64:
                        val = awkward_NumpyArrayfloat64_getitem_at0(static_cast<double*>(elnp->data()));
                        break;
                }
                
                pVec->at(i) = val;
            }
            
            cppneighbours.at(n) = pVec;
        }
        
        pRAT->setNeighbours(startfid, cppneighbours.size(), &cppneighbours);
        
        freeNeighbourLists(&cppneighbours);
    }
    catch(kealib::KEAException &e)
    {
        throw PyKeaLibException(e.what());
    }
}

PYBIND11_MODULE(pykealib, m) {
  // Ensure dependencies are loaded.
  pybind11::module::import("awkward");

  m.def("getNeighbours", &getNeighbours);
  m.def("setNeighbours", &setNeighbours);
  
  pybind11::register_exception<PyKeaLibException>(m, "KeaLibException");
}

