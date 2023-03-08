#!/bin/bash


#../build/bin/snippetInserter tmp_01.hsaco 12288
#../../bin/update_text_phdr tmp_01.hsaco 
# update the program header for note section??
#llvm-objcopy --dump-section=.note=tmp_01.note tmp_01.hsaco # dump the .note section of tmp_01
#llvm-objcopy --remove-section=.note tmp_01.hsaco # remove the original note section
#llvm-objcopy --add-section=.note=tmp_01.note.shrinked.new tmp_01.hsaco # add a new section 
#llvm-objcopy --add-section=.note=tmp_01.note.expanded.new tmp_01.hsaco # add a new section 
#../bin/update_note_phdr tmp_01.hsaco # update the program header for note section??

#llvm-objcopy --dump-section=.note=tmp_02.note tmp_02.hsaco # dump the .note section of tmp_01
#llvm-objcopy --remove-section=.note tmp_02.hsaco # remove the original note section
#llvm-objcopy --add-section=.note=tmp_02.note.shrinked.new tmp_02.hsaco # add a new section 
#llvm-objcopy --add-section=.note=tmp_02.note.expanded.new tmp_02.hsaco # add a new section 
#../bin/update_note_phdr tmp_02.hsaco # update the program header for note section??
../../bin/merge_kernel_v2
~/exec-rw/exec-rw vectoradd_hip.exe bundle.txt new

#xxd tmp_01.hsaco > tmp_01.xxd
#xxd tmp_01_backup.hsaco > tmp_01_backup.xxd
#diff tmp_01.xxd tmp_01_backup.xxd


