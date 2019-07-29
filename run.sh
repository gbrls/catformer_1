#!/bin/sh
cd $1
rm ./catformer
make
./catformer
