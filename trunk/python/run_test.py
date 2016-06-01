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
from osgeo import gdal
from kealib import neighbours

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
    values = numpy.arange(N_VALUES, dtype=numpy.integer)
    rat.WriteArray(values, 0)

    return ds

def testList(ds):
    """
    Tests the ability to read and write lists of neighbours
    """
    neighbourData = [[2, 3, 1], [5], [8, 0, 2], [6, 7,2]]
    neighbours.setNeighbours(ds, 1, 0, neighbourData)

    readData = neighbours.getNeighbours(ds, 1, 0, 4)
    if readData != neighbourData:
        raise SystemExit("Data doesn't match")

def testArray(ds):
    """
    Tests the ability to read and write numpy masked arrays
    """
    # create a masked array to test
    numpyNeighboursData = numpy.array([[8, 7, 0], [9, 0, 0], [1, 4, 3], 
                        [7, 0, 0]])
    numpyNeighboursMask = numpy.array([[False, False, True], [False, True, True], 
                    [False, False, False], [False, True, True]])
    numpyNeighbours = numpy.ma.MaskedArray(numpyNeighboursData, 
                    mask=numpyNeighboursMask)
    # set it
    neighbours.setNeighbours(ds, 1, 4, numpyNeighbours)

    # get it
    readData, readMask = neighbours.getNeighbours(ds, 1, 4, 4, 
                    neighbours.NEIGHBOURS_ARRAY)

    # turn back into a masked array
    numpyNeighboursOut = numpy.ma.MaskedArray(readData, mask=readMask)
    if not (numpyNeighbours == numpyNeighboursOut).all():
        raise SystemExit("Array Data doesn't match")

def doTests():
    """
    Main function
    """
    ds = setupFile()
    testList(ds)
    testArray(ds)
