maint set lane-divergence-support off
set breakpoint pending on
b _Z6KernelP4NodePiPbS2_S2_S1_i
#r ./graph64.txt
r ./graph1MW_6.txt
set $a=0x00007fffecc84000
#b * ($a+0x1E8)
#b * ($a+0x1EC)

#b * ($a+0x210)

#b * ($a+0x2C8)
#b * ($a+0x2CC)
#b * ($a+0x2D4)

d 1

#c
#c 10
#c 
#c 10
#c 
#c 9
#c 
#c 5

b * ($a + 0x184)
#b * ($a + 0x2CC)
#b * ($a + 0x2D0)
c

#b * ($a + 0x4C) thread 60
#b * ($a + 0x30C) thread 60
#c 

#b * ($a+0x2D8)
#b * ($a+0x2DC)
#b * ($a+0x2E0)
#b * ($a+0x2E4)
#b * ($a+0x2E8)
#b * ($a+0x2F0)
#b * ($a+0x2FC)
#b * ($a+0x304)
#b * ($a+0x324)
#d 1
#c
