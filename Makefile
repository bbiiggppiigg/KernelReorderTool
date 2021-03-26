HIPCC=/opt/rocm/hip/bin/hipcc
all: split_kernel edit_kernel merge_kernel set_register_usage extend_text extend_symbol mm

#
# A GNU Makefile
#

ifeq ($(DYNINST_ROOT),)
$(error DYNINST_ROOT is not set)
endif


mmpath = ../HIP-Examples/HIP-Examples-Applications/MatrixMultiplication/MatrixMultiplication

mm: $(mmpath)
	cp ../HIP-Examples/HIP-Examples-Applications/MatrixMultiplication/MatrixMultiplication .

edit_kernel: edit_kernel.o
	 g++ edit_kernel.o  -g -L$(DYNINST_ROOT)/lib  \
		 -ldyninstAPI -lsymtabAPI -lparseAPI -linstructionAPI -lcommon \
		 -lboost_filesystem -lboost_system -o edit_kernel

edit_kernel.o:  edit_kernel.cpp
	g++ -g -Wall  -I/home/wuxx1279/spack/opt/spack/linux-ubuntu18.04-zen/gcc-7.5.0/boost-1.75.0-x6xb3ekqedmhn3sgslg24e5vyw44yf66/include \
    -I/home/wuxx1279/spack/opt/spack/linux-ubuntu18.04-zen/gcc-7.5.0/intel-tbb-2020.3-ilm4ideiyh3avl5zdajneu25csygvwpt/include \
    -I$(DYNINST_ROOT)/include -c edit_kernel.cpp

set_register_usage: set_register_usage.cpp
	g++ -o set_register_usage set_register_usage.cpp

extend_text: extend_text.cpp
	g++ -o extend_text extend_text.cpp

extend_symbol: extend_symbol.cpp
	g++ -o extend_symbol extend_symbol.cpp



split_kernel: split_kernel.cpp
	g++ -o split_kernel split_kernel.cpp

merge_kernel: merge_kernel.cpp
	g++ -o merge_kernel merge_kernel.cpp


clean:
	rm -f *.bundle *.hsaco *.isa split_kernel merge_kernel edit_kernel *.o

