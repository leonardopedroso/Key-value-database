#!/bin/bash
cd _KVSLib
make
cp KVS-lib.a ../examples/simpleClient/
cd ..
cd _KVSLocalServer
make
cd ..
cd _KVSAuthServer
make 
cd ..
cd examples/simpleClient
make
./client