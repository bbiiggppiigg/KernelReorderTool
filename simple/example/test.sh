#!/bin/bash
../../bin/split_kernel_v2 vectoradd_hip.exe
cp tmp_01.hsaco backup.hsaco
../build/bin/snippetGenerator
../build/bin/snippetReader
../build/bin/snippetInserter tmp_01.hsaco
../../bin/merge_kernel_v2 
~/exec-rw/exec-rw vectoradd_hip.exe bundle.txt new
