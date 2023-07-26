#! /usr/bin/bash
cd build
if [ ! -z "$1" ]
  then
  	./$1
else
    ./test
fi