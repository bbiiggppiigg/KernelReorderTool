HIPCC=/opt/rocm/hip/bin/hipcc

srcs=split_kernel_v2 merge_kernel_v2 preload_global expand_args update_note_phdr report_args_loc disassemble preload_base split_kernel_v3 merge_kernel_v3 preload_global_v3 update_text_phdr update_dynamic
libs=kernel_elf_helper.o

BINS=$(addprefix bin/,$(srcs))
LIBS=$(addprefix lib/,$(libs))

all : $(BINS) $(LIBS)

DYNINST_ROOT=/home/wuxx1279/bin/dyninst-amdgpu-codegen
ifeq ($(DYNINST_ROOT),)
$(error DYNINST_ROOT is not set)
endif

TBB=/opt/spack/opt/spack/linux-centos8-zen2/gcc-10.2.0/intel-tbb-2020.3-zbj2dajg6q53hhsfd7kglxwqhyc7ie3v/include
TBB=/opt/intel-tbb/include
lDyninst= -ldyninstAPI -lsymtabAPI -lparseAPI -linstructionAPI -lcommon -lboost_filesystem -lboost_system  -ldynElf


lib/kernel_elf_helper.o: lib/kernel_elf_helper.h lib/kernel_elf_helper.cpp 
	g++ -g -Wall  -c lib/kernel_elf_helper.cpp -o lib/kernel_elf_helper.o -I msgpack-c/include/  -I/opt/intel-tbb/include

bin/preload_base: src/preload_global_v3.cpp lib/InsnFactory.h lib/kernel_elf_helper.o src/helper.h
	g++ -g -DMEASURE_BASE -Wall -Wextra -Wno-class-memaccess -I$(DYNINST_ROOT)/include -I$(TBB) -Ilib/ -Ilib/inih/ -Ilib/amdgpu-tooling src/preload_global_v3.cpp lib/amdgpu-tooling/KernelDescriptor.cpp -L$(DYNINST_ROOT)/lib -Iinclude -Iinih/ -I/opt/intel-tbb/include lib/kernel_elf_helper.o  $(lDyninst) -o bin/preload_base

bin/preload_global_v3: src/preload_global_v3.cpp lib/InsnFactory.h lib/kernel_elf_helper.o src/helper.h
	g++ -g -Wall -Wextra -Wno-class-memaccess -I$(DYNINST_ROOT)/include -I$(TBB) -Ilib/ -Ilib/inih/ -Ilib/amdgpu-tooling src/preload_global_v3.cpp lib/amdgpu-tooling/KernelDescriptor.cpp -L$(DYNINST_ROOT)/lib -Iinclude -Iinih/ -I/opt/intel-tbb/include lib/kernel_elf_helper.o  $(lDyninst) -o bin/preload_global_v3

bin/split_kernel_v3: src/split_kernel_v3.cpp
	g++ -g -o $@ $<

bin/merge_kernel_v3: src/merge_kernel_v3.cpp
	g++ -g -o $@ $<

bin/expand_args: src/expand_args.cpp
	g++ -I lib/msgpack-c/include/ -I /opt/rocm/include -Wl,--demangle -Wl,-rpath,/opt/rocm/lib/ -L/opt/rocm/lib/-lamd_comgr $< -o $@

bin/report_args_loc: src/report_args_loc.cpp
	g++ -I lib/msgpack-c/include/ -I /opt/rocm/include -Wl,--demangle -Wl,-rpath,/opt/rocm/lib/ -L/opt/rocm/lib/-lamd_comgr $< -o $@


bin/update_note_phdr: src/update_note_phdr.cpp
	g++ $< -o $@

bin/update_text_phdr: src/update_text_phdr.cpp
	g++ $< -o $@
bin/update_dynamic: src/update_dynamic.cpp
	g++ $< -o $@

bin/disassemble: src/disassemble.cpp
	g++ $< -o $@ -I$(DYNINST_ROOT)/include -I/opt/intel-tbb/include -L$(DYNINST_ROOT)/lib $(lDyninst)
clean:
	rm -f *.bundle *.hsaco *.isa src/*.o lib/*.o bin/*

