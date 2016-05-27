/*
 *  neighbours.cpp
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

#include <vector>
#include <Python.h>
#include "gdal_priv.h"
#include "libkea/KEAImageIO.h"

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
 GDALRasterAttributeTableShadow which is actually a pointer
 to a SwigPyObject whose ptr field is a pointer to a
 GDALRasterAttributeTable. Phew. 
 given a python object this function returns the underlying
 pointer. Returns NULL on failure (exception string already set)*/
void *getUnderlyingPtrFromSWIGPyObject(PyObject *pObj, PyObject *pException)
{
    PyObject *pThisAttr; /* the 'this' field */
    SwigPyObject *pSwigThisAttr;
    void *pUnderlying;

    pThisAttr = PyObject_GetAttrString(pObj, "this");
    if( pThisAttr == NULL )
    {
        PyErr_SetString(pException, "object does not appear to be a swig type");
        return NULL;
    }

    /* i think it is safe to do this since pObj is still around*/
    Py_DECREF(pThisAttr);

    /* convert this to a SwigPyObject*/
    pSwigThisAttr = (SwigPyObject*)pThisAttr;

    /* get the ptr field*/
    pUnderlying = pSwigThisAttr->ptr;
    if( pUnderlying == NULL )
    {
        PyErr_SetString(pException, "underlying object is NULL");
        return NULL;
    }

    return pUnderlying;
}


/* An exception object for this module */
/* created in the init function */
struct NeighboursState
{
    PyObject *error;
};

#if PY_MAJOR_VERSION >= 3
#define GETSTATE(m) ((struct NeighboursState*)PyModule_GetState(m))
#else
#define GETSTATE(m) (&_state)
static struct NeighboursState _state;
#endif

void freeNeighbourLists(std::vector<std::vector<size_t>* > *pNeighbours)
{
    /* frees all the sub vectors of pNeighbours */
    for( std::vector<std::vector<size_t>* >::iterator itr = pNeighbours->begin();
            itr != pNeighbours->end(); itr++)
    {
        delete *itr;
    }

}

static PyObject *neighbours_setNeighbours(PyObject *self, PyObject *args)
{
    PyObject *pPythonDataset; /* gdal.Dataset */
    int nBand;
    Py_ssize_t startfid;
    PyObject *pSequence;

    if( !PyArg_ParseTuple(args, "OinO:setNeighbours", &pPythonDataset, &nBand, &startfid, &pSequence))
        return NULL;

    if( !PySequence_Check(pSequence))
    {
        PyErr_SetString(GETSTATE(self)->error, "4th argument must be a sequence");
        return NULL;
    }

    void *pPtr = getUnderlyingPtrFromSWIGPyObject(pPythonDataset, GETSTATE(self)->error);
    if( pPtr == NULL )
        return NULL;

    GDALDataset *pDataset = (GDALDataset*)pPtr;

    GDALDriver *pDriver = pDataset->GetDriver();
    const char *pszName = pDriver->GetDescription();
    if( (strlen(pszName) != 3) || (pszName[0] != 'K') || (pszName[1] != 'E') || (pszName[2] != 'A'))
    {
        PyErr_SetString(GETSTATE(self)->error, "This function only works on KEA files");
        return NULL;
    }

    kealib::KEAImageIO *pImageIO = (kealib::KEAImageIO*)pDataset->GetInternalHandle(NULL);
    if( pImageIO == NULL )
    {
        PyErr_SetString(GETSTATE(self)->error, "GetInternalHandle returned NULL");
        return NULL;
    }

    try
    {
        kealib::KEAAttributeTable *pRAT = pImageIO->getAttributeTable(kealib::kea_att_file, nBand);
        if( pRAT == NULL )
        {
            PyErr_SetString(GETSTATE(self)->error, "No Attribute table in this file");
            return NULL;
        }

        Py_ssize_t nSeqLength = PySequence_Length(pSequence);
        std::vector<std::vector<size_t>* > neighbours(nSeqLength);
        for( Py_ssize_t n = 0; n < nSeqLength; n++)
        {
            PyObject *pSubSeq = PySequence_GetItem(pSequence, n);
            if( ( pSubSeq == NULL ) || (!PySequence_Check(pSubSeq)) )
            {
                PyErr_SetString(GETSTATE(self)->error, "All elements in sequence must also be a sequence");
                Py_XDECREF(pSubSeq);
                freeNeighbourLists(&neighbours);
                return NULL;
            }

            Py_ssize_t nSubSeqLength = PySequence_Length(pSubSeq);
            std::vector<size_t> *pVec = new std::vector<size_t>(nSubSeqLength);
            for( Py_ssize_t i = 0; i < nSubSeqLength; i++ )
            {
                PyObject *pElement = PySequence_GetItem(pSubSeq, i);
                if( (pElement == NULL) || (!PyLong_Check(pElement)) )
                {
                    PyErr_SetString(GETSTATE(self)->error, "All elements in the subsequence must be ints");
                    Py_XDECREF(pElement);
                    return NULL;
                }
                pVec->at(i) = PyLong_AsLong(pElement);
                Py_DECREF(pElement);
            }
            neighbours.at(n) = pVec;

            Py_DECREF(pSubSeq);
        }

        pRAT->setNeighbours(startfid, neighbours.size(), &neighbours);
        freeNeighbourLists(&neighbours);
    }
    catch(kealib::KEAException &e)
    {
        PyErr_Format(GETSTATE(self)->error, "Error from libkea: %s", e.what());
        return NULL;
    }

    Py_RETURN_NONE;
}

static PyObject *neighbours_getNeighbours(PyObject *self, PyObject *args)
{
    PyObject *pPythonDataset; /* gdal.Dataset */
    int nBand;
    Py_ssize_t startfid, length;

    if( !PyArg_ParseTuple(args, "Oinn:getNeighbours", &pPythonDataset, &nBand, &startfid, &length))
        return NULL;

    void *pPtr = getUnderlyingPtrFromSWIGPyObject(pPythonDataset, GETSTATE(self)->error);
    if( pPtr == NULL )
        return NULL;

    GDALDataset *pDataset = (GDALDataset*)pPtr;

    GDALDriver *pDriver = pDataset->GetDriver();
    const char *pszName = pDriver->GetDescription();
    if( (strlen(pszName) != 3) || (pszName[0] != 'K') || (pszName[1] != 'E') || (pszName[2] != 'A'))
    {
        PyErr_SetString(GETSTATE(self)->error, "This function only works on KEA files");
        return NULL;
    }

    kealib::KEAImageIO *pImageIO = (kealib::KEAImageIO*)pDataset->GetInternalHandle(NULL);
    if( pImageIO == NULL )
    {
        PyErr_SetString(GETSTATE(self)->error, "GetInternalHandle returned NULL");
        return NULL;
    }

    PyObject *pList = NULL;
    try
    {
        kealib::KEAAttributeTable *pRAT = pImageIO->getAttributeTable(kealib::kea_att_file, nBand);
        if( pRAT == NULL )
        {
            PyErr_SetString(GETSTATE(self)->error, "No Attribute table in this file");
            return NULL;
        }

        /* Read the neighbours */
        std::vector<std::vector<size_t>* > neighbours;
        pRAT->getNeighbours(startfid, length, &neighbours);

        pList = PyList_New(neighbours.size());
        for( size_t n = 0; n < neighbours.size(); n++ )
        {
            std::vector<size_t>* pSubVect = neighbours.at(n);
            PyObject *pSubList = PyList_New(pSubVect->size());
            for( size_t i = 0; i < pSubVect->size(); i++ )
            {
                PyObject *pVal = PyLong_FromLong(pSubVect->at(i));
                PyList_SetItem(pSubList, i, pVal);
            }
            PyList_SetItem(pList, n, pSubList);
        }

        freeNeighbourLists(&neighbours);
    }
    catch(kealib::KEAException &e)
    {
        PyErr_Format(GETSTATE(self)->error, "Error from libkea: %s", e.what());
        return NULL;
    }

    return pList;
}

/* Our list of functions in this module*/
static PyMethodDef NeighboursMethods[] = {
    {"setNeighbours", neighbours_setNeighbours, METH_VARARGS, 
"set the neighbours for given feature id(s).\n"
"call signature: setNeighbours(ds, band, startfid, seq)\n"
"where:\n"
"  ds is an instance of gdal.Dataset\n"
"  band is the band number (1-based)\n"
"  startfid is the feature to start with\n"
"  seq is a sequence with an element for each feature to be set.\n"
"    each element must be a sequence.\n"},
    {"getNeighbours", neighbours_getNeighbours, METH_VARARGS,
"get the neighbours for given feature id(s).\n"
"call signature: getNeighbours(ds, band, startfid, len)\n"
"where:\n"
"  ds is an instance of gdal.Dataset\n"
"  band is the band number (1-based)\n"
"  startfid is the feature to start with\n"
"  len is the number of features to read\n"
"A list of lists is returned.\n"},
    {NULL}        /* Sentinel */
};

#if PY_MAJOR_VERSION >= 3

static int neighbours_traverse(PyObject *m, visitproc visit, void *arg) 
{
    Py_VISIT(GETSTATE(m)->error);
    return 0;
}

static int neighbours_clear(PyObject *m) 
{
    Py_CLEAR(GETSTATE(m)->error);
    return 0;
}

static struct PyModuleDef moduledef = {
        PyModuleDef_HEAD_INIT,
        "neighbours",
        NULL,
        sizeof(struct NeighboursState),
        NeighboursMethods,
        NULL,
        neighbours_traverse,
        neighbours_clear,
        NULL
};

#define INITERROR return NULL

PyMODINIT_FUNC 
PyInit_neighbours(void)

#else
#define INITERROR return

PyMODINIT_FUNC
initneighbours(void)
#endif
{
    PyObject *pModule;
    struct NeighboursState *state;

#if PY_MAJOR_VERSION >= 3
    pModule = PyModule_Create(&moduledef);
#else
    pModule = Py_InitModule("neighbours", NeighboursMethods);
#endif
    if( pModule == NULL )
        INITERROR;

    state = GETSTATE(pModule);

    /* Create and add our exception type */
    state->error = PyErr_NewException("neighbours.error", NULL, NULL);
    if( state->error == NULL )
    {
        Py_DECREF(pModule);
        INITERROR;
    }
    PyModule_AddObject(pModule, "error", state->error);

#if PY_MAJOR_VERSION >= 3
    return pModule;
#endif
}
