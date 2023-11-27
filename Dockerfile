FROM ubuntu:22.04

# Needed in case tzdata gets upgraded
ENV TZ=Australia/Brisbane
ARG DEBIAN_FRONTEND=noninteractive

# Use Aussie mirrors
RUN sed -i 's/http:\/\/archive./http:\/\/au.archive./g' /etc/apt/sources.list

# Update Ubuntu software stack and install base GDAL stack
RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install -y wget g++ cmake libhdf5-dev libgdal-dev gdal-bin

RUN apt-get autoremove -y && apt-get clean && rm -rf /var/lib/apt/lists/*

COPY . /tmp/kealib
RUN cd /tmp/kealib \
    && mkdir build \
    && cd build \
    && cmake -D LIBKEA_WITH_GDAL=ON .. \
    && make -j2 \
    && make install \
    && cd ../.. \
    && rm -rf kealib

ENV LD_LIBRARY_PATH=/usr/local/lib
ENV GDAL_DRIVER_PATH=/usr/local/lib/gdalplugins

RUN gdal_translate --formats | grep KEA
