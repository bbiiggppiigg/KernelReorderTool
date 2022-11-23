#/bin/bash
TARGET=$1
CO=$2
if [ $CO == "" ]; then
    CO=tmp_00.hsaco
fi
echo $CO
BIN=/home/wuxx1279/KR/bin/
mkdir tmp 
cp $TARGET tmp 
cd tmp
$BIN/split_kernel_v2 $TARGET
cp $CO backup.hsaco
$BIN/../scripts/gen_config.sh $CO > config.ini

llvm-objdump -d $CO > orig.s
llvm-objdump -d $CO | sed 's/\/\/.*//g'> orig_clean.s
LD_LIBRARY_PATH=/home/wuxx1279/bin/dyninst/lib:$LD_LIBRARY_PATH DYNINST_DEBUG_PARSING=1 OMP_NUM_THREADS=1 $BIN/preload_base $CO config.ini

llvm-objcopy --dump-section=.note=$CO.note $CO # dump the .note section of tmp_00
llvm-objcopy --remove-section=.note $CO # remove the original note section
$BIN/expand_args $CO.note
llvm-objcopy --add-section=.note=$CO.note.expanded.new $CO # add a new section 
$BIN/update_note_phdr $CO # update the program header for note section??



$BIN/merge_kernel_v2
llvm-objdump -d $CO | sed 's/\/\/.*//g'> tmp_clean.s
llvm-objdump -d $CO > tmp.s
~/exec-rw/exec-rw ../$TARGET ./bundle.txt ./base
chmod +x ./base







