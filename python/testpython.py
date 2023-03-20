#!/usr/bin/env python

"""
 *  build_neighbours.py
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
"""

"""
Test harness for kealib.extrat
"""

import subprocess
import numpy
import awkward

from kealib import extrat
from kealib import build_neighbours
from kealib import fakegdal

INIT_RAT_SIZE = 100 # set in testseg.cpp
CORRECT_NEIGHBOURS = {0: [1, 10], 1: [0, 11], 10: [0, 11], 11: [10, 1]}
TEST_FIELDS = {'BoolField': (extrat.KEAFieldDataType.Bool, False), 
    'IntField': (extrat.KEAFieldDataType.Int, 22), 
    'FloatField': (extrat.KEAFieldDataType.Float, 3.14),
    'StringField': (extrat.KEAFieldDataType.String, "hello")}

def correctAnyOrder(one, two):
    alln = set(one)
    for el in two:
        # will raise an exception if not found
        alln.remove(el)
    return len(alln) == 0
    
def testBuildNeighbours(ds):
    """
    Test the building neighbours functionality
    
    Which also tests the reading image data.
    """
    build_neighbours.buildNeighbours(ds, 1, tilesize=8)
    
    neighbours = extrat.getNeighbours(ds, 1, 0, 12)
    for idx, n in enumerate(neighbours):
        if idx in CORRECT_NEIGHBOURS:
            assert correctAnyOrder(CORRECT_NEIGHBOURS[idx], n)
        else:
            assert len(n) == 0
            
def testAddNeighbours(ds):
    """
    See if we can write new neighbours in and get the same back
    """
    startfid = 5
    newNeighbours = awkward.from_iter([[99, 87, 65, 27], [44, 21, 98]])
    extrat.setNeighbours(ds, 1, startfid, newNeighbours)
    
    testNeighbours = extrat.getNeighbours(ds, 1, startfid, len(newNeighbours))
    
    for n1, n2 in zip(newNeighbours, testNeighbours):
        assert correctAnyOrder(n1, n2)
        
def testAddField(ds):
    """
    """
    numFields = 1 # should just be histogram there
    for fld in TEST_FIELDS:
        ty, initVal = TEST_FIELDS[fld]
        extrat.addField(ds, 1, fld, ty, initVal, "Generic")
        numFields += 1
        
    size = extrat.getSize(ds, 1)
    assert size == INIT_RAT_SIZE # set in the C++ code testseg.cpp
     
    testNumFields = extrat.getNumFields(ds, 1)
    assert numFields == numFields
    
    for f in range(numFields):
        i = extrat.getFieldByIdx(ds, 1, f)
        assert (i.name == "Histogram") or i.name in TEST_FIELDS
        
    # now see that the data is there
    for fld in TEST_FIELDS:
        ty, initVal = TEST_FIELDS[fld]
        field = extrat.getFieldByName(ds, 1, fld)
        data = extrat.getField(ds, 1, field, 0, size)
        if fld == 'IntField':
            # have to skip the initVal check as it only works on the first
            # column of a type. Already have an Int colunm (histogram)
            break
        for d in data:
            assert d == initVal
        
def testResize(ds):
    """
    Test we can resize the RAT
    """
    extrat.addRows(ds, 1, 10)
    assert extrat.getSize(ds, 1) == (INIT_RAT_SIZE + 10)
    
def checkDataElWise(d1, d2):
    """
    Check elementwise. Works for numpy and awkward arrays
    """
    assert len(d1) == len(d2)
    assert isinstance(d1, type(d2))
    for el1, el2 in zip(d1, d2):
        assert isinstance(el1, type(el2))
        assert el1 == el2
    
def testWriteRead(ds):
    """
    Test we can write to all column types and read the same data back
    """
    data = {'BoolField': numpy.array([True, False, False, True]),
        'IntField': numpy.array([11, 43, 101, -67, -3, 91]),
        'FloatField': numpy.array([3.22, 90.0, -45.3, 322.8, 0.56]),
        'StringField': awkward.from_iter(["Hello1", "Hello2", "Hello3", "d982nn"])}

    # write them all        
    for fld in data:
        field = extrat.getFieldByName(ds, 1, fld)
        towrite = data[fld]
        extrat.setField(ds, 1, field, 80, towrite)
        
    # now check
    for fld in data:
        field = extrat.getFieldByName(ds, 1, fld)
        tocheck = data[fld]
        dataOut = extrat.getField(ds, 1, field, 80, len(tocheck))
        checkDataElWise(tocheck, dataOut)
        
def testMetadata(ds):
    """
    Check the other functions for get information about the image
    """
    info = extrat.getSpatialInfo(ds)
    # these are all set to the default values by testsegs.kea
    assert info.wktString == ""
    assert info.tlX == 0
    assert info.tlY == 0
    assert info.xRes == 1
    assert info.yRes == -1
    assert info.xRot == 0
    assert info.yRot == 0
    assert info.xSize == 20
    assert info.ySize == 20
    
    assert extrat.getImageBandDataType(ds, 1) == extrat.KEADataType.t8uint
    assert extrat.getNumOfImageBands(ds) == 1
    assert extrat.getNoDataValue(ds, 1) == 90 # set in testseg.cpp

def test():
    # First create our test file
    # Currently this is only done in C++ but could be altered in future
    # But we would need to be able to create files and write to them 
    # from Python
    subprocess.check_call(['./testseg'])
    
    ds = fakegdal.Dataset('segs.kea')

    testBuildNeighbours(ds)  
    
    testAddNeighbours(ds) 
    
    testAddField(ds)
    
    testResize(ds)
    
    testWriteRead(ds)
    
    testMetadata(ds)

if __name__ == '__main__':
    test()
