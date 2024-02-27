#/bin/bash
TARGET=$1
CO=$2
BIN=/home/wuxx1279/KR/bin/
mkdir code-objects

set -e
NEW_LIBRARY_PATH=/home/wuxx1279/bin/dyninst-main/lib:$LD_LIBRARY_PATH
LD_LIBRARY_PATH=$NEW_LIBRARY_PATH $BIN/split_kernel.exe $TARGET &> split.log
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
    LD_LIBRARY_PATH=$NEW_LIBRARY_PATH  $BIN/../scripts/gen_config.sh $ff > $ff.config.ini
    llvm-objdump -d $ff > orig.s
    llvm-objdump -d $ff | sed 's/\/\/.*//g'> orig_clean.s
    cat $ff.config.ini >> tmp.config.ini
    LD_LIBRARY_PATH=$NEW_LIBRARY_PATH $BIN/update_kd.exe $ff
    #LD_LIBRARY_PATH=$NEW_LIBRARY_PATH DYNINST_DEBUG_PARSING=1 OMP_NUM_THREADS=1 KR_DEBUG=1 $BIN/bd_inplace.exe $ff $ff.config.ini  
    LD_LIBRARY_PATH=$NEW_LIBRARY_PATH DYNINST_DEBUG_PARSING=1 OMP_NUM_THREADS=1 KR_DEBUG=1 $BIN/thread_id.exe $ff $ff.config.ini  
    #LD_LIBRARY_PATH=/home/wuxx1279/bin/dyninst-main/lib:$LD_LIBRARY_PATH DYNINST_DEBUG_PARSING=1 OMP_NUM_THREADS=1 KR_DEBUG=1 $BIN/debug $ff $ff.config.ini  
    cp $ff $ff.tmp1
    llvm-objcopy --rename-section=.text=.old.text $ff # rename old text section
    llvm-objcopy --add-section=.text=$ff.text.new $ff # add a new textsection
    llvm-objcopy --set-section-alignment=.text=4096 $ff
    LD_LIBRARY_PATH=$NEW_LIBRARY_PATH    ~/KR/bin/update_text_phdr.exe $ff  &> update_text.log
    cp $ff $ff.tmp2
    llvm-objcopy --set-section-flags=.old.text=readonly $ff 
    LD_LIBRARY_PATH=$NEW_LIBRARY_PATH    ~/KR//bin/update_dynamic.exe $ff &> update_dynamic.log

    #cp $ff $ff.tmp3
    #llvm-objcopy --remove-section=.old.text $ff

    cp $ff $ff.tmp4
    llvm-objcopy --dump-section=.note=$ff.note $ff # dump the .note section of tmp_00
    llvm-objcopy --remove-section=.note $ff # remove the original note section

    cp $ff $ff.tmp5
    LD_LIBRARY_PATH=$NEW_LIBRARY_PATH    $BIN/expand_args.exe $ff.note &> expand_args.log
    llvm-objcopy --add-section=.note=$ff.note.expanded.new $ff # add a new section 

    cp $ff $ff.tmp6
    LD_LIBRARY_PATH=$NEW_LIBRARY_PATH    $BIN/update_note_phdr.exe $ff &> update_note.log # update the program header for note section??

    cp $ff $ff.tmp7
    llvm-objdump -d $ff > $ff.out
    fi
done
cd ..
$BIN/../scripts/modify_config.py code-objects/tmp.config.ini > config.ini
LD_LIBRARY_PATH=$NEW_LIBRARY_PATH $BIN/merge_kernel.exe &> merge.log
~/exec-rw/exec-rw  ./$TARGET ./test_bundle.txt ./new.tmp &> exec-rw.log
llvm-objcopy --rename-section=.hip_fatbin=abc ./new.tmp
llvm-objcopy --rename-section=.new_fatbin=.hip_fatbin ./new.tmp

roc-obj-ls ./new.tmp > fb.tmp
python $BIN/../scripts/gen_coffsets.py
~/exec-rw/exec-rw2 ./$TARGET ./test_bundle.txt ./new ./co_offsets &> exec-rw2.log
llvm-objcopy --rename-section=.hip_fatbin=abc ./new
llvm-objcopy --rename-section=.new_fatbin=.hip_fatbin ./new

chmod +x ./new
exit

