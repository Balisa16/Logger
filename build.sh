#! /usr/bin/bash
cd build
if [ ! -z "$1" ]
  then
  	rm -r *
    cmake ..
fi
make -j4