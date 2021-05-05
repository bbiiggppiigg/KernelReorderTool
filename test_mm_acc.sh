#/bin/bash
make
echo "Splitting Kernel"
bin/split_kernel MM/MatrixMultiplication

echo "Patching Text Section Size"
bin/extend_text tmp_00.hsaco 4096


echo "Setting SGPR Count"
bin/set_register_usage tmp_00.hsaco _Z9mmmKernelP15HIP_vector_typeIfLj4EES1_S1_jj 26

echo "Inserting tramps"


bin/move_block tmp_00.hsaco 0x124c 0x1260 0x1224 

llvm-objdump -d --mcpu=gfx803 ./tmp_00.hsaco > mm.s
LD_LIBRARY_PATH=/home/wuxx1279/bin/dyninst/lib:$LD_LIBRARY_PATH DYNINST_DEBUG_PARSING=1 OMP_NUM_THREADS=1 bin/test_acc tmp_00.hsaco MM/test.config MM/bb_file

echo "Mering Kernel"
bin/merge_kernel


llvm-objdump -d --mcpu=gfx803 ./tmp_00.hsaco > tmp.s
#./MatrixMultiplication.new
