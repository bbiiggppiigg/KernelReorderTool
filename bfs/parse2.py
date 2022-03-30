#!/home/wuxx1279/KernelReorderTool/pip/bin/python2

import re
import sys
diff =  []

import matplotlib
matplotlib.use('agg')
import matplotlib.pyplot as plt

def parse_line(pat,l):
    res = re.match(pat,l)
    return int(res.group(1)),int(res.group(2))

def get_result(filename):
    with open(filename) as f:
        lines = f.readlines()
        i = 0
        run_id = 0


        times = []
        runs = []
        while i < len(lines):
            line = lines[i].strip()
            while i +1 < len(lines) and line != "#RUN#":
                i = i +1
                line = lines[i].strip()
                continue
            # Here lines[i] = "#RUN#"

            print("i = ",i,"line= ",line)
            if line == "Result stored in result.txt":
                break
            k1_time = []
            k1_run = []
            while i +3 < len(lines):
                try:
                    b1_line = lines[i+1].strip()
                    b2_line = lines[i+2].strip()
                    t_line = lines[i+3].strip()

                    #print(b1_line)
                    #print(b2_line)
                    #print(t_line)
                    b1_conv, b1_total = parse_line(r"K1:(\d+) (\d+)",b1_line)
                    b2_conv, b2_total = parse_line(r"K1:(\d+) (\d+)",b2_line)
                    start, end = parse_line(r"K1#: (\d+) (\d+)",t_line)

                    k1_time.append(end-start)
                    k1_run.append(b1_total+b2_total)
                    i = i + 3
                except Exception as e:
                    break

            fig, ax =plt.subplots()
            ax.scatter(k1_time,k1_run)
            plt.savefig("png/k1_run_%02d"%(run_id+1))

            times.append(k1_time)
            print("avg time = ",sum(k1_time) / len(k1_time))
            runs.append(k1_run)
            run_id += 1

    return run_id, times, runs 




def get_result_per_b(filename, bid ):
    with open(filename) as f:
        lines = f.readlines()
        i = 0
        run_id = 0


        times = []
        runs = []
        while i < len(lines):
            line = lines[i].strip()
            while i +1 < len(lines) and line != "#RUN#":
                i = i +1
                line = lines[i].strip()
                continue
            # Here lines[i] = "#RUN#"

            print("i = ",i,"line= ",line)
            if line == "Result stored in result.txt":
                break
            k1_time = []
            k1_run = []
            while i +3 < len(lines):
                try:
                    b1_line = lines[i+1].strip()
                    b2_line = lines[i+2].strip()
                    t_line = lines[i+3].strip()

                    #print(b1_line)
                    #print(b2_line)
                    #print(t_line)
                    b1_conv, b1_total = parse_line(r"K1:(\d+) (\d+)",b1_line)
                    b2_conv, b2_total = parse_line(r"K1:(\d+) (\d+)",b2_line)
                    start, end = parse_line(r"K1#: (\d+) (\d+)",t_line)
                    if(bid ==0 and b1_total !=0 ):
                        k1_time.append((end-start) / b1_total)
                        k1_run.append(b1_total)
                    if(bid ==1 and b2_total !=0 ):
                        k1_time.append((end-start) / b2_total)
                        k1_run.append(b2_total)


                    i = i + 3
                except Exception as e:
                    break

            fig, ax =plt.subplots()
            ax.scatter(k1_time,k1_run)
            plt.savefig("png/k1_run_%02d"%(run_id+1))

            times.append(k1_time)
            print("avg time = ",sum(k1_time) / len(k1_time))
            runs.append(k1_run)
            run_id += 1

    return run_id, times, runs 



#bid, b_k1, b_k2 = get_result("base.result")
iid, i_k1, i_k2 = get_result("overhead.txt")



"""
print("KERNEL 1")
for i in range(iid):
    avg_base = b_k1[i] 
    avg_new = i_k1[i]
    print("base avg cycle = ",avg_base, " new avg cycle = ",avg_new , " overhead = ", (avg_new - avg_base )/avg_base )


print("KERNEL 2")
for i in range(iid):
    avg_base = b_k2[i] 
    avg_new = i_k2[i]
    print("base avg cycle = ",avg_base, " new avg cycle = ",avg_new , " overhead = ", (avg_new - avg_base )/avg_base )



"""
