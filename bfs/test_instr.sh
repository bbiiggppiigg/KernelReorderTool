#/bin/bash
TARGET=bfs
../bin/split_kernel $TARGET.instr
cp tmp_00.hsaco backup.hsaco
llvm-objdump -d --mcpu=gfx900 ./tmp_00.hsaco > orig.s
llvm-objdump -d --mcpu=gfx900 ./tmp_00.hsaco | sed 's/\/\/.*//g'> orig_clean.s
LD_LIBRARY_PATH=/home/wuxx1279/bin/dyninst/lib:$LD_LIBRARY_PATH DYNINST_DEBUG_PARSING=1 OMP_NUM_THREADS=1 ../bin/updated_insert_tramp tmp_00.hsaco config.ini
../bin/merge_kernel
llvm-objdump -d --mcpu=gfx900 ./tmp_00.hsaco | sed 's/\/\/.*//g'> tmp_clean.s
llvm-objdump -d --mcpu=gfx900 ./tmp_00.hsaco > tmp.s





