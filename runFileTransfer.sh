#!/bin/bash
cd _KVSLib
make
cp KVS-lib.a ../examples/fileTransfer/
cd ..
cd examples/fileTransfer
rm downloadClient2*
make
./client1
/bin/sleep 1
./client2