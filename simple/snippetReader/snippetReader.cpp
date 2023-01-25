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
        CodeGen * _gen;
        uint32_t _addr;
        snippet(CodeGen * gen, uint32_t addr) : _gen(gen) , _addr(addr) {}
}snippet;

vector<snippet> snippets;

int main(){
    FILE * fp = fopen("snippets.txt","r");

    uint32_t size, addr;
    char filename[1000];
    void * ptr;
    while(~fscanf(fp,"%d %d %s",&addr,&size,filename)){
        FILE * s_fp = fopen(filename,"r");
        ptr = malloc(size);
        fread(ptr,1,size,s_fp);
        fclose(s_fp);
        InstructionDecoder decoder(ptr,size,Arch_amdgpu_gfx908);
        uint32_t end = size;
        uint32_t cur = 0;
        Instruction instr;
        printf("Snippet at location %p, size = %d, filename = %s\n",addr,size,filename);
        while(cur < end){
            instr = decoder.decode();

            std::cout <<instr.format() << std::endl;
            cur += instr.size(); 

        }

        free(ptr);
    }
        return 0;    
}
