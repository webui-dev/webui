#!/bin/bash
mkdir -p build

# webui-2.5.0, examples
pushd build
cmake .. --fresh
cmake --build . --config Debug 
popd

