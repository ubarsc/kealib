#!/usr/bin/env python

"""
Main script for building neighbours 
"""

from __future__ import print_function, division

import sys
import argparse
from rios import rat
from rios import applier
from rios.cuiprogress import GDALProgressBar

from kealib import extrat

from osgeo import gdal
gdal.UseExceptions()

DFLT_TILESIZE = 2048

def getCmdargs():
    """     
    Get the command line arguments.
    """
    p = argparse.ArgumentParser()
    p.add_argument("-i", "--infile", help=("Input Raster file. Neighbours will "
                + "be written back to this file. This file should have a Raster"
                + "Attribute Table"))
    p.add_argument("-t", "--tilesize", default=DFLT_TILESIZE,
        help="Size (in pixels) of tiles to chop input image into for processing."+
                " (default=%(default)s)", type=int)
    p.add_argument("--eightway", default=False, action="store_true",
        help="Use 8-way instead of 4-way")
    p.add_argument("--band", "-b", default=1, type=int, help="Band to use in "+
                "input dataset (1-based) (default=%(default)s)")
    cmdargs = p.parse_args()
    
    if cmdargs.infile is None:
        print('Must supply input file name')
        p.print_help()
        sys.exit()

    return cmdargs
    
def riosAccumulate(info, inputs, outputs, otherargs):
    """
    Calls NeighbourAccumulator.addArray() with each tile
    """
    # convert to 0 baed for numpy
    data = inputs.input[otherargs.band - 1]
    otherargs.accumulator.addArray(data)

def main(cmdargs):
    """
    Main routine. Calls RIOS to do the processing. 
    """
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
    
if __name__ == '__main__':
    cmdargs = getCmdargs()

    main(cmdargs)

