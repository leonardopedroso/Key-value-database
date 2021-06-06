#!/bin/bash
cd _KVSLib
make
cp KVS-lib.a ../examples/callbackTest/
cd ..
cd examples/callbackTest
make
./client