#!/bin/sh

cd /app

find ./src ./test -name '*.cpp' -o -name '*.hpp' -o -name '*.h' | \
  entr -r sh -c '
    cmake --build build/debug && \
    ./build/debug/PondAudio'

