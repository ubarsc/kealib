Main Page: [kealib.org](http://kealib.org/)

KEALib provides an implementation of the [GDAL](http://gdal.org/) data model. The format supports raster attribute tables, image pyramids, meta-data and in-built statistics while also handling very large files and compression throughout.

Based on the [HDF5](https://www.hdfgroup.org/HDF5/) standard, it also provides a base from which other formats can be derived and is a good choice for long term data archiving. An independent software library (libkea) provides complete access to the KEA image format and a GDAL driver allowing KEA images to be used from any GDAL supported software. 

# Download #
## Binaries ##

Binaries for Windows, Linux and Mac are available through Conda on the "osgeo" channel:

1. Download and install the Python 3.4 installer for your platform from the Miniconda site.
1. Bring up the command line and type "conda install -c osgeo kealib"
1. For GDAL support under Linux and Mac (nothing else required for Windows), the $GDAL_DRIVER_PATH environment variable must be set to the location of "gdalplugins" sub-directory of your Miniconda install. The following command should work for the default Miniconda installation location:


```
#!bash

export GDAL_DRIVER_PATH=~/miniconda3/gdalplugins
```


4. To upgrade when a new version is released, type "conda update -c osgeo kealib"

Drivers for ArcMap and ERDAS Imagine are available from [downloads](https://bitbucket.org/chchrsc/kealib/downloads)

## Source ##

KEALib requires: [HDF5](https://www.hdfgroup.org/HDF5/) with the C++ libraries and headers. [GDAL](http://gdal.org/) is required to build the GDAL driver.
Download releases from [downloads](https://bitbucket.org/chchrsc/kealib/downloads)

# Documentation #

Some documentation on compilation is available in the [KEALib wiki](https://bitbucket.org/chchrsc/kealib/wiki).

Documentation on the GDAL data model and API is available from the [GDAL website](http://gdal.org/).

There is also a [paper](http://dx.doi.org/10.1016/j.cageo.2013.03.025) describing the features of the format.