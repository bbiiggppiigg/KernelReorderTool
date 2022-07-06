#/bin/bash
TARGET=backprop



CONF=k1
../bin/split_kernel $TARGET.instr
cp tmp_00.hsaco backup.hsaco
llvm-objdump -d --mcpu=gfx900 ./tmp_00.hsaco > orig.s
llvm-objdump -d --mcpu=gfx900 ./tmp_00.hsaco | sed 's/\/\/.*//g'> orig_clean.s
LD_LIBRARY_PATH=/home/wuxx1279/bin/dyninst/lib:$LD_LIBRARY_PATH DYNINST_DEBUG_PARSING=1 OMP_NUM_THREADS=1 ../bin/updated_insert_tramp tmp_00.hsaco $CONF.ini
../bin/merge_kernel
llvm-objdump -d --mcpu=gfx900 ./tmp_00.hsaco | sed 's/\/\/.*//g'> tmp_clean.s
llvm-objdump -d --mcpu=gfx900 ./tmp_00.hsaco > tmp.s
cp $TARGET.instr.new $TARGET.instr.$CONF.new
#ROCR_VISIBLE_DEVICES=0 ./backprop.instr.k1.new 65536 > lds.txt

CONF=k2
../bin/split_kernel $TARGET.instr
cp tmp_00.hsaco backup.hsaco
llvm-objdump -d --mcpu=gfx900 ./tmp_00.hsaco > orig.s
llvm-objdump -d --mcpu=gfx900 ./tmp_00.hsaco | sed 's/\/\/.*//g'> orig_clean.s
LD_LIBRARY_PATH=/home/wuxx1279/bin/dyninst/lib:$LD_LIBRARY_PATH DYNINST_DEBUG_PARSING=1 OMP_NUM_THREADS=1 ../bin/updated_insert_tramp tmp_00.hsaco $CONF.ini
../bin/merge_kernel $TARGET.instr.$CONF.new
llvm-objdump -d --mcpu=gfx900 ./tmp_00.hsaco | sed 's/\/\/.*//g'> tmp_clean.s
llvm-objdump -d --mcpu=gfx900 ./tmp_00.hsaco > tmp.s







