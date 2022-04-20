#!/bin/bash
TARGET=b+tree
rm *.perf
./$TARGET.base.k1.new file data/mil.txt command data/command.txt > result.base.k1.txt
rm *.perf
./$TARGET.instr.k1.new file data/mil.txt command data/command.txt> result.instr.k1.txt
./c_overhead.py

