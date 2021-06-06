#!/bin/bash
cd _KVSLib
make
cp KVS-lib.a ../examples/multiClients/
cd ..
cd examples/multiClients
make
./client