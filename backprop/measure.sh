#!/bin/bash


#T=1
#if [ $T -eq 0 ]
#then
#    GPU=0
#    CO=tmp_0$GPU.hsaco
#    LOC=bfs/900
#    TARGET=bfs
#else
#    GPU=1
#    CO=tmp_0$GPU.hsaco
#    LOC=bfs/908
#    TARGET=bfs
#fi


measure(){
GOAL=$1
OPTION=$2
../bin/split_kernel $TARGET.instr
cp $CO backup.hsaco
LD_LIBRARY_PATH=/home/wuxx1279/bin/dyninst/lib:$LD_LIBRARY_PATH DYNINST_DEBUG_PARSING=1 OMP_NUM_THREADS=1 ../bin/m_$GOAL $CO config.ini $OPTION
../bin/merge_kernel
ROCR_VISIBLE_DEVICES=$GPU ./bfs.instr.new ./data/graph1k.txt > res/$LOC/$GOAL$OPTION.txt
}

GPU=0
CO=tmp_0$GPU.hsaco
LOC=bfs/900
TARGET=bfs

measure init 
measure branch 0
measure branch 1
measure wb 
measure wb_pb
measure conseq 

GPU=1
CO=tmp_0$GPU.hsaco
LOC=bfs/908
TARGET=bfs

measure init 
measure branch 0
measure branch 1
measure wb 
measure wb_pb
measure conseq 



rsync -avz res/ orion:res/


