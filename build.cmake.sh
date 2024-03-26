#!/bin/bash
mkdir -p build
pushd build
cmake .. --fresh
cmake --build . 
popd