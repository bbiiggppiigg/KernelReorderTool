#/bin/bash
BIN=/home/wuxx1279/KR/bin/
CO=$1
llvm-objdump -d $CO > orig.s
llvm-objdump -d $CO | sed 's/\/\/.*//g'> orig_clean.s
llvm-objcopy --dump-section=.note=$CO.note $CO 
$BIN/report_args_loc.exe $CO.note > args.tmp
$BIN/../scripts/gen_config.py orig.s

