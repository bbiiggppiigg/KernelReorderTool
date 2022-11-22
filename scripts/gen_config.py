#!/usr/bin/env python
import re
import sys
filename= sys.argv[1]

def find_max_in_singles(singles):
    if(singles == []):
        return 0
    return max(list(map(lambda x: int(x[1:]),singles)))

def find_max_in_pairs(pairs):
    if(pairs == []):
        return 0
    return max(list(map(lambda x: int((x.split(":")[1][:-1])),pairs)))

def find_max(singles,pairs):
    return max(find_max_in_singles(singles),find_max_in_pairs(pairs))



def find_max_sgpr(text):
    singles = re.compile(r's\d+').findall(text)
    pairs = re.compile(r's\[\d+:\d+\]').findall(text)
    return(find_max(singles,pairs))

def find_max_vgpr(text):
    singles = re.compile(r'v\d+').findall(text)
    pairs = re.compile(r'v\[\d+:\d+\]').findall(text)
    return(find_max(singles,pairs))

kernel_info = dict()
with open("args.tmp","r") as f:
    lines = f.readlines()
    for line in lines:
        kname, kernarg_size , kernarg_num = line.strip().split()
        kernel_info[kname] = (int(kernarg_size) , int(kernarg_num))

with open(filename) as f:
    fid = 1
    lines = f.readlines()
    max_sgpr = 0
    max_vgpr = 0
    branch_count = 0
    kname = ""
    for i in range(len(lines)):
        if("<" in lines[i] and ("+" not in lines[i]) and "BB" not in lines[i]):
            trash,line = lines[i].split("<")
            line,trash = line.split(">")
            if(fid != 1):
                print("sgpr_max = ",max_sgpr,";")
                print("vgpr_max = ",max_vgpr,";")
                print("kernarg_size = ",kernel_info[kname][0],";")
                print("kernarg_num = ",kernel_info[kname][1],";")
                print("branch_count = ",branch_count,";")
            print("[%d]"%fid)
            print("kernel_name = ",line)
            kname = line

            fid = fid+1
            max_sgpr = 0
            max_vgpr =0
            branch_count = 0
        else:
            max_sgpr = max(max_sgpr,find_max_sgpr(lines[i]))
            max_vgpr = max(max_vgpr,find_max_vgpr(lines[i]))
            if("s_and_saveexec" in lines[i]):
                branch_count += 1

    if(max_sgpr !=0 or max_vgpr !=0):
        print("sgpr_max = ",max_sgpr,";")
        print("vgpr_max = ",max_vgpr,";")
        print("kernarg_size = ",kernel_info[kname][0],";")
        print("kernarg_num = ",kernel_info[kname][1],";")
        print("branch_count = ",branch_count,";")

    #print(find_max_sgpr(text))
    #print(find_max_vgpr(text))


