#!/bin/bash
./_clean_all.sh
make -f Makefile.lyb
make -f Makefile.tst
make -f Makefile.pyt

cp ./build/fdftest ./
