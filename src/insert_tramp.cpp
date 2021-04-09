#include <elf.h>
#include <vector>
#include <iostream>
#include "CodeObject.h"
#include "InstructionDecoder.h"
#include "CFG.h"
#include "InstrUtil.h"


using namespace std;
using namespace Dyninst;
using namespace ParseAPI;
using namespace InstructionAPI;
uint32_t insert_tramp(FILE * f, vector<MyInsn> & prologues, vector<MyInsn> & epilogues, uint32_t start_included, uint32_t end_excluded, uint32_t tramp_location, uint32_t scc_store_index, vector<char *> & insn_pool);


void test_acc(FILE * f, vector<bb> &bb_vec ,vector<char *> &insn_pool){
    // 1. Backup Address ( Prologue) 
    // 2. 
    //      a. Insert MemTime at start
    //      b. 
    //            i. Insert MemTime at end
    //            ii. Peforme Subtraction
    // 3. Write Back Resulti (Epilogue)


    /*
        Reserve s16:17 address
        Reserve s18:19 / s20:21 for storing mem_time value
        Reserve s22:23 for storing scc value
    */

#define ADDR_SGPR 16
#define MEMTIME_SGPR_1 18
#define MEMTIME_SGPR_2 20
#define SCC_SGPR 22
#define ADDR_VGPR 6
#define ACC_VGPR 8


    uint32_t avail_addr = 0;
    vector<MyInsn> pro_1, ep_1;

    ep_1.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    ep_1.push_back(InsnFactory::create_s_mov_b64(ADDR_SGPR,0,insn_pool));
    for ( auto & b : bb_vec){
        ep_1.push_back(InsnFactory::create_s_mov_b64(b.acc_sgpr,128,insn_pool));
    }

    avail_addr = insert_tramp(f,pro_1,ep_1,0x1000,0x1018,0x150c,SCC_SGPR,insn_pool);


    vector<MyInsn> pro_temp, epi_temp;
    for ( auto & b : bb_vec) {
       pro_temp.clear(); 
       epi_temp.clear();
       set_memtime_pro_ep(pro_temp,epi_temp,MEMTIME_SGPR_1,MEMTIME_SGPR_2,b.acc_sgpr,insn_pool);
       avail_addr = insert_tramp(f,pro_temp,epi_temp,b.start_included,b.end_excluded,avail_addr,SCC_SGPR,insn_pool);
    }
    
    vector<MyInsn> pro_3, ep_3;

    ep_3.push_back(InsnFactory::create_s_add_u32( ADDR_SGPR,ADDR_SGPR ,0x4010  ,true, insn_pool));
    ep_3.push_back(InsnFactory::create_s_addc_u32( ADDR_SGPR +1 , ADDR_SGPR+1  , 128 , false , insn_pool));

    for ( auto & b : bb_vec ){
        set_writeback_pro_ep(pro_3,ep_3,ADDR_SGPR,ADDR_VGPR,b.acc_sgpr,ACC_VGPR,insn_pool);
    }

    avail_addr = insert_tramp(f,pro_3,ep_3,0x1500,0x1508,avail_addr,SCC_SGPR,insn_pool);
}

void test_counter(FILE * f, vector<bb> &bb_vec ,vector<char *> &insn_pool){
    // 1. Backup Address ( Prologue) 
    // 2. 
    //      a. Insert MemTime at start
    //      b. 
    //            i. Insert MemTime at end
    //            ii. Peforme Subtraction
    // 3. Write Back Resulti (Epilogue)


    /*
        Reserve s16:17 address
        Reserve s18:19 / s20:21 for storing mem_time value
        Reserve s22:23 for storing scc value
    */

#define ADDR_SGPR 16
#define MEMTIME_SGPR_1 18
#define MEMTIME_SGPR_2 20
#define SCC_SGPR 22
#define ADDR_VGPR 6
#define ACC_VGPR 8


    uint32_t avail_addr = 0;
    vector<MyInsn> pro_1, ep_1;

    ep_1.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    ep_1.push_back(InsnFactory::create_s_mov_b64(ADDR_SGPR,0,insn_pool));
    for ( auto & b : bb_vec){
        ep_1.push_back(InsnFactory::create_s_mov_b64(b.acc_sgpr,128,insn_pool));
    }

    avail_addr = insert_tramp(f,pro_1,ep_1,0x1000,0x1018,0x150c,SCC_SGPR,insn_pool);


    vector<MyInsn> pro_temp, epi_temp;
    for ( auto & b : bb_vec) {
       pro_temp.clear(); 
       epi_temp.clear();
       set_counter_pro_ep(pro_temp,epi_temp,b.acc_sgpr,insn_pool);
       avail_addr = insert_tramp(f,pro_temp,epi_temp,b.start_included,b.end_excluded,avail_addr,SCC_SGPR,insn_pool);
    }
    
    vector<MyInsn> pro_3, ep_3;

    ep_3.push_back(InsnFactory::create_s_add_u32( ADDR_SGPR,ADDR_SGPR ,0x4010  ,true, insn_pool));
    ep_3.push_back(InsnFactory::create_s_addc_u32( ADDR_SGPR +1 , ADDR_SGPR+1  , 128 , false , insn_pool));

    for ( auto & b : bb_vec ){
        set_writeback_pro_ep(pro_3,ep_3,ADDR_SGPR,ADDR_VGPR,b.acc_sgpr,ACC_VGPR,insn_pool);
    }

    avail_addr = insert_tramp(f,pro_3,ep_3,0x1500,0x1508,avail_addr,SCC_SGPR,insn_pool);
}



int main(int argc, char **argv){


    vector < char *> insn_pool;
    if(argc != 5){
        printf("Usage: %s <binary path> <start_included> <end_excluded> <tramp_location> \n", argv[0]);
        return -1;
    }
    
    char *binaryPath = argv[1];
    FILE* fp = fopen(binaryPath,"rb+");
    vector<MyInsn> insns;
    vector<MyInsn> empty;
    

    vector<bb>  vec_bb= {{ 0x10f4, 0x13d0, 24 },  { 0x13dc, 0x1434, 26 }, { 0x1438, 0x1500, 28 }};

    test_acc(fp,vec_bb,insn_pool);
    //test_counter(fp,vec_bb,insn_pool);

    for (auto &p : insn_pool){
        free(p);    
    }
  /* 
    map<Address,bool> seen;
    vector<Function *> funcs;
    SymtabCodeSource * sts;
    CodeObject * co;
    sts = new SymtabCodeSource( argv[1]);
    co = new CodeObject( sts);
    co -> parse();

    const CodeObject::funclist & all = co->funcs();
    auto fit = all.begin();
    for (int i =0 ; fit != all.end(); fit++ , i++){
        Function * f = * fit;
        InstructionDecoder decode(f->isrc()->getPtrToInstruction(f->addr()),InstructionDecoder::maxInstructionLength,f->region()->getArch());

        auto bit = f->blocks().begin();
        for (; bit != f->blocks().end(); ++ bit){
            Block * b = * bit;

            for (auto & edges : b->targets()){
                
                printf(" block starting addr = %x , last instr addr = %x\n",
                    edges->src()->start(),
                    edges->src()->lastInsnAddr()
                );

            }
                    
        }

    }
    std::cout <<"number of basic blocks " << seen.size() << std::endl;
    */

    fclose(fp);
    return 0;

}





/*void test_accumulation(FILE * f, vector<char *> &insn_pool){
    // 1. Backup Address ( Prologue) 
    // 2. 
    //      a. Insert MemTime at start
    //      b. 
    //            i. Insert MemTime at end
    //            ii. Peforme Subtraction
    // 3. Write Back Resulti (Epilogue)


 
#define ADDR_SGPR 16
#define MEMTIME_SGPR_1 18
#define MEMTIME_SGPR_2 20
#define SCC_SGPR 22


    uint32_t avail_addr = 0;
    vector<MyInsn> pro_1, ep_1;

    ep_1.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    ep_1.push_back(InsnFactory::create_s_mov_b64(ADDR_SGPR,0,insn_pool));
    ep_1.push_back(InsnFactory::create_s_mov_b64(24,128,insn_pool));
    ep_1.push_back(InsnFactory::create_s_mov_b64(26,128,insn_pool));
    ep_1.push_back(InsnFactory::create_s_mov_b64(28,128,insn_pool));

    avail_addr = insert_tramp(f,pro_1,ep_1,4096,4120,5388,SCC_SGPR,insn_pool);

    vector<MyInsn> pro_a, ep_a;
    set_memtime_pro_ep(pro_a,ep_a,MEMTIME_SGPR_1,MEMTIME_SGPR_2,24,insn_pool);
    avail_addr = insert_tramp(f,pro_a,ep_a,0x10f4,0x13d0,avail_addr,SCC_SGPR,insn_pool);

    vector<MyInsn> pro_b, ep_b;
    set_memtime_pro_ep(pro_b,ep_b,MEMTIME_SGPR_1,MEMTIME_SGPR_2,26,insn_pool);
    avail_addr = insert_tramp(f,pro_b,ep_b,0x13dc,0x1434,avail_addr,SCC_SGPR,insn_pool);

    vector<MyInsn> pro_c, ep_c;
    set_memtime_pro_ep(pro_c,ep_c,MEMTIME_SGPR_1,MEMTIME_SGPR_2,28,insn_pool);
    avail_addr = insert_tramp(f,pro_c,ep_c,0x1438,0x1500,avail_addr,SCC_SGPR,insn_pool);
    
    vector<MyInsn> pro_3, ep_3;

    ep_3.push_back(InsnFactory::create_s_add_u32( ADDR_SGPR,ADDR_SGPR ,0x4010  ,true, insn_pool));
    ep_3.push_back(InsnFactory::create_s_addc_u32( ADDR_SGPR +1 , ADDR_SGPR+1  , 128 , false , insn_pool));
    
    set_writeback_pro_ep(pro_3,ep_3,ADDR_SGPR,6,24,8,insn_pool);
    set_writeback_pro_ep(pro_3,ep_3,ADDR_SGPR,6,26,8,insn_pool);
    set_writeback_pro_ep(pro_3,ep_3,ADDR_SGPR,6,28,8,insn_pool);

    avail_addr = insert_tramp(f,pro_3,ep_3,0x1500,0x1508,avail_addr,SCC_SGPR,insn_pool);
}
*/

