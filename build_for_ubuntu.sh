#!/bin/bash

if [ "$#" -ne 1 ]; then
    echo "Usage: $0 <ubuntu_ver>"
    exit 1
fi

set -ex
docker build --progress=plain --build-arg UBUNTU_VER=$1 .


