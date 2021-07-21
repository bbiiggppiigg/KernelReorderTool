#include "kernel_elf_helper.h"


int main(int argc, char **argv){
    if(argc != 4){
        printf("Usage: %s <binary path> <kernel_name> <new_sgpr_count> \n", argv[0]);
        return -1;
    }
    char *binaryPath = argv[1];
    FILE * fp = fopen(binaryPath,"rb+");
    vector< pair<uint64_t,string>> kds;
    get_kds(fp,kds);
    auto tmp = COMPUTE_PGM_RSRC1(fp,kds[0].first);
    set_sgpr_usage( fp , kds , string(argv[2])+string(".kd") , strtoul(argv[3],0,0));
    fclose(fp);
}



