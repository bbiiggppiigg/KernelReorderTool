#!/usr/bin/env python
import re
import sys
filename= sys.argv[1]

def find_id(text):
    res = re.match(r'\[(\d+)\]',text)
    return res


iid = 1 

with open(filename) as f:
    lines = f.readlines()
    for line in lines:
        res = find_id(line)
        if(res is not None):
            print(f"[{iid}]")
            iid+=1
        else:
            print(line.strip())


