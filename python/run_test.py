#!/usr/bin/env python

#  Permission is hereby granted, free of charge, to any person 
#  obtaining a copy of this software and associated documentation 
#  files (the "Software"), to deal in the Software without restriction, 
#  including without limitation the rights to use, copy, modify, 
#  merge, publish, distribute, sublicense, and/or sell copies of the 
#  Software, and to permit persons to whom the Software is furnished 
#  to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be 
#  included in all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, 
#  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES 
#  OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. 
#  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR 
#  ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF 
#  CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
#  WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

"""
Test script for kealib Neighbours python bindings
"""
from __future__ import print_function
import os
import numpy
import awkward
from osgeo import gdal
import build.pykealib

TESTFILE = 'test.kea'
N_VALUES = 8

def setupFile():
    """
    Sets up a test file we can use and creates a single RAT column.
    Returns a GDAL dataset.
    """
    if os.path.exists(TESTFILE):
        os.remove(TESTFILE)

    driver = gdal.GetDriverByName('KEA')
    ds = driver.Create(TESTFILE, 100, 100, 1, gdal.GDT_Byte)
    band = ds.GetRasterBand(1)
    band.SetMetadataItem('LAYER_TYPE', 'thematic')

    rat = band.GetDefaultRAT()
    rat.CreateColumn('Values', gdal.GFT_Integer, gdal.GFU_Generic)
    rat.SetRowCount(N_VALUES)
    values = numpy.arange(N_VALUES, dtype=numpy.int32)
    rat.WriteArray(values, 0)

    return ds

def testAwkward(ds):
    """
    Tests the ability to read and write awkward array of neighbours
    """
    builder = awkward.ArrayBuilder()
    builder.begin_list()
    builder.integer(2)
    builder.integer(3)
    builder.integer(1)
    builder.end_list()
    
    builder.begin_list()
    builder.integer(5)
    builder.end_list()
    
    builder.begin_list()
    builder.integer(8)
    builder.integer(0)
    builder.integer(2)
    builder.end_list()

    builder.begin_list()
    builder.integer(6)
    builder.integer(7)
    builder.integer(2)
    builder.end_list()
    
    neighbourData = builder.snapshot()
    
    build.pykealib.setNeighbours(ds, 1, 0, neighbourData.layout)

    readData = build.pykealib.getNeighbours(ds, 1, 0, 4)
    
    if awkward.any(readData != neighbourData):
        raise SystemExit("Data doesn't match")

def doTests():
    """
    Main function
    """
    ds = setupFile()
    testAwkward(ds)

if __name__ == '__main__':
    doTests()
    
