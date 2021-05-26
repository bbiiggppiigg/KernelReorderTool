#!/usr/bin/env python
import sys

filename = sys.argv[1]
with open(filename,'r') as f:
    lines = f.readlines()

lines = lines[1:]
indices = []
timestamps = []
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
for i in range(0,n,2):
    print(f"i = {i}")
    index = (i/2)
    timestamp = int(timestamps[i+1],16)-int(timestamps[i],16)
    if(timestamps[i+1]=='3f8000003f800000'):
        print('skip this')
    else:
        print('index = %d, timestamp = 0x%x'%(index,timestamp))




