#!/bin/bash

# ARIA Sensing.

export PREFIX="/home/alessioc/projects/qucs_s/klayout"

cd "$PREFIX"

mkdir -p "$PREFIX/include/db"
mkdir -p "$PREFIX/include/tl"
mkdir -p "$PREFIX/include/lay"
mkdir -p "$PREFIX/include/layui"
mkdir -p "$PREFIX/include/layview"
mkdir -p "$PREFIX/include/laybasic"
mkdir -p "$PREFIX/include/gsi"
mkdir -p "$PREFIX/include/rdb"
mkdir -p "$PREFIX/include/edt"


cp ./src/tl/tl/*.h "$PREFIX/include/tl/"
cp ./src/db/db/*.h "$PREFIX/include/db/"
cp ./src/lay/lay/*.h "$PREFIX/include/lay/"
cp ./src/layui/layui/*.h "$PREFIX/include/layui/"
cp ./src/layview/layview/*.h "$PREFIX/include/layview/"
cp ./src/laybasic/laybasic/*.h "$PREFIX/include/laybasic/"
cp ./src/gsi/gsi/*.h "$PREFIX/include/gsi/"
cp ./src/rdb/rdb/*.h "$PREFIX/include/rdb/"
cp ./src/edt/edt/*.h "$PREFIX/include/edt/"

