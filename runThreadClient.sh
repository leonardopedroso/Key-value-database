#!/bin/bash
cd _KVSLib
make
cp KVS-lib.a ../examples/threadClient/
cd ..
cd _KVSLocalServer
make
cd ..
cd _KVSAuthServer
make 
cd ..
cd examples/threadClient
make
./client