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

import os
import argparse
import numpy
from kealib import extrat

DFLT_TILESIZE = 2048

KEA_TO_NUMPY = {extrat.KEADataType.t8int: numpy.int8,
    extrat.KEADataType.t16int: numpy.int16,
    extrat.KEADataType.t32int: numpy.int32,
    extrat.KEADataType.t64int: numpy.int64,
    extrat.KEADataType.t8uint: numpy.uint8,
    extrat.KEADataType.t16uint: numpy.uint16,
    extrat.KEADataType.t32uint: numpy.uint32,
    extrat.KEADataType.t64uint: numpy.uint64,
    extrat.KEADataType.t32float: numpy.float32,
    extrat.KEADataType.t64float: float}

def getCmdargs():
    """     
    Get the command line arguments.
    """
    p = argparse.ArgumentParser()
    p.add_argument("-i", "--infile", help=("Input Raster file. Neighbours will "
                + "be written back to this file. This file should have a Raster"
                + "Attribute Table"), required=True)
    p.add_argument("-t", "--tilesize", default=DFLT_TILESIZE,
        help="Size (in pixels) of tiles to chop input image into for processing."+
                " (default=%(default)s)", type=int)
    p.add_argument("--eightway", default=False, action="store_true",
        help="Use 8-way instead of 4-way")
    p.add_argument("--band", "-b", default=1, type=int, help="Band to use in "+
                "input dataset (1-based) (default=%(default)s)")
    cmdargs = p.parse_args()
    
    return cmdargs
    
def riosAccumulate(info, inputs, outputs, otherargs):
    """
    Calls NeighbourAccumulator.addArray() with each tile
    """
    # convert to 0 baed for numpy
    data = inputs.input[otherargs.band - 1]
    otherargs.accumulator.addArray(data)

def main():
    """
    Main routine. For calling from the command line 
    """
    cmdargs = getCmdargs()

    # we need the dataset for NeighbourAccumulator
    # Open using GDAL here, test routines can supply a fake
    # GDAL dataset.
    # only import here so we don't need GDAL for testing
    from osgeo import gdal
    gdal.UseExceptions()
    ds = gdal.Open(cmdargs.infile, gdal.GA_Update)
    
    buildNeighbours(ds, cmdargs.band, cmdargs.tilesize, cmdargs.eightway)
    
def buildNeighbours(ds, band, tilesize=DFLT_TILESIZE, eightway=False):
    """
    Does the actual work or building the neighbours. 
    
    ds should be a GDAL dataset object
    """
    fieldInfo = extrat.getFieldByName(ds, band, "Histogram")
    size = extrat.getSize(ds, band)
    histogram = extrat.getField(ds, band, fieldInfo, 0, size)
    
    # will throw exception if not set, but we need a nodata otherwise
    # we can't do a read with margin
    nodata = extrat.getNoDataValue(ds, band)
    
    keaDataType = extrat.getImageBandDataType(ds, band)
    dataType = KEA_TO_NUMPY[keaDataType]

    imageInfo = extrat.getSpatialInfo(ds)

    accumulator = extrat.NeighbourAccumulator(histogram, ds, 
                        band, not eightway)    
    
    numXtiles = int(numpy.ceil(imageInfo.xSize / tilesize))
    numYtiles = int(numpy.ceil(imageInfo.ySize / tilesize))

    for tileRow in range(numYtiles):
        ypos = tileRow * tilesize
        ysize = min(tilesize, (imageInfo.ySize - ypos))
        for tileCol in range(numXtiles):
            xpos = tileCol * tilesize
            xsize = min(tilesize, (imageInfo.xSize - xpos))

            data = readBlockWithMargin(ds, band, xpos, ypos, 
                        xsize, ysize, 1, dataType, nodata, 
                        imageInfo.xSize, imageInfo.ySize)
            accumulator.addArray(data)
                        
    
def readBlockWithMargin(ds, band, xoff, yoff, xsize, ysize, margin, datatype, 
        nodata, RasterXSize, RasterYSize):
        """
        Stolen from RIOS
        
        A 'drop-in' look-alike for the ReadAsArray function in GDAL,
        but with the option of specifying a margin width, such that
        the block actually read and returned will be larger by that many pixels. 
        The returned array will ALWAYS contain these extra rows/cols, and 
        if they do not exist in the file (e.g. because the margin would push off 
        the edge of the file) then they will be filled with the given nullVal. 
        Otherwise they will be read from the file along with the rest of the block. 
        
        Variables within this function which have _margin as suffix are intended to 
        designate variables which include the margin, as opposed to those without. 
        
        This routine will cope with any specified region, even if it is entirely outside
        the given raster. The returned block would, in that case, be filled
        entirely with the null value. 
        
        """
        
        # Create the final array, with margin, but filled with the null value. 
        xSize_margin = xsize + 2 * margin
        ySize_margin = ysize + 2 * margin
        outBlockShape = (ySize_margin, xSize_margin)
        
        # Create the empty output array, filled with the appropriate null value. 
        block_margin = numpy.full(outBlockShape, nodata, dtype=datatype)

        # Calculate the bounds of the block which we will actually read from the file,
        # based on what we have been asked for, what margin size, and how close we
        # are to the edge of the file. 
        
        # The bounds of the whole image in the file
        imgLeftBound = 0
        imgTopBound = 0
        imgRightBound = RasterXSize
        imgBottomBound = RasterYSize
        
        # The region we will, in principle, read from the file. Note that xSize_margin 
        # and ySize_margin are already calculated above
        xoff_margin = xoff - margin
        yoff_margin = yoff - margin
        
        # Restrict this to what is available in the file
        xoff_margin_file = max(xoff_margin, imgLeftBound)
        xoff_margin_file = min(xoff_margin_file, imgRightBound)
        xright_margin_file = xoff_margin + xSize_margin
        xright_margin_file = min(xright_margin_file, imgRightBound)
        xSize_margin_file = xright_margin_file - xoff_margin_file

        yoff_margin_file = max(yoff_margin, imgTopBound)
        yoff_margin_file = min(yoff_margin_file, imgBottomBound)
        ySize_margin_file = min(ySize_margin, imgBottomBound - yoff_margin_file)
        ybottom_margin_file = yoff_margin + ySize_margin
        ybottom_margin_file = min(ybottom_margin_file, imgBottomBound)
        ySize_margin_file = ybottom_margin_file - yoff_margin_file
        
        # How many pixels on each edge of the block we end up NOT reading from 
        # the file, and thus have to leave as null in the array
        notRead_left = xoff_margin_file - xoff_margin
        notRead_right = xSize_margin - (notRead_left + xSize_margin_file)
        notRead_top = yoff_margin_file - yoff_margin
        notRead_bottom = ySize_margin - (notRead_top + ySize_margin_file)
        
        # The upper bounds on the slices specified to receive the data
        slice_right = xSize_margin - notRead_right
        slice_bottom = ySize_margin - notRead_bottom
        
        if xSize_margin_file > 0 and ySize_margin_file > 0:
            # Now read in the part of the array which we can actually read from the file.
            # Read each layer separately, to honour the layerselection
            
            # The part of the final array we are filling
            imageSlice = (slice(notRead_top, slice_bottom), slice(notRead_left, slice_right))
            
            block_margin[imageSlice] = extrat.getImageBlock(ds, band, xoff_margin_file, 
                    yoff_margin_file, xSize_margin_file, ySize_margin_file) 
            
        return block_margin

    