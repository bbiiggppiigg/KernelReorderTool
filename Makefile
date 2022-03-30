HIPCC=/opt/rocm/hip/bin/hipcc

targets=split_kernel edit_kernel merge_kernel set_register_usage extend_text extend_symbol test_acc test_counter test_getreg move_block report_kd test_time analyze_metadata update_note_modify_lds_size updated_insert_tramp updated_base measure_overhead m_init m_branch m_w m_wb_pb m_conseq

TARGETS=$(addprefix bin/,$(targets))

all : $(TARGETS) $(mm)

DYNINST_ROOT=/home/wuxx1279/bin/dyninst
ifeq ($(DYNINST_ROOT),)
$(error DYNINST_ROOT is not set)
endif

TBB=/opt/spack/opt/spack/linux-centos8-zen2/gcc-10.2.0/intel-tbb-2020.3-zbj2dajg6q53hhsfd7kglxwqhyc7ie3v/include
TBB=/opt/intel-tbb/include
lDyninst= -ldyninstAPI -lsymtabAPI -lparseAPI -linstructionAPI -lcommon -lboost_filesystem -lboost_system 

mmpath = ../HIP-Examples/HIP-Examples-Applications/MatrixMultiplication/MatrixMultiplication


lib/kernel_elf_helper.o: lib/kernel_elf_helper.h lib/kernel_elf_helper.cpp 
	g++ -g -Wall  -c lib/kernel_elf_helper.cpp -o lib/kernel_elf_helper.o -I msgpack-c/include/  -I/opt/intel-tbb/include

bin/measure_overhead: src/measure_overhead.cpp lib/InsnFactory.h lib/kernel_elf_helper.o  
	g++ -g -Wall -I$(DYNINST_ROOT)/include -I$(TBB) src/measure_overhead.cpp -L$(DYNINST_ROOT)/lib -Iinclude -Iinih/ -I/opt/intel-tbb/include lib/InstrUtil.o lib/kernel_elf_helper.o $(lDyninst) -o bin/measure_overhead


bin/m_init: src/measure_overhead.cpp lib/InsnFactory.h lib/kernel_elf_helper.o  
	g++ -g -Wall -Dm_INIT -I$(DYNINST_ROOT)/include -I$(TBB) src/measure_overhead.cpp -L$(DYNINST_ROOT)/lib -Iinclude -Iinih/ -I/opt/intel-tbb/include lib/InstrUtil.o lib/kernel_elf_helper.o $(lDyninst) -o $@

bin/m_branch: src/measure_overhead.cpp lib/InsnFactory.h lib/kernel_elf_helper.o  
	g++ -g -Wall -Dm_BRANCH -I$(DYNINST_ROOT)/include -I$(TBB) src/measure_overhead.cpp -L$(DYNINST_ROOT)/lib -Iinclude -Iinih/ -I/opt/intel-tbb/include lib/InstrUtil.o lib/kernel_elf_helper.o $(lDyninst) -o $@

bin/m_w: src/measure_overhead.cpp lib/InsnFactory.h lib/kernel_elf_helper.o  
	g++ -g -Wall -Dm_WRITEBACK -I$(DYNINST_ROOT)/include -I$(TBB) src/measure_overhead.cpp -L$(DYNINST_ROOT)/lib -Iinclude -Iinih/ -I/opt/intel-tbb/include lib/InstrUtil.o lib/kernel_elf_helper.o $(lDyninst) -o $@

bin/m_wb_pb: src/measure_overhead.cpp lib/InsnFactory.h lib/kernel_elf_helper.o  
	g++ -g -Wall -Dm_WRITE_PER_B -I$(DYNINST_ROOT)/include -I$(TBB) src/measure_overhead.cpp -L$(DYNINST_ROOT)/lib -Iinclude -Iinih/ -I/opt/intel-tbb/include lib/InstrUtil.o lib/kernel_elf_helper.o $(lDyninst) -o $@

bin/m_conseq: src/measure_overhead.cpp lib/InsnFactory.h lib/kernel_elf_helper.o  
	g++ -g -Wall -Dm_CONSEQ -I$(DYNINST_ROOT)/include -I$(TBB) src/measure_overhead.cpp -L$(DYNINST_ROOT)/lib -Iinclude -Iinih/ -I/opt/intel-tbb/include lib/InstrUtil.o lib/kernel_elf_helper.o $(lDyninst) -o $@




bin/updated_insert_tramp: src/updated_insert_tramp.cpp lib/InsnFactory.h lib/kernel_elf_helper.o  
	g++ -g -Wall -I$(DYNINST_ROOT)/include -I$(TBB) src/updated_insert_tramp.cpp -L$(DYNINST_ROOT)/lib -Iinclude -Iinih/ -I/opt/intel-tbb/include lib/InstrUtil.o lib/kernel_elf_helper.o $(lDyninst) -o bin/updated_insert_tramp

bin/updated_base: src/updated_base.cpp lib/InsnFactory.h lib/kernel_elf_helper.o 
	g++ -g -Wall -I$(DYNINST_ROOT)/include -I$(TBB) src/updated_base.cpp -L$(DYNINST_ROOT)/lib -Iinclude -Iinih/ lib/InstrUtil.o lib/kernel_elf_helper.o $(lDyninst) -o bin/updated_base


MatrixMultiplication: $(mmpath)
	cp $(mmpath) .

bin/analyze_metadata: src/analyze_metadata.cpp
	g++ src/analyze_metadata.cpp -o bin/analyze_metadata

bin/edit_kernel: src/edit_kernel.o
	 g++ src/edit_kernel.o  -g -L$(DYNINST_ROOT)/lib  $(lDyninst) -o bin/edit_kernel

src/edit_kernel.o:  src/edit_kernel.cpp
	g++ -g -Wall -I$(DYNINST_ROOT)/include -I$(TBB) -c src/edit_kernel.cpp -o src/edit_kernel.o



bin/test_acc: src/test_acc.o lib/InstrUtil.o
	 g++ src/test_acc.o  lib/InstrUtil.o -g -L$(DYNINST_ROOT)/lib  $(lDyninst) -o bin/test_acc

src/test_acc.o:  src/test_acc.cpp 
	g++ -g -Wall -I$(DYNINST_ROOT)/include -I$(TBB) -Iinclude -c src/test_acc.cpp -o src/test_acc.o

bin/test_time: src/test_time.o lib/InstrUtil.o
	 g++ src/test_time.o  lib/InstrUtil.o -g -L$(DYNINST_ROOT)/lib  $(lDyninst) -o bin/test_time

src/test_time.o:  src/test_time.cpp 
	g++ -g -Wall -I$(DYNINST_ROOT)/include -Iinclude -I$(TBB) -c src/test_time.cpp -o src/test_time.o

bin/test_counter: src/test_counter.o lib/InstrUtil.o
	 g++ src/test_counter.o  lib/InstrUtil.o -g -L$(DYNINST_ROOT)/lib  $(lDyninst) -o bin/test_counter

src/test_counter.o:  src/test_counter.cpp 
	g++ -g -Wall -I$(DYNINST_ROOT)/include -Iinclude -I$(TBB) -c src/test_counter.cpp -o src/test_counter.o

bin/test_getreg: src/test_getreg.o lib/InstrUtil.o
	 g++ src/test_getreg.o  lib/InstrUtil.o -g -L$(DYNINST_ROOT)/lib  $(lDyninst) -o bin/test_getreg

src/test_getreg.o:  src/test_getreg.cpp 
	g++ -g -Wall -I$(DYNINST_ROOT)/include -I$(TBB) -Iinclude -c src/test_getreg.cpp -o src/test_getreg.o

lib/InstrUtil.o: lib/InstrUtil.cpp lib/InsnFactory.h
	g++ -g -Wall  -c lib/InstrUtil.cpp -o lib/InstrUtil.o

bin/move_block: src/move_block.cpp lib/InstrUtil.o
	g++ -o bin/move_block -Iinclude src/move_block.cpp lib/InstrUtil.o

bin/set_register_usage: src/set_register_usage.cpp lib/kernel_elf_helper.o
	g++ -o bin/set_register_usage -Iinclude lib/kernel_elf_helper.o src/set_register_usage.cpp

bin/extend_text: src/extend_text.cpp lib/kernel_elf_helper.o
	g++ -o bin/extend_text -Iinclude lib/kernel_elf_helper.o src/extend_text.cpp

bin/report_kd: src/report_kd.cpp
	g++ -o $@ $<

bin/extend_symbol: src/extend_symbol.cpp
	g++ -o $@ $<

bin/split_kernel: src/split_kernel.cpp
	g++ -o $@ $<

bin/merge_kernel: src/merge_kernel.cpp
	g++ -o $@ $<

bin/update_note_modify_lds_size: src/update_note_modify_lds_size.cpp
	g++ -I ../msgpack-c/include/ $< -o $@


clean:
	rm -f *.bundle *.hsaco *.isa src/*.o lib/*.o bin/*

