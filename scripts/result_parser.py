#!/usr/bin/python

from statistics import stdev as stdev
import re

def parse_line(line):
    res = re.match(r"(\d+): (\d+) (\d+)",line)
    return int(res.group(1)),int(res.group(2)),int(res.group(3))

kid_base = []
res_base = []
kid_instr = []
res_instr = []




with open("base.out") as f:
    lines = f.readlines()
    lid = 0
    res = []
    while lid < len(lines):
        while not lines[lid].startswith("num_records = "):
            lid+=1
            continue
        lid+=1
        while not lines[lid].startswith("done"):
            kid, start , end = parse_line(lines[lid])
            if(res == []):
                kid_base.append(kid)
            if start !=0 and end > start:
                res.append(end-start)
            lid+=1

        if(len(res)!=0):
            print(kid,sum(res)/len(res), stdev(res))
            res_base.append(res)
        res = []
        lid+=1

with open("instr.out") as f:
    lines = f.readlines()
    lid = 0
    res = []
    while lid < len(lines):
        while not lines[lid].startswith("num_records = "):
            lid+=1
            continue
        lid+=1
        while not lines[lid].startswith("done"):
            kid, start , end = parse_line(lines[lid])
            if(res == []):
                kid_instr.append(kid)
            if start !=0 and end > start:
                res.append(end-start)
            lid+=1
        if(len(res)!=0):
            print(kid,sum(res)/len(res), stdev(res))
            res_instr.append(res)
        res = []
        lid+=1

i = 0
for i in range(len(res_instr)):
    instr_avg = sum(res_instr[i]) * 1.0 /len(res_instr[i])
    base_avg = sum(res_base[i]) * 1.0 /len(res_base[i])
    print("base avg = %f, stdev = %f, instr avg = %f, stdev = %f"%(base_avg,stdev(res_base[i]),instr_avg,stdev(res_instr[i])))
    print("kid = %d overhead = %f"%(kid_base[i],(instr_avg -base_avg)/base_avg))
    print("")
