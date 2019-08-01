#!/bin/sh
cd $1
time make
./catformer
make clean
