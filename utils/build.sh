#!/bin/sh

rm -rf build
mkdir -p build
cd build && cmake ../ -DTEST_CASE=ON && make -j4

