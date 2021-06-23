#!/usr/bin/env python
import sys
import statistics

filename = sys.argv[1]
with open(filename,'r') as f:
    lines = f.readlines()

lines = lines[1:]
indices = []
timestamps = []
for line in lines:
    try:
        time = line.split("(hipEventElapsedTime) = ")[1]
        time = time.split("ms")[0]
    except Exception as e:
        continue

print ("Runtime = ",time)

for line in lines:
    try:
        tmp1,tmp2 = line.split(',')
        tmp1_1,tmp1_2 = tmp1.split(':')
        tmp2_1,tmp2_2 = tmp2.split('0x')
        index = tmp1_2.strip()
        timestamp = tmp2_2.strip()
        indices.append(index)
        timestamps.append(timestamp)
    except Exception as e:
        continue

min_index = indices[0]
n = len(indices)
print(n)
tss = []
starts = [] 
ends = []

def report_stats(name, stats):
    print(f'reporting statistics for {name}')
    print(f'min = {min(stats)}, max ={max(stats)}, gap = {max(stats)-min(stats)}')
    print ('average = ',statistics.mean(stats))
    print ('stdev = ',statistics.pstdev(stats))
    print('======================================')



for i in range(0,n,2):
    index = (i/2)
    #timestamp = int(timestamps[i+1],16)-int(timestamps[i],16)
    start = int(timestamps[i],16)
    end = int(timestamps[i+1],16)
    timestamp = end - start
    starts.append(start)
    ends.append(end)
    tss.append(timestamp)


report_stats('time',tss)
report_stats('start',starts)
report_stats('end',ends)




