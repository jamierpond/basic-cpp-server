#!/bin/sh

cd /app

cmake --preset docker

find ./src ./test -name '*.cpp' -o -name '*.hpp' -o -name '*.h' | \
  entr -r sh -c '
    cmake --build build-docker && \
    ./build-docker/PondAudio'

