#!/usr/bin/python3

import re
import sys
diff =  []



def get_result(filename):
    ret = [] 
    with open(filename) as f:
        lines = f.readlines()
        for line in lines:
            try:
                #print(line)
                line = line.strip()
                res = re.match(r"0x([0-9A-Fa-f]+): (\d+) (\d+)",line)
                start , end = res.group(2),res.group(3)
                if end < start:
                    continue
                ret.append( int(end) - int(start))
            except:
                continue
    return ret


res_base = get_result("result.base")
res_new = get_result("result.new")

avg_base = sum(res_base) / len(res_base)
avg_new = sum(res_new)/ len(res_new)
print("base avg cycle = ",avg_base, " new avg cycle = ",avg_new , " overhead = ", (avg_new - avg_base )/avg_base )


