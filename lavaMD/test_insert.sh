#/bin/bash
TARGET=lavaMD
../bin/split_kernel $TARGET.exe
cp tmp_00.hsaco backup.hsaco
llvm-objdump -d --mcpu=gfx900 ./tmp_00.hsaco > orig.s
llvm-objdump -d --mcpu=gfx900 ./tmp_00.hsaco | sed 's/\/\/.*//g'> orig_clean.s
LD_LIBRARY_PATH=/home/wuxx1279/bin/dyninst/lib:$LD_LIBRARY_PATH DYNINST_DEBUG_PARSING=1 OMP_NUM_THREADS=1 ../bin/insert_tramp tmp_00.hsaco 41 54 1000 2 88 3200000 7200
#llvm-objcopy --dump-section .note=tmp_00.note.old tmp_00.hsaco
#update_note_modify_lds_size tmp_00.note.old tmp_00.note.new 127
#llvm-objcopy --rename-section .note=.qq tmp_00.hsaco
#llvm-objcopy -R .note tmp_00.hsaco
#llvm-objcopy --add-section .note=tmp_00.note.old tmp_00.hsaco
#llvm-objcopy --set-section-flags .note=alloc tmp_00.hsaco
#llvm-objcopy --set-section-alignment .note=4 tmp_00.hsaco
../bin/merge_kernel
llvm-objdump -d --mcpu=gfx900 ./tmp_00.hsaco | sed 's/\/\/.*//g'> tmp_clean.s
llvm-objdump -d --mcpu=gfx900 ./tmp_00.hsaco > tmp.s




# Backup Commands
#llvm-objcopy -R .note tmp_00.hsaco
