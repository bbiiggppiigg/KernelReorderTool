
#include <elf.h>
#include <vector>
#include <iostream>

// dyninst includes
#include "CodeObject.h"
#include "InstructionDecoder.h"
#include "CFG.h"

// my own includes 
#include "InsnFactory.h"
#include "kernel_elf_helper.h"
#include "inih/INIReader.h"
#include "AMDHSAKernelDescriptor.h"


#include "config.h"
#include "codegen-amdgpu.h"
using namespace std;
using namespace Dyninst;
using namespace ParseAPI;
using namespace InstructionAPI;



typedef struct snippet{
    public:
    void * _ptr;
    uint32_t _addr;
    uint32_t _size;
    snippet(void * ptr, uint32_t addr, uint32_t size): _ptr (ptr), _addr(addr) , _size(size){}
}snippet;

vector<snippet> snippets;
int main(int argc, char * argv[]){
    vector< char * > insn_pool;
    if(argc !=2){
        printf("Usage: %s <binary path> \n",argv[0]);
    }
    char *p;
    uint32_t offset = 0;
    if(argc == 3){
        offset = strtol(argv[2],&p,10)-0x1000;
        if(p){
            puts("GGGGGGGGGGGGGGG");
        }
        printf("offset = %x\n",offset);
        assert(offset >=0);
    }
    FILE * fp = fopen("snippets.txt","r");
    uint32_t size, addr;
    char filename[1000];
    void * ptr;
    while(~fscanf(fp,"%d %d %s",&addr,&size,filename)){
        FILE * s_fp = fopen(filename,"r");
        ptr = malloc(size);
        fread(ptr,1,size,s_fp);
        fclose(s_fp);
        snippets.push_back(snippet(ptr,addr,size));
    }
    fclose(fp);

    char *binaryPath = argv[1];

    FILE* f_binary = fopen(binaryPath,"rb+");


    vector<kernel_bound> kernel_bounds ; 
    uint32_t text_end; // address after last instr
    getKernelBounds(binaryPath,kernel_bounds,text_end);
    uint32_t sid = 0;
    uint32_t target_shift  = 0;
    for (auto kb : kernel_bounds){

        uint32_t per_kernel_size_sum  = 0;
        uint32_t func_start = kb.first;
        uint32_t func_end = kb.last;
        vector<CFG_EDGE> edges;
        vector<pair<uint32_t, uint32_t >> save_mask_insns;
        vector<uint32_t> endpgms;
        analyze_binary(binaryPath, edges,  save_mask_insns, func_start , func_end, endpgms); 
        vector<MyBranchInsn> branches;
        printf("before converting branches\n");
        for (auto & edge : edges ) {
            branches.push_back(
                    InsnFactory::convert_to_branch_insn(edge.branch_address, edge.target_address, edge.cmd, edge.length, insn_pool)
                    );
        }

        for(; sid < snippets.size(); sid++){
            uint32_t addr = snippets[sid]._addr + target_shift;
            uint32_t size = snippets[sid]._size;
            void * ptr = snippets[sid]._ptr;
            if (addr < func_start || addr > func_end)
                break;
           offsetted_inplace_insert(offset,f_binary,func_start,func_end, addr , size , ptr ,branches, kernel_bounds,endpgms,insn_pool);
           target_shift += size;
           per_kernel_size_sum += size;
        }

        /*printf("target_shift =%u\n",target_shift);
        uint32_t nop_size = 0;
        if(target_shift%256!=0){
            uint32_t remain = 256 - (target_shift%256);
            uint32_t num_nops = remain / 4;
            vector<MyInsn> nops;
            printf("number of remain = %u, num_nops = %u\n", remain,num_nops);
            for(uint32_t i=0; i <num_nops ;i++){
                nops.push_back(InsnFactory::create_s_nop(insn_pool));
            }
            offsetted_inplace_insert(offset,f_binary,func_start,func_end,nops,branches, func_end +target_shift,kernel_bounds,endpgms,insn_pool);
            nop_size += get_size(nops);
            per_kernel_size_sum += get_size(nops);
        }*/
        update_symtab_symbol_size(f_binary,kb.name.c_str(), func_end - func_start + per_kernel_size_sum );
    }
    extend_text(f_binary,target_shift);
    fclose(f_binary);


    return 0;    
}
