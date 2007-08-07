@echo off
cd ..
qmake
make -f Makefile.Release
cd setup