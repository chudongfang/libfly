#!/bin/bash
ctags -R .
make clean
make
./test_server
#make clean
