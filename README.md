# KernelReorderTool

This is simple set of tools based on Dyninst Instruction and ParseAPI to reorder AMDGPU kernel within a basic block.

There are 3 main compoenents of this tool set:

## split_kernel

This tool reads the elf header of a given amdgpu fat binary, and extract the kernels embedded in .haip_fatbin section into a series of kernels, named tmp_<id>.hsaco.

A meta_data file named "fmeta_data.txt" will be generated with the name of the original fat binary, number of kernels, and offset and size of each kernel.

## merge_kernel

This tool reads the info in "fmeta_data.txt", merge the splitted kernels with a copy of the original fatbinary to create a new version of fatbinary that consists of (potentially) modified kernels, named <original_name>.new.  

By running merge_kernel immediately after split_kernel, you should get a fatbinary that has exactly the same content as the original fatbinary.

## edit_kernel

This is an interactive tool that allows reordering of instructions within a kernel. Note that currently we didn't patch control-flow-target when reordering instructions, so please becareful not to reorder instructions across control-flow-instruction boundaries. 






