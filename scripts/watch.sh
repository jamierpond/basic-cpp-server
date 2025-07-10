#!/bin/sh

cd /app

# Watch for changes and rebuild using Ninja
find ./src -name '*.cpp' -o -name '*.h' | \
  entr -r sh -c '
    cmake -S . -B build \
      -G Ninja \
      -DCMAKE_BUILD_TYPE=Debug \
      -DCMAKE_C_COMPILER=clang \
      -DCMAKE_CXX_COMPILER=clang++ && \
    cmake --build build && \
    ./build/PondAudio'

