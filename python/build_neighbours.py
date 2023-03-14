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
from kealib import extrat



DFLT_TILESIZE = 2048

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
    
def buildNeighbours():
    
    
    # we need the dataset for NeighbourAccumulator
    ds = gdal.Open(cmdargs.infile, gdal.GA_Update)
    
    # read in the histogram to pass to NeighbourAccumulator
    histogram = rat.readColumn(ds, "Histogram", cmdargs.band)
    
    inputs = applier.FilenameAssociations()
    inputs.input = cmdargs.infile

    outputs = applier.FilenameAssociations()

    controls = applier.ApplierControls()
    controls.setOverlap(1)
    controls.setWindowXsize(cmdargs.tilesize)
    controls.setWindowYsize(cmdargs.tilesize)
    progress = GDALProgressBar()
    controls.setProgress(progress)
    
    otherargs = applier.OtherInputs()
    otherargs.accumulator = extrat.NeighbourAccumulator(histogram, ds, 
                        cmdargs.band, not cmdargs.eightway)
    otherargs.band = cmdargs.band
    
    # finished with this - NeighbourAccumulator takes a copy
    del histogram
    
    applier.apply(riosAccumulate, inputs, outputs, otherargs, controls=controls)
    