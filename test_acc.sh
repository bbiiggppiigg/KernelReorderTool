#/bin/bash
make
echo "Splitting Kernel"
bin/split_kernel DCT/dct

echo "Patching Text Section Size"
bin/extend_text tmp_00.hsaco 4096


echo "Setting SGPR Count"
bin/set_register_usage tmp_00.hsaco _Z9DCTKernelPfS_S_S_jjj 34

bin/move_block tmp_00.hsaco 0x152c 0x1534 0x1524
llvm-objdump -d --mcpu=gfx803 ./tmp_00.hsaco > dct.s

LD_LIBRARY_PATH=/home/wuxx1279/bin/dyninst/lib:$LD_LIBRARY_PATH DYNINST_DEBUG_PARSING=1 OMP_NUM_THREADS=1 bin/test_acc tmp_00.hsaco DCT/test.config DCT/bb_file

#LD_LIBRARY_PATH=/home/wuxx1279/bin/dyninst/lib:$LD_LIBRARY_PATH DYNINST_DEBUG_PARSING=1 OMP_NUM_THREADS=1 bin/test_acc tmp_00.hsaco DCT/test.config DCT/tmp

echo "Mering Kernel"
bin/merge_kernel



#printf '\x22' | dd conv=notrunc of=DCT/dct.new bs=1 seek=$((0x7d3d))

llvm-objdump -d --mcpu=gfx803 ./tmp_00.hsaco > tmp.s
#./MatrixMultiplication.new
