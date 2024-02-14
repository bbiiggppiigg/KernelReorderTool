#!/usr/bin/python

from statistics import stdev as stdev
import re

def parse_line(line):
    try:
        #res = re.match(r"(\d+): (\d+) (\d+)",line)
        res = re.match(r"(\d+): ([0-9a-fA-F]+) ([0-9a-fA-F]+)",line)
        return int(res.group(1)),int("0x"+res.group(2),16),int("0x"+res.group(3),16)
    except:
        return [],[],[]

kid_base = []
res_base = []
kid_instr = []
res_instr = []
knames = []
start_min = 0x7fffffffffffffff
end_max = -1
freq = 1.502 * 1e9
def parse_result(filename, kid_list , res_list):
    with open(filename) as f:
        lines = f.readlines()
        lid = 0
        res = []
        kid = 0
        while lid < len(lines):
            while not lines[lid].startswith("num_records = "):
                lid+=1
                continue
            kname = lines[lid].strip().split(",")[1]
            knames.append(kname)
            lid+=1
            res = []
            kid_list.append(kid)
            kid+=1
            start_min = 0x7fffffffffffffff
            end_max = -1

            while True:
                wid, start , end = parse_line(lines[lid])
                if( wid == []):
                    break
                if start !=0 and end > start:
                    if start < start_min:
                        start_min = start
                    if end > end_max:
                        end_max = end
                    res.append((end-start) * 1e6 / freq)
                lid+=1
            lid+=1
            if(len(res)!=0):
                #print(kid,sum(res)/len(res), stdev(res))
                res_list.append(res)
                print("duration = ",(end_max-start_min)* 1e6/ freq)

            pass
    
parse_result("base.out",kid_base,res_base)
parse_result("instr.out",kid_instr,res_instr)

i = 0
for i in range(len(res_instr)):
    instr_avg = sum(res_instr[i]) * 1.0 /len(res_instr[i]) 
    base_avg = sum(res_base[i]) * 1.0 /len(res_base[i]) 
    print("kname = %s, base avg = %f, stdev = %f, instr avg = %f, stdev = %f"%(knames[i],base_avg,stdev(res_base[i]),instr_avg,stdev(res_instr[i])))
    print("kid = %d overhead = %f"%(kid_base[i],(instr_avg -base_avg)/base_avg))
    print("")
