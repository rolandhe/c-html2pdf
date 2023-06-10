#!/bin/sh

OS_NAME="`uname`"
echo  $OS_NAME


rm -fr build
mkdir build
cd build
cmake ..
make


