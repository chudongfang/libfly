#!/bin/bash
ctags -R .
make clean
make
./test
#make clean
