#!/bin/bash
./_clean_all.sh
./_build_lyb.sh
./_build_tst.sh
./_build_pyt.sh

cp ./build/fdftest ./
cp ./build/libfdf.so ./
cp ./Python/fdf.*.so ./
