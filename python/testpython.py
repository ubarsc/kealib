#!/usr/bin/env python

"""
Test harness for kealib.extrat
"""

import subprocess
import awkward

from kealib import extrat
from kealib import build_neighbours
import fakegdal

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
    assert size == 100 # set in the C++ code testseg.cpp
     
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
            # column of a type. Already had an Int colunm (histogram)
            break
        for d in data:
            assert d == initVal
        

def test():
    # First create our test file
    # Currently this is only done in C++ but could be altered in future
    subprocess.check_call(['./testseg'])
    
    ds = fakegdal.Dataset('segs.kea')

    testBuildNeighbours(ds)  
    
    testAddNeighbours(ds) 
    
    testAddField(ds)

if __name__ == '__main__':
    test()
