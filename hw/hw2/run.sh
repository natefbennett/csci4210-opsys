#!/usr/bin/env bash

# build code
make clean
make 

# create custom MYPATH env variable
export MYPATH=/usr/local/bin:/usr/bin:/bin:.
echo $MYPATH

# run shell program
valgrind ./shell < hw2-input13.txt

# remove set custom env variable
unset MYPATH
