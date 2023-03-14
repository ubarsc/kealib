/*
 *  fakegdal.cpp
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

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include "libkea/KEAImageIO.h"

const char szKEA[] = "KEA";

extern "C"
{
    kealib::KEAImageIO *GDALGetDatasetDriver(kealib::KEAImageIO *);
    const char *GDALGetDescription(kealib::KEAImageIO *);
    kealib::KEAImageIO *GDALGetInternalHandle(kealib::KEAImageIO *, void*);
    int IsFakeGDAL();
}

// matches layout of SWIG structure.
typedef struct {
    PyObject_HEAD
    kealib::KEAImageIO* pImageIO;
} FakeGDALObject;

static int fakeGDAL_init(FakeGDALObject *self, PyObject *args, PyObject *kwds)
{
    const char *pszFilename;
    if (!PyArg_ParseTuple(args, "s", &pszFilename))
        return -1;
    
    self->pImageIO = new kealib::KEAImageIO();
    try
    {
        H5::H5File *pH5File = kealib::KEAImageIO::openKeaH5RW( pszFilename );
        self->pImageIO->openKEAImageHeader(pH5File);
    }
    catch (const kealib::KEAIOException &e)
    {
        delete self->pImageIO;
        self->pImageIO = nullptr;
        PyErr_SetString(PyExc_RuntimeError, "Unable to open file");
        return -1;
    }
    return 0;
}

static void fakeGDAL_dealloc(FakeGDALObject *self)
{
    if( self->pImageIO != nullptr )
    {
        self->pImageIO->close();
        delete self->pImageIO;
    }
    Py_TYPE(self)->tp_free((PyObject *)self);
}

// fake the presence of a 'this' attribute which just returns
// self (to match GDAL)
static PyObject *fakeGDAL_this(PyObject *self, void *closure)
{
    Py_INCREF(self);
    return self;
}

// just so we can check we are getting a "fake" one
static PyObject *fakeGDAL_Is_fake(PyObject *self, void *closure)
{
    Py_RETURN_TRUE;
}

/* get/set */
static PyGetSetDef fakeGDAL_getseters[] = {
    {(char*)"this", (getter)fakeGDAL_this, NULL, NULL, NULL},
    {(char*)"is_fake", (getter)fakeGDAL_Is_fake, NULL, NULL, NULL},
    {NULL}  /* Sentinel */
};

static PyTypeObject FakeGDALDataset_Type = {
    PyVarObject_HEAD_INIT(NULL, 0)
    .tp_name = "fakegdal.Dataset",
    .tp_basicsize = sizeof(FakeGDALObject),
    .tp_dealloc = (destructor)fakeGDAL_dealloc,
    .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
    .tp_doc = PyDoc_STR("Fake GDAL Dataset Object"),
    .tp_getset = fakeGDAL_getseters,
    .tp_init = (initproc)fakeGDAL_init
};

static struct PyModuleDef FakeGDAL_Module = {
    PyModuleDef_HEAD_INIT,
    "fakegdal",
    NULL,
    -1,
    NULL
};

PyMODINIT_FUNC
PyInit_fakegdal(void)
{
    PyObject *m;

    m = PyModule_Create(&FakeGDAL_Module);
    if (m == NULL)
        return NULL;
        
    FakeGDALDataset_Type.tp_new = PyType_GenericNew;
    if( PyType_Ready(&FakeGDALDataset_Type) < 0)
        return NULL;
    
    Py_INCREF(&FakeGDALDataset_Type);
    PyModule_AddObject(m, "Dataset", (PyObject*)&FakeGDALDataset_Type);
        
    return m;
}

kealib::KEAImageIO *GDALGetDatasetDriver(kealib::KEAImageIO *pImageIO)
{
    return pImageIO;
}

const char *GDALGetDescription(kealib::KEAImageIO *)
{
    return szKEA;
}

kealib::KEAImageIO *GDALGetInternalHandle(kealib::KEAImageIO *pImageIO, void *p)
{
    return pImageIO;
}

int IsFakeGDAL()
{
    return 1;
}
