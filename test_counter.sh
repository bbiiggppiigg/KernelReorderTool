#/bin/bash
make
echo "Splitting Kernel"
bin/split_kernel MatrixMultiplication

echo "Patching Text Section Size"
bin/extend_text tmp_00.hsaco 4096


echo "Setting SGPR Count"
bin/set_register_usage tmp_00.hsaco

echo "Inserting tramps"

#LD_LIBRARY_PATH=/home/wuxx1279/bin/dyninst/lib:$LD_LIBRARY_PATH DYNINST_DEBUG_PARSING=1 OMP_NUM_THREADS=1 bin/test_acc tmp_00.hsaco 4096 4120 5728
LD_LIBRARY_PATH=/home/wuxx1279/bin/dyninst/lib:$LD_LIBRARY_PATH DYNINST_DEBUG_PARSING=1 OMP_NUM_THREADS=1 bin/test_counter tmp_00.hsaco 4096 4120 5728


echo "Mering Kernel"
bin/merge_kernel


llvm-objdump -d --mcpu=gfx803 ./tmp_00.hsaco > tmp.s
