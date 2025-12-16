#!/bin/bash

script_dir="$(dirname "$0")"
proj_name=mesrouter

cd $script_dir/..
mkdir -p build && cd build
cmake ..
make
mv $proj_name ..