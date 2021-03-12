all: split_kernel edit_kernel merge_kernel set_register_usage extend_text

#
# A GNU Makefile
#

ifeq ($(DYNINST_ROOT),)
$(error DYNINST_ROOT is not set)
endif

NCURSES_PATH=/home/wuxx1279/.local/


edit_kernel: edit_kernel.o
	 g++ edit_kernel.o  -g -L$(DYNINST_ROOT)/lib  \
		 -ldyninstAPI -lsymtabAPI -lparseAPI -linstructionAPI -lcommon \
		 -lboost_filesystem -lboost_system -o edit_kernel

edit_kernel.o:  edit_kernel.cpp
	g++ -g -Wall  -I$(DYNINST_ROOT)/include -c edit_kernel.cpp

set_register_usage: set_register_usage.cpp
	g++ -o set_register_usage set_register_usage.cpp

extend_text: extend_text.cpp
	g++ -o extend_text extend_text.cpp



split_kernel: split_kernel.cpp
	g++ -o split_kernel split_kernel.cpp

merge_kernel: merge_kernel.cpp
	g++ -o merge_kernel merge_kernel.cpp


clean:
	rm -f *.bundle *.hsaco *.isa split_kernel merge_kernel edit_kernel *.o

