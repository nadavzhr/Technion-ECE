#!/bin/bash

make clean
make

# compile all the examples
echo "compiling example1"
./rx-cc ./example-files/example1.cmm
echo "compiling example2"
./rx-cc ./example-files/example2.cmm
echo "compiling example3"
./rx-cc ./example-files/example3-main.cmm
./rx-cc ./example-files/example3-funcs.cmm
echo "compiling example4"
./rx-cc ./example-files/example4.cmm
echo "compiling example5"
./rx-cc ./example-files/example5.cmm
echo "compiling example6"
./rx-cc ./example-files/example6.cmm

# link and run the examples one by one
./rx-linker example1.rsk
./rx-vm example1.e
./rx-linker example2.rsk
./rx-vm example2.e
./rx-linker example3-main.rsk example3-funcs.rsk
./rx-vm example3-main.e
./rx-linker example4.rsk
./rx-vm example4.e
./rx-linker example6.rsk
./rx-vm example6.e
