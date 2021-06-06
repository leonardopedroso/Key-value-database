#!/bin/bash
cd _KVSLib
make
cp KVS-lib.a ../examples/threadMadness/
cd ..
cd examples/threadMadness
make
./client