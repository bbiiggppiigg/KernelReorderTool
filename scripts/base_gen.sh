#/bin/bash
TARGET=$1
CO=$2
BIN=/home/wuxx1279/KR/bin/
mkdir code-objects

set -e
$BIN/split_kernel $TARGET &> split.log
cd code-objects

if [[ $CO == "" ]]; then
    CO="gfx908"
fi


echo "TARGET = $TARGET"
echo "CODE OBJECT = $CO"

rm -f tmp.config.ini
cc=0
for ff in *; do
    if [[ "$ff" == *"$CO" ]]; then
    #if [[ "$ff" == *"gfx908" ]] || [[ "$ff" == *"gfx90a" ]]; then
    echo "Processing " $ff
    cp $ff $ff.backup
    $BIN/../scripts/gen_config.sh $ff > $ff.config.ini
    llvm-objdump -d $ff > orig.s
    llvm-objdump -d $ff | sed 's/\/\/.*//g'> orig_clean.s
    cat $ff.config.ini >> tmp.config.ini
    $BIN/update_kd $ff
    LD_LIBRARY_PATH=/home/wuxx1279/bin/dyninst-main/lib:$LD_LIBRARY_PATH DYNINST_DEBUG_PARSING=1 OMP_NUM_THREADS=1 KR_DEBUG=1 $BIN/bd_base $ff $ff.config.ini  
    #LD_LIBRARY_PATH=/home/wuxx1279/bin/dyninst-main/lib:$LD_LIBRARY_PATH DYNINST_DEBUG_PARSING=1 OMP_NUM_THREADS=1 KR_DEBUG=1 $BIN/debug $ff $ff.config.ini  
    llvm-objcopy --rename-section=.text=.old.text $ff # rename old text section
    llvm-objcopy --add-section=.text=$ff.text.new $ff # add a new textsection
    llvm-objcopy --set-section-alignment=.text=4096 $ff
    ~/KR/bin/update_text_phdr $ff  &> update_text.log
    llvm-objcopy --set-section-flags=.old.text=readonly $ff 
    ~/KR//bin/update_dynamic $ff &> update_dynamic.log
    #llvm-objcopy --remove-section=.old.text $ff

    llvm-objcopy --dump-section=.note=$ff.note $ff # dump the .note section of tmp_00
    llvm-objcopy --remove-section=.note $ff # remove the original note section
    $BIN/expand_args $ff.note &> expand_args.log
    llvm-objcopy --add-section=.note=$ff.note.expanded.new $ff # add a new section 
    $BIN/update_note_phdr $ff &> update_note.log # update the program header for note section??

    llvm-objdump -d $ff > $ff.out
    fi
done
cd ..
$BIN/../scripts/modify_config.py code-objects/tmp.config.ini > config.ini
$BIN/merge_kernel &> merge.log
~/exec-rw/exec-rw  ./$TARGET ./test_bundle.txt ./base.tmp &> exec-rw.log
llvm-objcopy --rename-section=.hip_fatbin=abc ./base.tmp
llvm-objcopy --rename-section=.new_fatbin=.hip_fatbin ./base.tmp

roc-obj-ls ./base.tmp > fb.tmp
python $BIN/../scripts/gen_coffsets.py
~/exec-rw/exec-rw2 ./$TARGET ./test_bundle.txt ./base ./co_offsets &> exec-rw2.log
llvm-objcopy --rename-section=.hip_fatbin=abc ./base
llvm-objcopy --rename-section=.new_fatbin=.hip_fatbin ./base

chmod +x ./base
exit

