#!/bin/bash

set -e -x

docker pull emscripten/emsdk:3.1.35

cd .devcontainer && docker build -t watch-dev .
