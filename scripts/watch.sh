
#!/bin/bash

cd $(dirname "$0")
cd ../

# Watch for changes and rebuild
find ./src -name '*.cpp' -o -name '*.h' | \
  entr -r sh -c 'cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ && cmake --build build && ./build/PondAudio'

