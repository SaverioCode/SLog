#!/bin/bash

BASE_DIR=$(cd "$(dirname "$0")" && pwd)
PARENT_DIR=$(dirname "$BASE_DIR")
TAG=$1

if [ -z "$TAG" ]; then
    echo "Usage: $0 <tag>"
    exit 1
fi

docker run -it --rm -v ${PARENT_DIR}:/home/dev/slogger slogger:${TAG} bash
