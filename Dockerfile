ARG UBUNTU_VER
FROM ubuntu:${UBUNTU_VER}

# Needed in case tzdata gets upgraded
ENV TZ=Australia/Brisbane
ARG DEBIAN_FRONTEND=noninteractive

# Use Aussie mirrors
RUN sed -i 's/http:\/\/archive./http:\/\/au.archive./g' /etc/apt/sources.list
RUN [ -f "/etc/apt/sources.list.d/ubuntu.sources" ] && sed -i 's|http://archive.ubuntu.com/ubuntu/|http://ap-southeast-2.ec2.archive.ubuntu.com/ubuntu/|g' /etc/apt/sources.list.d/ubuntu.sources || true

# Update Ubuntu software stack and install base GDAL stack
RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install -y wget g++ cmake libhdf5-dev libgdal-dev gdal-bin 

RUN echo asdhasjkd
RUN cmake --version
RUN gdal_translate --version

ENV HIGHFIVE_VER=3.3.0
RUN cd /tmp \
    && wget -q https://github.com/highfive-devs/highfive/archive/refs/tags/v${HIGHFIVE_VER}.tar.gz \
    && tar xf v${HIGHFIVE_VER}.tar.gz \
    && cd highfive-${HIGHFIVE_VER} \
    && mkdir build \
    && cd build \
    && cmake -DHIGHFIVE_UNIT_TESTS=OFF \
      -DHIGHFIVE_EXAMPLES=OFF \
      -DHIGHFIVE_USE_BOOST=OFF \
      -DHIGHFIVE_USE_INSTALL_DEPS=OFF \ 
      -DCMAKE_BUILD_TYPE=Release \
      .. \
    && make install \
    && cd ../.. \
    && rm -rf v${HIGHFIVE_VER}.tar.gz highfive-${HIGHFIVE_VER}


RUN --mount=type=bind,source=.,target=/sources cp -R /sources /tmp/kealib
RUN cd /tmp/kealib \
    && rm -rf build \
    && mkdir build \
    && cd build \
    && cmake -D LIBKEA_WITH_GDAL=ON .. \
    && make -j2 \
    && ctest \
    && make install \
    && cd ../.. \
    && rm -rf kealib

ENV LD_LIBRARY_PATH=/usr/local/lib
ENV GDAL_DRIVER_PATH=/usr/local/lib/gdalplugins

RUN gdal_translate --formats | grep KEA
