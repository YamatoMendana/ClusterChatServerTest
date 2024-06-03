#/bin/bash


set -x

rm -rf `pwd`/build/*

mkdir build

cd `pwd`/build &&
	cmake .. &&
	make
