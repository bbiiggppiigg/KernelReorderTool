#!/usr/bin/python


k1_bs = 3
k2_bs = 1
import re



def parse_line(pat,l):
     res = re.match(r"%s"%pat,l)
     return int(res.group(1)),int(res.group(2))



def parse_time(filename):
    times = []
    with open(filename) as f:
        lines = f.readlines()
        i = 0
        while i < len(lines):
            line = lines[i]
            if(line.startswith("# of blocks = 10000")):
                break
            i+=1
        i+=1
        print("line = ",line)
        while i < len(lines):
            try:
                for j in range(k1_bs):
                    line = lines[j+i].strip()
                    conv, total = parse_line("K1:(\d+) (\d+)",line)
                line = lines[i+k1_bs].strip()
                start , end = parse_line("K1#: (\d+) (\d+)",line)
                if(end > start):

                    times.append(end-start)
                    #print(start,end)
                i+=(k1_bs+1)
            except Exception as e:
                print(i,e)
                break
        while i < len(lines):
            line = lines[i]
            if(line.startswith("before")):
                break
            i+=1
        i+=1
        while i < len(lines):
            try:
                for j in range(k2_bs):
                    line = lines[j+i].strip()
                    conv, total = parse_line("K2:(\d+) (\d+)",line)
                line = lines[i+k2_bs].strip()
                start , end = parse_line("K2#: (\d+) (\d+)",line)
                if(end > start):
                    times.append(end-start)
                    #print( start , end)
                i+=(k2_bs+1)
            except Exception as e:
                #print(i, e)
                break
        return sum(times)/ len(times)


k1_base = parse_time("base.txt")
k1_lds = parse_time("lds.txt")
k1_global = parse_time("global.txt")

print("kernel 1 native = %f, lds = %f, overhead %f"%(k1_base,k1_lds, (k1_lds-k1_base)/k1_base))
print("kernel 1 native = %f, global = %f, overhead %f"%(k1_base,k1_global, (k1_global-k1_base)/k1_base))
