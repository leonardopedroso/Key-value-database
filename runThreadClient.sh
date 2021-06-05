#!/bin/bash
cd _KVSLib
make
cp KVS-lib.a ../examples/threadClient/
cd ..
cd examples/threadClient
make
./client