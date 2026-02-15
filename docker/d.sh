#!/bin/bash

BASE_DIR=$(cd "$(dirname "$0")" && pwd)
PARENT_DIR=$(dirname "$BASE_DIR")
TAG=$1

if [ -z "$TAG" ]; then
    echo "Usage: $0 <tag>"
    exit 1
fi

DOCKER_SLOG_DIR=/home/dev/slog

docker run -it --rm \
    -v ${PARENT_DIR}/include:${DOCKER_SLOG_DIR}/include \
    -v ${PARENT_DIR}/src:${DOCKER_SLOG_DIR}/src \
    -v ${PARENT_DIR}/tests:${DOCKER_SLOG_DIR}/tests \
    -v ${PARENT_DIR}/example:${DOCKER_SLOG_DIR}/example \
    -v ${PARENT_DIR}/CMakeLists.txt:${DOCKER_SLOG_DIR}/CMakeLists.txt \ 
    slog:${TAG} bash
