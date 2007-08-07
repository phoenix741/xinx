@echo off
cd ..
qmake
make -f Makefile.Release dist
cd setup