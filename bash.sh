#!/bin/bash

echo "Script begin."

rm -rf CMakeCache.txt cmake_install.cmake Makefile servo

echo "Files have been deleted"

cmake .

sudo make

