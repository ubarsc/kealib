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

if os.path.exists(TESTFILE):
    os.remove(TESTFILE)

driver = gdal.GetDriverByName('KEA')
ds = driver.Create(TESTFILE, 100, 100, 1, gdal.GDT_Byte)
band = ds.GetRasterBand(1)
band.SetMetadataItem('LAYER_TYPE', 'thematic')

rat = band.GetDefaultRAT()
rat.CreateColumn('Values', gdal.GFT_Integer, gdal.GFU_Generic)
rat.SetRowCount(4)
values = numpy.array([1, 2, 3, 4], dtype=numpy.integer)
rat.WriteArray(values, 0)

neighbourData = [[2, 3, 1], [5], [8, 0, 2], [6, 7,2]]
neighbours.setNeighbours(ds, 1, 0, neighbourData)

readData = neighbours.getNeighbours(ds, 1, 0, 4)
if readData != neighbourData:
    raise SystemExit("Data doesn't match")
