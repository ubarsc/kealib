#
# This creates an Ubuntu derived base image to build
# Kealib and package it as .deb
# 
# NOTE: The built package is designed to work on ubuntu jammy
#
# Ubuntu 22.04 Jammy Jellyfish
FROM ubuntu:jammy

MAINTAINER EnvSys <developers@envsys.co.uk>

ENV ROOTDIR /usr/local/

ENV TZ=Europe/London
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

# Load assets
WORKDIR $ROOTDIR/

# Install basic dependencies
RUN apt-get update -y && \
    apt-get install -y --no-install-recommends software-properties-common gnupg && \
    add-apt-repository universe && \
    add-apt-repository -y ppa:ubuntugis/ubuntugis-unstable && \
    apt-get update -y && apt-get install --no-install-recommends -y \
    build-essential \
    libgdal-dev \
    libhdf5-dev \
    cmake

ARG KEALIB_VER=1.5.1
ARG PACKAGE_DIR=/usr/local/packages
ARG KEALIB_PACKAGE_DIR=$PACKAGE_DIR/libkea_$KEALIB_VER-1
ARG make_threads=7

RUN mkdir /usr/local/src/kealib
COPY . /usr/local/src/kealib

RUN mkdir -p $KEALIB_PACKAGE_DIR/DEBIAN && mkdir -p $KEALIB_PACKAGE_DIR/etc/profile.d

# Install kealib
RUN cd /usr/local/src/kealib && \
    mkdir build && cd build && \
    cmake \
    -D CMAKE_VERBOSE_MAKEFILE=ON \
    -D BUILD_SHARED_LIBS=ON \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_INSTALL_PREFIX=/usr \
    .. && \
    make -j "${make_threads}" && \
    make install DESTDIR=$KEALIB_PACKAGE_DIR && \
    cmake \
    -D CMAKE_VERBOSE_MAKEFILE=ON \
    -D CMAKE_INSTALL_PREFIX=/usr \
    -D LIBKEA_WITH_GDAL=ON \
    -D CMAKE_BUILD_TYPE=Release \
    .. && \
    make -j "${make_threads}" && \
    make install DESTDIR=$KEALIB_PACKAGE_DIR && \
    echo export GDAL_DRIVER_PATH=/usr/lib/gdalplugins >> $KEALIB_PACKAGE_DIR/etc/profile.d/kealib.sh

COPY ./contrib/ubuntu/jammy_ubuntugis_unstable/DEBIAN $KEALIB_PACKAGE_DIR/DEBIAN
RUN dpkg-deb --build $KEALIB_PACKAGE_DIR
RUN echo "The package is built and is available in the image in $PACKAGE_DIR" && ls $PACKAGE_DIR
