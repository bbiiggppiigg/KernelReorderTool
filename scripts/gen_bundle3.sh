#/bin/bash
TARGET=$1
CO=$2
BIN=/home/wuxx1279/KR/bin/
mkdir code-objects

$BIN/split_kernel_v3 $TARGET
cd code-objects

if [[ $CO == "" ]]; then
    CO="gfx908"
fi



echo "TARGET = $TARGET"
echo "CODE OBJECT = $CO"

for ff in *; do
    if [[ "$ff" == *"$CO" ]]; then
    echo "Processing " $ff
    cp $ff $ff.backup
    $BIN/../scripts/gen_config.sh $ff > $ff.config.ini
    llvm-objdump -d $ff > orig.s
    llvm-objdump -d $ff | sed 's/\/\/.*//g'> orig_clean.s
    #LD_LIBRARY_PATH=/home/wuxx1279/bin/dyninst/lib:$LD_LIBRARY_PATH DYNINST_DEBUG_PARSING=1 OMP_NUM_THREADS=1 $BIN/preload_global_v3 $ff $ff.config.ini
    #llvm-objcopy --dump-section=.note=$ff.note $ff # dump the .note section of tmp_00
    #llvm-objcopy --remove-section=.note $ff # remove the original note section
    #$BIN/expand_args $ff.note
    #llvm-objcopy --add-section=.note=$ff.note.expanded.new $ff # add a new section 
    #$BIN/update_note_phdr $ff # update the program header for note section??
    fi
done
cd ..
$BIN/merge_kernel_v3
~/exec-rw/exec-rw ./$TARGET ./test_bundle.txt ./new3
chmod +x ./new3
exit


#llvm-objdump -d $CO | sed 's/\/\/.*//g'> tmp_clean.s
#llvm-objdump -d $CO > tmp.s
#~/exec-rw/exec-rw ../$TARGET ./bundle.txt ./new2
#chmod +x ./new3







