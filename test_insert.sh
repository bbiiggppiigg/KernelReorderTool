#/bin/bash

bin/split_kernel vectoradd_hip.exe

LD_LIBRARY_PATH=/home/wuxx1279/bin/dyninst/lib:$LD_LIBRARY_PATH DYNINST_DEBUG_PARSING=1 OMP_NUM_THREADS=1 bin/insert_tramp tmp_00.hsaco 1 2 3

bin/extend_text tmp_00.hsaco 236
bin/merge_kernel
#llvm-objdump -d --mcpu=gfx803 ./tmp_00.hsaco > tmp.s
