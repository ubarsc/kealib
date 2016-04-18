Main Page: [kealib.org](http://kealib.org/)

KEALib provides an implementation of the [GDAL](http://gdal.org/) data model. The format supports raster attribute tables, image pyramids, meta-data and in-built statistics while also handling very large files and compression throughout.

Based on the [HDF5](https://www.hdfgroup.org/HDF5/) standard, it also provides a base from which other formats can be derived and is a good choice for long term data archiving. An independent software library (libkea) provides complete access to the KEA image format and a GDAL driver allowing KEA images to be used from any GDAL supported software. 

# Download #
## Binaries ##

Binaries for Windows, Linux and Mac are available through Conda.

Drivers for ArcMap and ERDAS Imagine are available from [downloads](https://bitbucket.org/chchrsc/kealib/downloads)

## Source ##

KEALib requires: [HDF5](https://www.hdfgroup.org/HDF5/) with the C++ libraries and headers. [GDAL](http://gdal.org/) is required to build the GDAL driver.
Download releases from [downloads](https://bitbucket.org/chchrsc/kealib/downloads)

# Documentation #

Some documentation on compilation is available in the [KEALib wiki](https://bitbucket.org/chchrsc/kealib/wiki).

Documentation on the GDAL data model and API is available from the [GDAL website](http://gdal.org/).

There is also a [paper](http://dx.doi.org/10.1016/j.cageo.2013.03.025) describing the features of the format.