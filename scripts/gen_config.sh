#/bin/bash
BIN=/home/wuxx1279/KR/bin/
CO=$1
llvm-objdump -d --mcpu=gfx900 $CO > orig.s
llvm-objdump -d --mcpu=gfx900 $CO | sed 's/\/\/.*//g'> orig_clean.s
llvm-objcopy --dump-section=.note=$CO.note $CO # dump the .note section of tmp_00
$BIN/report_args_loc $CO.note > args.tmp
$BIN/../scripts/gen_config.py orig.s

