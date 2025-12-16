#!/bin/bash

script_dir="$(dirname "$0")"
proj_name=mesrouter

cd $script_dir/..
rm -rf build 2>/dev/null
rm $proj_name 2>/dev/null