#!/bin/bash
cd _KVSLib
make
cp KVS-lib.a ../examples/simpleClient/
cd ..
cd examples/simpleClient
make
./client