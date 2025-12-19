#!/bin/bash

script_dir="$(dirname "$0")"
proj_name=mesrouter
preset=default

cd $script_dir/..
cmake --preset=$preset
cd build
make
mv $proj_name ..