#!/bin/bash
cd _KVSLib
make
cp KVS-lib.a ../examples/KVSLib_ui/
cd ..
cd examples/KVSLib_ui
make
./client