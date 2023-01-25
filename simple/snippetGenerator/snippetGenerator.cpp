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
   CodeGen gen;
   amdgpuCodeGen amdgen;
   /*amdgpuCodeGen::generate_s_add_u32(gen,2,1,16,false);
   amdgpuCodeGen::generate_s_add_u32(gen,4,3,18,false);
   amdgpuCodeGen::generate_s_add_u32(gen,6,5,20,false);
   amdgpuCodeGen::generate_s_addc_u32(gen,8,7,24,false);*/

   amdgpuCodeGen::generate_v_add_f32(gen,1,1,258,false);
   snippets.push_back(snippet(&gen,0x1048));
   int sid =0 ;
   char s_filename[1000];
   FILE * fp = fopen("snippets.txt","w");
   for ( auto snippet : snippets ) {
        sprintf(s_filename,"snippet_%d.txt",sid);
        void * ptr = snippet._gen->get_ptr(0);
        uint32_t size = snippet._gen->used();
        uint32_t addr = snippet._addr;
        fprintf(fp,"%d %d %s",addr,size,s_filename);


        FILE * s_fp = fopen(s_filename,"w");
        fwrite(snippet._gen->get_ptr(0),1,snippet._gen->used(),s_fp);
        fclose(s_fp);
    }
    fclose(fp); 
   /*printf("%lx\n",gen.get_ptr(0));
   uint32_t *cmd_ptr  = (uint32_t * ) gen.get_ptr(0);
   printf("%lx\n",*cmd_ptr);



   InstructionDecoder decoder(gen.get_ptr(0),gen.used(),Arch_amdgpu_gfx908);
   uint32_t end = gen.used();
   uint32_t size = 0;
   Instruction instr;
   while(size < end){
   instr = decoder.decode();

   std::cout <<instr.format() << std::endl;
   size += instr.size(); 
   }*/

   return 0;    
   }
