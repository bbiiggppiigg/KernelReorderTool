HIPCC=/opt/rocm/hip/bin/hipcc

srcs=thread_id expand_args update_note_phdr report_args_loc disassemble split_kernel merge_kernel bd_inplace bd_inplace_global update_text_phdr update_dynamic debug update_kd pure_bd
special=bd_base
libs=kernel_elf_helper.o

EXES := $(foreach item,$(srcs),bin/$(item).exe)
all: bin/kernel_elf_helper.o $(EXES)

DYNINST_ROOT=/home/wuxx1279/bin/dyninst-liveness
ifeq ($(DYNINST_ROOT),)
$(error DYNINST_ROOT is not set)
endif

TBB=/opt/spack/opt/spack/linux-centos8-zen2/gcc-10.2.0/intel-tbb-2020.3-zbj2dajg6q53hhsfd7kglxwqhyc7ie3v/include
lDyninst= -ldyninstAPI -lsymtabAPI -lparseAPI -linstructionAPI -lcommon -lboost_filesystem -lboost_system  -ldynElf
iLib= -I$(DYNINST_ROOT)/include -I$(TBB) -I ELFIO -I amdgpu-tooling -I msgpack-c -Iinclude -I/opt/rocm/include -Ilib/ -Ilib/inih -Ilib/amdgpu-tooling
lLib= -L$(DYNINST_ROOT)/lib -L/opt/rocm/lib/
options= -std=c++17 -g -Wall -Wextra -Wno-class-memaccess
loptions= -Wl,--demangle -Wl,-rpath,/opt/rocm/lib
links= $(lDyninst) -lamd_comgr
bin/%.exe: src/%.cpp $(CURDIR)/bin/kernel_elf_helper.o src/helper.h
	g++ $(options) $(iLib) $^ $(iLib) $(lLib) $(links) $(loptions) -o $@

bin/kernel_elf_helper.o: src/kernel_elf_helper.h src/kernel_elf_helper.cpp 
	g++ -g -Wall  -c src/kernel_elf_helper.cpp -o $@ -I msgpack-c/include/  -I/opt/intel-tbb/include
clean:
	rm -f *.bundle *.hsaco *.isa src/*.o lib/*.o bin/*.o bin/*.exe

ifeq ("x","y")
lib/kernel_elf_helper.o: lib/kernel_elf_helper.h lib/kernel_elf_helper.cpp 
	g++ -g -Wall  -c lib/kernel_elf_helper.cpp -o lib/kernel_elf_helper.o -I msgpack-c/include/  -I/opt/intel-tbb/include

bin/bd_base: src/bd_inplace.cpp lib/InsnFactory.h lib/kernel_elf_helper.o src/helper.h
	g++ -g -DMEASURE_BASE -Wall -Wextra -Wno-class-memaccess -I$(DYNINST_ROOT)/include -I$(TBB) -Ilib/ -Ilib/inih/ -Ilib/amdgpu-tooling src/bd_inplace.cpp lib/amdgpu-tooling/KernelDescriptor.cpp -L$(DYNINST_ROOT)/lib -Iinclude -Iinih/ -I/opt/intel-tbb/include lib/kernel_elf_helper.o  $(lDyninst) -o bin/bd_base

bin/bd_inplace: src/bd_inplace.cpp lib/InsnFactory.h lib/kernel_elf_helper.o src/helper.h
	g++ -g -Wall -Wextra -Wno-class-memaccess -I$(DYNINST_ROOT)/include -I$(TBB) -Ilib/ -Ilib/inih/ -Ilib/amdgpu-tooling src/bd_inplace.cpp lib/amdgpu-tooling/KernelDescriptor.cpp -L$(DYNINST_ROOT)/lib -Iinclude -Iinih/ -I/opt/intel-tbb/include lib/kernel_elf_helper.o  $(lDyninst) -o bin/bd_inplace

bin/pure_bd: src/pure_bd.cpp lib/InsnFactory.h lib/kernel_elf_helper.o src/helper.h
	g++ -g -Wall -Wextra -Wno-class-memaccess -I$(DYNINST_ROOT)/include -I$(TBB) -Ilib/ -Ilib/inih/ -Ilib/amdgpu-tooling src/pure_bd.cpp lib/amdgpu-tooling/KernelDescriptor.cpp -L$(DYNINST_ROOT)/lib -Iinclude -Iinih/ -I/opt/intel-tbb/include lib/kernel_elf_helper.o  $(lDyninst) -o bin/pure_bd


bin/bd_inplace_global: src/bd_inplace_global.cpp lib/InsnFactory.h lib/kernel_elf_helper.o src/helper.h
	g++ -g -Wall -Wextra -Wno-class-memaccess -I$(DYNINST_ROOT)/include -I$(TBB) -Ilib/ -Ilib/inih/ -Ilib/amdgpu-tooling src/bd_inplace.cpp lib/amdgpu-tooling/KernelDescriptor.cpp -L$(DYNINST_ROOT)/lib -Iinclude -Iinih/ -I/opt/intel-tbb/include lib/kernel_elf_helper.o  $(lDyninst) -o bin/bd_inplace_global

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
endif
