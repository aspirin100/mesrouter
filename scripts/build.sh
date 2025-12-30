#!/bin/bash

script_dir="$(dirname "$0")"
proj_name=mesrouter
preset=default

cd $script_dir/..
cmake -DCMAKE_MAKE_PROGRAM=/usr/bin/make --preset=$preset
cd build
make