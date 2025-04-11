#!/bin/bash

make clean
make

# compile all the examples
echo "compiling test1"
./rx-cc ./tests/test1/test.cmm
echo "compiling test2"
./rx-cc ./tests/test2/test.cmm
echo "compiling test3"
./rx-cc ./tests/test3/test.cmm
echo "compiling test4"
./rx-cc ./tests/test4/test.cmm
echo "compiling test5"
./rx-cc ./tests/test5/test.cmm
echo "compiling test6"
./rx-cc ./tests/test6/test.cmm
echo "compiling test7"
./rx-cc ./tests/test7/test.cmm
echo "compiling test8"
./rx-cc ./tests/test8/test.cmm
echo "compiling test9"
./rx-cc ./tests/test9/test.cmm
echo "compiling test10"
./rx-cc ./tests/test10/test.cmm

# link and run the examples one by one
./rx-linker test.rsk
./rx-vm test.e < /tests/test1/input.in > output1.out
./rx-linker test.rsk
./rx-vm test.e < /tests/test2/input.in > output2.out
./rx-linker test.rsk 
./rx-vm test.e < /tests/test3/input.in > output3.out
./rx-linker test.rsk
./rx-vm test.e < /tests/test4/input.in > output4.out
./rx-linker test.rsk
./rx-vm test.e < /tests/test5/input.in > output5.out
./rx-linker test.rsk
./rx-vm test.e < /tests/test6/input.in > output6.out
./rx-linker test.rsk
./rx-vm test.e < /tests/test7/input.in > output7.out
./rx-linker test.rsk
./rx-vm test.e < /tests/test8/input.in > output8.out
./rx-linker test.rsk
./rx-vm test.e < /tests/test9/input.in > output9.out
./rx-linker test.rsk
./rx-vm test.e < /tests/test10/input.in > output10.out
