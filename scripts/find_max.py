#!/usr/bin/env python
import re
import sys
filename= sys.argv[1]

def find_max_in_singles(singles):
    return max(list(map(lambda x: int(x[1:]),singles)))

def find_max_in_pairs(pairs):
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
    
with open(filename) as f:
    text = f.read()
    print(find_max_sgpr(text))
    print(find_max_vgpr(text))


