HIPCC=/opt/rocm/hip/bin/hipcc
all: split_kernel edit_kernel merge_kernel set_register_usage extend_text extend_symbol mm insert_tramp test_acc test_counter

#
# A GNU Makefile
#
DYNINST_ROOT=/home/wuxx1279/bin/dyninst
ifeq ($(DYNINST_ROOT),)
$(error DYNINST_ROOT is not set)
endif

lDyninst= -ldyninstAPI -lsymtabAPI -lparseAPI -linstructionAPI -lcommon -lboost_filesystem -lboost_system

mmpath = ../HIP-Examples/HIP-Examples-Applications/MatrixMultiplication/MatrixMultiplication

mm: $(mmpath)
	cp ../HIP-Examples/HIP-Examples-Applications/MatrixMultiplication/MatrixMultiplication .

edit_kernel: edit_kernel.o
	 g++ edit_kernel.o  -g -L$(DYNINST_ROOT)/lib  $(lDyninst) -o edit_kernel

edit_kernel.o:  edit_kernel.cpp
	g++ -g -Wall -I$(DYNINST_ROOT)/include -c edit_kernel.cpp

insert_tramp: insert_tramp.o
	 g++ insert_tramp.o  -g -L$(DYNINST_ROOT)/lib  $(lDyninst) -o insert_tramp

insert_tramp.o:  insert_tramp.cpp
	g++ -g -Wall -I$(DYNINST_ROOT)/include -c insert_tramp.cpp


test_acc: test_acc.o
	 g++ test_acc.o  -g -L$(DYNINST_ROOT)/lib  $(lDyninst) -o test_acc

test_acc.o:  test_acc.cpp
	g++ -g -Wall -I$(DYNINST_ROOT)/include -c test_acc.cpp

test_counter: test_counter.o
	 g++ test_acc.o  -g -L$(DYNINST_ROOT)/lib  $(lDyninst) -o test_counter

test_counter.o:  test_counter.cpp
	g++ -g -Wall -I$(DYNINST_ROOT)/include -c test_counter.cpp

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

