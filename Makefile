HIPCC=/opt/rocm/hip/bin/hipcc

srcs=expand_args update_note_phdr report_args_loc disassemble bd_base split_kernel merge_kernel bd_inplace update_text_phdr update_dynamic empty debug update_kd
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

bin/bd_base: src/bd_inplace.cpp lib/InsnFactory.h lib/kernel_elf_helper.o src/helper.h
	g++ -g -DMEASURE_BASE -Wall -Wextra -Wno-class-memaccess -I$(DYNINST_ROOT)/include -I$(TBB) -Ilib/ -Ilib/inih/ -Ilib/amdgpu-tooling src/bd_inplace.cpp lib/amdgpu-tooling/KernelDescriptor.cpp -L$(DYNINST_ROOT)/lib -Iinclude -Iinih/ -I/opt/intel-tbb/include lib/kernel_elf_helper.o  $(lDyninst) -o bin/bd_base

bin/bd_inplace: src/bd_inplace.cpp lib/InsnFactory.h lib/kernel_elf_helper.o src/helper.h
	g++ -g -Wall -Wextra -Wno-class-memaccess -I$(DYNINST_ROOT)/include -I$(TBB) -Ilib/ -Ilib/inih/ -Ilib/amdgpu-tooling src/bd_inplace.cpp lib/amdgpu-tooling/KernelDescriptor.cpp -L$(DYNINST_ROOT)/lib -Iinclude -Iinih/ -I/opt/intel-tbb/include lib/kernel_elf_helper.o  $(lDyninst) -o bin/bd_inplace

bin/debug: src/debug.cpp lib/InsnFactory.h lib/kernel_elf_helper.o src/helper.h
	g++ -g -Wall -Wextra -Wno-class-memaccess -I$(DYNINST_ROOT)/include -I$(TBB) -Ilib/ -Ilib/inih/ -Ilib/amdgpu-tooling src/debug.cpp lib/amdgpu-tooling/KernelDescriptor.cpp -L$(DYNINST_ROOT)/lib -Iinclude -Iinih/ -I/opt/intel-tbb/include lib/kernel_elf_helper.o  $(lDyninst) -o bin/debug

bin/empty: src/empty.cpp lib/InsnFactory.h lib/kernel_elf_helper.o src/helper.h
	g++ -g -Wall -Wextra -Wno-class-memaccess -I$(DYNINST_ROOT)/include -I$(TBB) -Ilib/ -Ilib/inih/ -Ilib/amdgpu-tooling src/empty.cpp lib/amdgpu-tooling/KernelDescriptor.cpp -L$(DYNINST_ROOT)/lib -Iinclude -Iinih/ -I/opt/intel-tbb/include lib/kernel_elf_helper.o  $(lDyninst) -o bin/empty

bin/update_kd: src/update_kd.cpp
	g++ -g -o $@ $<


bin/split_kernel: src/split_kernel.cpp
	g++ -g -o $@ $<

bin/merge_kernel: src/merge_kernel.cpp
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

