#!/bin/bash

set -e

docker run \
  --rm \
  -v $(pwd):/watch \
  -u $(id -u):$(id -g) \
  -p 8080:8080 \
  -it \
  watch-dev \
  /bin/bash
