HIPCC=${HIP_PATH}/bin/hipcc

srcs=update_msgpack_gpr_usage thread_bb_counter thread_id expand_args update_note_phdr report_args_loc disassemble split_kernel merge_kernel bd_inplace bd_inplace_global update_text_phdr update_dynamic debug update_kd pure_bd
special=bd_base
libs=kernel_elf_helper.o

EXES := $(foreach item,$(srcs),bin/$(item).exe)
all: bin/kernel_elf_helper.o $(EXES)

DYNINST_ROOT=/home/wuxx1279/bin/dynmaster
ifeq ($(DYNINST_ROOT),)
$(error DYNINST_ROOT is not set)
endif

lDyninst= -ldyninstAPI -lsymtabAPI -lparseAPI -linstructionAPI -lcommon -lboost_filesystem -lboost_system  -ldynElf
iLib= -I$(DYNINST_ROOT)/include -I$(TBB) -I -I amdgpu-tooling -I msgpack-c -Iinclude -I/opt/rocm/include -Ilib/ -Ilib/inih -Ilib/amdgpu-tooling
lLib= -L$(DYNINST_ROOT)/lib64 -L/opt/rocm/lib/
options= -std=c++17 -g -Wall -Wextra -Wno-class-memaccess
loptions= -Wl,--demangle -Wl,-rpath,/opt/rocm/lib
links= $(lDyninst) -lamd_comgr

bin/parse_kernel.exe: src/parse_kernel.cpp $(CURDIR)/bin/kernel_elf_helper.o
	g++ $(options) $(iLib) $^ $(iLib) $(lLib) $(links) $(loptions) -o $@

bin/%.exe: src/%.cpp $(CURDIR)/bin/kernel_elf_helper.o src/helper.h
	g++ $(options) $(iLib) $^ $(iLib) $(lLib) $(links) $(loptions) -o $@

bin/kernel_elf_helper.o: src/kernel_elf_helper.h src/kernel_elf_helper.cpp 
	g++ -g -Wall  -c src/kernel_elf_helper.cpp -o $@ -I msgpack-c/include/  -I/opt/intel-tbb/include
clean:
	rm -f *.bundle *.hsaco *.isa src/*.o lib/*.o bin/*.o bin/*.exe


