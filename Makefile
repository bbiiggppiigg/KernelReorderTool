HIPCC=/opt/rocm/hip/bin/hipcc

targets=split_kernel edit_kernel merge_kernel set_register_usage extend_text extend_symbol test_acc test_counter test_getreg move_block report_kd test_time analyze_metadata insert_tramp

TARGETS=$(addprefix bin/,$(targets))

all : $(TARGETS) $(mm)

DYNINST_ROOT=/home/wuxx1279/bin/dyninst
ifeq ($(DYNINST_ROOT),)
$(error DYNINST_ROOT is not set)
endif

lDyninst= -ldyninstAPI -lsymtabAPI -lparseAPI -linstructionAPI -lcommon -lboost_filesystem -lboost_system

mmpath = ../HIP-Examples/HIP-Examples-Applications/MatrixMultiplication/MatrixMultiplication

bin/insert_tramp: src/insert_tramp.cpp lib/InsnFactory.h
	g++ -g -Wall -I$(DYNINST_ROOT)/include src/insert_tramp.cpp -L$(DYNINST_ROOT)/lib -Iinclude lib/InstrUtil.o $(lDyninst) -o bin/insert_tramp

MatrixMultiplication: $(mmpath)
	cp $(mmpath) .

bin/analyze_metadata: src/analyze_metadata.cpp
	g++ src/analyze_metadata.cpp -o bin/analyze_metadata

bin/edit_kernel: src/edit_kernel.o
	 g++ src/edit_kernel.o  -g -L$(DYNINST_ROOT)/lib  $(lDyninst) -o bin/edit_kernel

src/edit_kernel.o:  src/edit_kernel.cpp
	g++ -g -Wall -I$(DYNINST_ROOT)/include -c src/edit_kernel.cpp -o src/edit_kernel.o


bin/test_acc: src/test_acc.o lib/InstrUtil.o
	 g++ src/test_acc.o  lib/InstrUtil.o -g -L$(DYNINST_ROOT)/lib  $(lDyninst) -o bin/test_acc

src/test_acc.o:  src/test_acc.cpp 
	g++ -g -Wall -I$(DYNINST_ROOT)/include -Iinclude -c src/test_acc.cpp -o src/test_acc.o

bin/test_time: src/test_time.o lib/InstrUtil.o
	 g++ src/test_time.o  lib/InstrUtil.o -g -L$(DYNINST_ROOT)/lib  $(lDyninst) -o bin/test_time

src/test_time.o:  src/test_time.cpp 
	g++ -g -Wall -I$(DYNINST_ROOT)/include -Iinclude -c src/test_time.cpp -o src/test_time.o

bin/test_counter: src/test_counter.o lib/InstrUtil.o
	 g++ src/test_counter.o  lib/InstrUtil.o -g -L$(DYNINST_ROOT)/lib  $(lDyninst) -o bin/test_counter

src/test_counter.o:  src/test_counter.cpp 
	g++ -g -Wall -I$(DYNINST_ROOT)/include -Iinclude -c src/test_counter.cpp -o src/test_counter.o

bin/test_getreg: src/test_getreg.o lib/InstrUtil.o
	 g++ src/test_getreg.o  lib/InstrUtil.o -g -L$(DYNINST_ROOT)/lib  $(lDyninst) -o bin/test_getreg

src/test_getreg.o:  src/test_getreg.cpp 
	g++ -g -Wall -I$(DYNINST_ROOT)/include -Iinclude -c src/test_getreg.cpp -o src/test_getreg.o

lib/InstrUtil.o: lib/InstrUtil.cpp lib/InsnFactory.h
	g++ -g -Wall  -c lib/InstrUtil.cpp -o lib/InstrUtil.o



bin/move_block: src/move_block.cpp lib/InstrUtil.o
	g++ -o bin/move_block -Iinclude src/move_block.cpp lib/InstrUtil.o

bin/report_kd: src/report_kd.cpp
	g++ -o bin/report_kd src/report_kd.cpp



bin/set_register_usage: src/set_register_usage.cpp
	g++ -o bin/set_register_usage src/set_register_usage.cpp

bin/extend_text: src/extend_text.cpp
	g++ -o bin/extend_text src/extend_text.cpp

bin/extend_symbol: src/extend_symbol.cpp
	g++ -o bin/extend_symbol src/extend_symbol.cpp


bin/split_kernel: src/split_kernel.cpp
	g++ -o bin/split_kernel src/split_kernel.cpp

bin/merge_kernel: src/merge_kernel.cpp
	g++ -o bin/merge_kernel src/merge_kernel.cpp


clean:
	rm -f *.bundle *.hsaco *.isa src/*.o lib/*.o bin/*

