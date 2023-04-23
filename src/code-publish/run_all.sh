#!/usr/bin/env bash

# If project not ready, generate cmake file
if [[ ! -d build ]]; then
    echo "good"
else
    rm -rf build
fi
mkdir -p build

cd build
cmake ..
make -j
cd ..

# time bin/SIMULATOR