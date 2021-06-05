#!/bin/bash
cd _KVSLib
make
cd ..
cd _KVSLocalServer
make
cd ..
cd _KVSAuthServer
make 
cd ..
./_KVSAuthServer/KVSAuthServer