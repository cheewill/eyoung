#!/bin/sh

tar xf m4-1.4.16.tar.gz
cd m4-1.4.16
./configure
make
make install
cd ..
rm -rf m4-1.4.16

tar xf bison-2.7.tar.gz
cd bison-2.7
patch data/yacc.c ../yacc.c.diff
./configure
make
make install
cd ..
rm -rf bison-2.7

tar xf flex-2.5.37.tar.bz2
cd flex-2.5.37
./configure
make
make install
cd ..
rm -rf flex-2.5.37

tar xf libelf-0.8.9.tar.gz
cd libelf-0.8.9.tar
./configure 
make
make install
cd ..
rm -rf libelf-0.8.9

tar xf tcc-0.9.26.tar.bz2
cd tcc-0.9.26
./configure --disable-static
make
make install
cd ..
rm -rf tcc-0.9.26
