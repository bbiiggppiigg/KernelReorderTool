HIPCC=/opt/rocm/hip/bin/hipcc

srcs=split_kernel split_kernel_v2 merge_kernel merge_kernel_v2 preload_global
libs=kernel_elf_helper.o InstrUtil.o

BINS=$(addprefix bin/,$(srcs))
LIBS=$(addprefix lib/,$(libs))

all : $(BINS) $(LIBS)

DYNINST_ROOT=/home/wuxx1279/bin/dyninst
ifeq ($(DYNINST_ROOT),)
$(error DYNINST_ROOT is not set)
endif

TBB=/opt/spack/opt/spack/linux-centos8-zen2/gcc-10.2.0/intel-tbb-2020.3-zbj2dajg6q53hhsfd7kglxwqhyc7ie3v/include
TBB=/opt/intel-tbb/include
lDyninst= -ldyninstAPI -lsymtabAPI -lparseAPI -linstructionAPI -lcommon -lboost_filesystem -lboost_system  -ldynElf


lib/kernel_elf_helper.o: lib/kernel_elf_helper.h lib/kernel_elf_helper.cpp 
	g++ -g -Wall  -c lib/kernel_elf_helper.cpp -o lib/kernel_elf_helper.o -I msgpack-c/include/  -I/opt/intel-tbb/include

bin/preload_global: src/preload_global.cpp lib/InsnFactory.h lib/kernel_elf_helper.o  
	g++ -g -Wall -Wno-class-memaccess -I$(DYNINST_ROOT)/include -I$(TBB) -Ilib/ -Ilib/inih/ -Ilib/amdgpu-tooling src/preload_global.cpp lib/amdgpu-tooling/KernelDescriptor.cpp -L$(DYNINST_ROOT)/lib -Iinclude -Iinih/ -I/opt/intel-tbb/include lib/InstrUtil.o lib/kernel_elf_helper.o $(lDyninst) -o bin/preload_global

lib/InstrUtil.o: lib/InstrUtil.cpp lib/InsnFactory.h
	g++ -g -Wall  -c lib/InstrUtil.cpp -o lib/InstrUtil.o

bin/split_kernel_v2: src/split_kernel_v2.cpp
	g++ -o $@ $<

bin/merge_kernel_v2: src/merge_kernel_v2.cpp
	g++ -o $@ $<

clean:
	rm -f *.bundle *.hsaco *.isa src/*.o lib/*.o bin/*

