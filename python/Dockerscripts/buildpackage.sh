#!/usr/bin/env bash

# This script builds the package using Docker

# Run this second

# Needs to be run as root

BUILD_ROOT=$(cd "$(dirname "$0")/../.."; pwd;)
export HOST_USER_ID=$(id -u)
# needed so we can write the output...
chmod a+w .

docker run -it \
    -v "${BUILD_ROOT}":/home/conda/recipe_root:rw \
    awk:latest \
    bash \
    /home/conda/recipe_root/python/Dockerscripts/dockerbuildconda.sh
