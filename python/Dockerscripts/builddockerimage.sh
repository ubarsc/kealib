#!/usr/bin/env bash

# This script builds the docker image that is used 
# for compilation.

# Run this first

# Needs to be run as root

docker build -t awk:latest -f Dockerfiles/dockerimg .

