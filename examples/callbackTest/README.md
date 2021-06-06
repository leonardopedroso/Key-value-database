In this test, one has to use the bash scripts runCallbackTest_createkey.sh and runCallbackTest.sh. 
1. Run auth server
2. Run KVSlocal server and crete group ab
3. Run runCallbackTest_createkey.sh
4. Run runCallbackTest.sh
5. Hit enter very fastly on the runCallbackTest_createkey.sh program 
6. Then ckeck that there were 4 callbacks function ran for the key newValue and only 3 changes changes on runCallbackTest.sh because one on of the processes kill himself and yet not seg falts arise :)
