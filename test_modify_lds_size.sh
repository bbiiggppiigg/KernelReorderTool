#!/bin/bash
#make
llvm-objcopy --dump-section .note=tmp_00.note.old tmp_00.hsaco
llvm-objcopy -R .note tmp_00.hsaco
update_note_modify_lds_size tmp_00.note tmp_00.note.new 700
llvm-objcopy --add-section .note=tmp_00.note.new tmp_00.hsaco
LD_LIBRARY_PATH=/home/wuxx1279/support/lib/comgr/build/:$LD_LIBRARY_PATH ~/test/msgpack_test tmp_00.hsaco
