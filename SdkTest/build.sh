#!/bin/sh
gcc main.c -I ../include -L ../lib/ -l:libMotorMaster.Sdk.so -o SdkTest
export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:../lib

./SdkTest
