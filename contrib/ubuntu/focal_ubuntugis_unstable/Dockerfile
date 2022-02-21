#
# This creates an Ubuntu derived base image to build
# Kealib and package it as .deb
# 
# NOTE: The built package is designed to work on ubuntu focal with ubuntu-gis stable PPA
# This is for the updated gdal versions available within that PPA.
#
# Ubuntu 20.04 Bionic Beaver
FROM ubuntu:focal

MAINTAINER EnvSys <developers@envsys.co.uk>

ENV ROOTDIR /usr/local/

ENV TZ=Europe/London
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Load assets
WORKDIR $ROOTDIR/

# Install basic dependencies
RUN apt-get update -y && \
    apt-get install -y --no-install-recommends software-properties-common && \
    add-apt-repository universe && \
    add-apt-repository -y ppa:ubuntugis/ubuntugis-unstable && \
    apt-get update -y && apt-get install --no-install-recommends -y \
    build-essential \
    libgdal-dev \
    libhdf5-dev \
    cmake

ARG KEALIB_VER=1.4.14
ARG PACKAGE_DIR=/usr/local/packages
ARG KEALIB_PACKAGE_DIR=$PACKAGE_DIR/libkea_$KEALIB_VER-1
ARG make_threads=7

RUN mkdir /usr/local/src/kealib
COPY . /usr/local/src/kealib

RUN mkdir -p $KEALIB_PACKAGE_DIR/DEBIAN && mkdir -p $KEALIB_PACKAGE_DIR/etc/profile.d

# Install kealib
RUN cd /usr/local/src/kealib && \
    cmake \
    -D CMAKE_VERBOSE_MAKEFILE=ON \
    -D HDF5_INCLUDE_DIR=/usr/include/hdf5/serial \
    -D GDAL_INCLUDE_DIR=/usr/include/gdal \
    -D LIBKEA_WITH_GDAL=ON \
    -D BUILD_SHARED_LIBS=ON \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/usr \
    . && \
    make -j "${make_threads}" && \
    make install DESTDIR=$KEALIB_PACKAGE_DIR && \
    echo export GDAL_DRIVER_PATH=/usr/lib/gdalplugins >> $KEALIB_PACKAGE_DIR/etc/profile.d/kealib.sh
   
COPY ./contrib/ubuntu/focal_ubuntugis_stable/DEBIAN $KEALIB_PACKAGE_DIR/DEBIAN
RUN dpkg-deb --build $KEALIB_PACKAGE_DIR
RUN echo "The package is built and is available in the image in $PACKAGE_DIR"
