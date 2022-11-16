#include <vector>
#include <iostream>
#include "InstrUtil.h"

using namespace std;
int main(int argc, char **argv){
    if(argc != 5){
        printf("Usage: %s <binary path> <start_included> <end_excluded> <new_location>  \n", argv[0]);
        return -1;
    }
    uint32_t start_included = strtoul(argv[2],0,16); 
    uint32_t end_excluded = strtoul(argv[3],0,16); 
    uint32_t new_location = strtoul(argv[4],0,16); 
    char *binaryPath = argv[1];
    FILE* fp = fopen(binaryPath,"rb+");

    //printf("start included = %x, end_excluded = %x, new_location = %x\n",start_included,end_excluded,new_location);
    move_block_to(fp,start_included,end_excluded,new_location);
    fclose(fp);
    return 0;

}



