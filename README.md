# KernelReorderTool

This is simple set of tools based on Dyninst Instruction and ParseAPI to reorder AMDGPU kernel within a basic block.

There are 3 main compoenents of this tool set:

## split_kernel

This tool reads the elf header of a given amdgpu fat binary, and extract the kernels embedded in .haip_fatbin section into a series of kernels, named tmp_<id>.hsaco.

A meta_data file named "fmeta_data.txt" will be generated with the name of the original fat binary, number of kernels, and offset and size of each kernel.

Example Usage:
```
split_kernel ./MatrixMultiplication
```
After running this, you should get a kernel named "tmp00.hsaco" 

## merge_kernel

This tool reads the info in "fmeta_data.txt", merge the splitted kernels with a copy of the original fatbinary to create a new version of fatbinary that consists of (potentially) modified kernels, named <original_name>.new.  

By running merge_kernel immediately after split_kernel, you should get a fatbinary that has exactly the same content as the original fatbinary.

Usage:
```
./merge_kernel
```
## edit_kernel

This is an interactive tool that allows reordering of instructions within a kernel. Note that currently we didn't patch control-flow-target when reordering instructions, so please becareful not to reorder instructions across control-flow-instruction boundaries. 

Executing this should bring you into an infinite loop where you can list the functions in the kernel, reorder instructions , and upon quit the updates are saved.

Example Usage:

./edit_kernel tmp00.hsaco

```
list
1
```

You'll see a list of instructions in the function with an integer index in front of each instruction
```
edit
1
before
9 6
```

The above input sequence will  move the instruction with index 9 before the instruction with index 6.
The input sequence for moving an instruction after another instruction is similar.
If you list again, you should be able to see the changes that you've made reflected.
```
list
```
After you are done editing the kernel, you quit the interactive loop so the kernel is updated.
```
quit
```













