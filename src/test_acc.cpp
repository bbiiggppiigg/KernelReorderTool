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

typedef struct config {
    uint32_t orig_addr_sgpr ; // Index of sgpr that holds the base address at kernel lauch
    uint32_t addr_sgpr ; // Index of sgpr used to hold base address for later calculation
    uint32_t memtime_sgpr_1; // Index of sgpr that is used to get first timestamp and timestamp diff calculation
    uint32_t memtime_sgpr_2; // Index of sgpr that is used to get second timestamp ( larger )
    uint32_t scc_sgpr; // Index of sgpr used to save and restore scc value
    uint32_t addr_vgpr; // Index of VGPR used to store address to write back
    uint32_t acc_vgpr; // Index of VGPR used to store value to write back
    uint32_t prolog_start_included; // Address of first block, usually 0x1000
    uint32_t prolog_end_excluded; // End address of first block to preserve, 
    uint32_t first_scratch_space; // Address of the first available space for writing new instructions
    uint32_t epilog_start_included; // Adresss of last block before we write back restuls, usually the instruction before last instruction
    uint32_t epilog_end_excluded; // Usually the address of the last instruction
    uint32_t write_back_index; // byte offset to the address at original end - 8, where we start to write back results
}config;

config read_config(char * filename){
    FILE * fp = fopen(filename,"r");
    char tmp[100];
    config ret ;
    fgets(tmp,100,fp);
    ret.orig_addr_sgpr = strtoul(tmp,0,0);
    fgets(tmp,100,fp);
    ret.addr_sgpr = strtoul(tmp,0,0);
    fgets(tmp,100,fp);
    ret.memtime_sgpr_1 = strtoul(tmp,0,0);
    fgets(tmp,100,fp);
    ret.memtime_sgpr_2= strtoul(tmp,0,0);
    fgets(tmp,100,fp);
    ret.scc_sgpr = strtoul(tmp,0,0);
    fgets(tmp,100,fp);
    ret.addr_vgpr = strtoul(tmp,0,0);
    fgets(tmp,100,fp);
    ret.acc_vgpr = strtoul(tmp,0,0);
    fgets(tmp,100,fp);
    ret.prolog_start_included = strtoul(tmp,0,0);
    fgets(tmp,100,fp);
    ret.prolog_end_excluded = strtoul(tmp,0,0);
    fgets(tmp,100,fp);
    ret.first_scratch_space = strtoul(tmp,0,0);
    fgets(tmp,100,fp);
    ret.epilog_start_included = strtoul(tmp,0,0);
    fgets(tmp,100,fp);
    ret.epilog_end_excluded = strtoul(tmp,0,0);
    fclose(fp);
    return ret;
}

void test_acc(FILE * f, config c,vector<bb> &bb_vec ,vector<char *> &insn_pool){
    // 1. backup address ( prologue) 
    // 2. 
    //      a. insert memtime at start
    //      b. 
    //            i. insert memtime at end
    //            ii. peforme subtraction
    // 3. write back resulti (epilogue)



    uint32_t avail_addr = 0;
    vector<MyInsn> pro_1, ep_1;

    ep_1.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    ep_1.push_back(InsnFactory::create_s_mov_b64(c.addr_sgpr,c.orig_addr_sgpr,insn_pool));
    for ( auto & b : bb_vec){
        ep_1.push_back(InsnFactory::create_s_mov_b64(b.acc_sgpr,129,insn_pool));
    }

    avail_addr = insert_tramp(f,pro_1,ep_1,c.prolog_start_included,c.prolog_end_excluded,c.first_scratch_space,c.scc_sgpr,insn_pool);


    vector<MyInsn> pro_temp, epi_temp;
    for ( auto & b : bb_vec) {
       pro_temp.clear(); 
       epi_temp.clear();
       set_memtime_pro_ep(pro_temp,epi_temp,c.memtime_sgpr_1,c.memtime_sgpr_2,b.acc_sgpr,insn_pool);
       avail_addr = insert_tramp(f,pro_temp,epi_temp,b.start_included,b.end_excluded,avail_addr,c.scc_sgpr,insn_pool);
    }
    
    vector<MyInsn> pro_3, ep_3;

    //ep_3.push_back(InsnFactory::create_s_lshr_b32( 30, 130 ,3, insn_pool));
    //ep_3.push_back(InsnFactory::create_s_mov_b32( 30, 8, insn_pool));
    //ep_3.push_back(InsnFactory::create_s_lshl_b32( 30, 0, 30 , insn_pool));
    //ep_3.push_back(InsnFactory::create_s_and_b32( 30,0xff,3, 0xffff, insn_pool));
    ep_3.push_back(InsnFactory::create_s_add_u32( c.addr_sgpr,c.addr_sgpr ,128  , false, insn_pool));
    ep_3.push_back(InsnFactory::create_s_addc_u32( c.addr_sgpr +1 , c.addr_sgpr+1  , 128 , false , insn_pool));
    ep_3.push_back(InsnFactory::create_s_add_u32( c.addr_sgpr,c.addr_sgpr ,0x3ff8  ,true, insn_pool));
    ep_3.push_back(InsnFactory::create_s_addc_u32( c.addr_sgpr +1 , c.addr_sgpr+1  , 128 , false , insn_pool));

    for ( auto & b : bb_vec ){
        //set_writeback_pro_ep(pro_3,ep_3,c.addr_sgpr,c.addr_vgpr,b.acc_sgpr,c.acc_vgpr,insn_pool);
        set_writeback_pro_ep(pro_3,ep_3,c.addr_sgpr,c.addr_vgpr,b.acc_sgpr,c.acc_vgpr,insn_pool);
    }

    avail_addr = insert_tramp(f,pro_3,ep_3,c.epilog_start_included,c.epilog_end_excluded,avail_addr,c.scc_sgpr,insn_pool);
}

void test_acc_new(FILE * f, config c,vector<bb> &bb_vec ,vector<char *> &insn_pool){
    // 1. backup address ( prologue) 
    // 2. 
    //      a. insert memtime at start
    //      b. 
    //            i. insert memtime at end
    //            ii. peforme subtraction
    // 3. write back resulti (epilogue)



    uint32_t avail_addr = 0;
    vector<MyInsn> pro_1, ep_1;

    ep_1.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    ep_1.push_back(InsnFactory::create_s_mov_b64(c.addr_sgpr,c.orig_addr_sgpr,insn_pool));
    for ( auto & b : bb_vec){
        ep_1.push_back(InsnFactory::create_s_mov_b64(b.acc_sgpr,129,insn_pool));
    }
    ep_1.push_back(InsnFactory::create_s_load_dword(36,4,0xc,insn_pool));
    ep_1.push_back(InsnFactory::create_s_mov_b32(38,8,false,insn_pool));
    ep_1.push_back(InsnFactory::create_s_mov_b32(39,9,false,insn_pool));

    avail_addr = insert_tramp(f,pro_1,ep_1,c.prolog_start_included,c.prolog_end_excluded,c.first_scratch_space,c.scc_sgpr,insn_pool);


    vector<MyInsn> pro_temp, epi_temp;
    for ( auto & b : bb_vec) {
       pro_temp.clear(); 
       epi_temp.clear();
       set_memtime_pro_ep(pro_temp,epi_temp,c.memtime_sgpr_1,c.memtime_sgpr_2,b.acc_sgpr,insn_pool);
       avail_addr = insert_tramp(f,pro_temp,epi_temp,b.start_included,b.end_excluded,avail_addr,c.scc_sgpr,insn_pool);
    }
    
    vector<MyInsn> pro_3, ep_3;

    // Test Generating WG_ID
    ep_3.push_back(InsnFactory::create_s_ashr_i32( 36, 131 ,36, insn_pool));
    ep_3.push_back(InsnFactory::create_s_mul_hi_u32(37,36,39,insn_pool));
    ep_3.push_back(InsnFactory::create_s_mul_i32(36,36,39,insn_pool));
    ep_3.push_back(InsnFactory::create_s_add_u32( 36,36 ,38  , false, insn_pool));
    ep_3.push_back(InsnFactory::create_s_addc_u32( 37 , 37  , 128 , false , insn_pool));
    ep_3.push_back(InsnFactory::create_s_lshl_b64(36, 131 , 36 , insn_pool));
    //
    //
    //
    //
    //
    ep_3.push_back(InsnFactory::create_s_add_u32( c.addr_sgpr,c.addr_sgpr ,36  , false, insn_pool));
    ep_3.push_back(InsnFactory::create_s_addc_u32( c.addr_sgpr +1 , c.addr_sgpr+1  , 37 , false , insn_pool));
    ep_3.push_back(InsnFactory::create_s_add_u32( c.addr_sgpr,c.addr_sgpr ,0x3ff8  ,true, insn_pool));
    ep_3.push_back(InsnFactory::create_s_addc_u32( c.addr_sgpr +1 , c.addr_sgpr+1  , 128 , false , insn_pool));

    for ( auto & b : bb_vec ){
        //set_writeback_pro_ep(pro_3,ep_3,c.addr_sgpr,c.addr_vgpr,b.acc_sgpr,c.acc_vgpr,insn_pool);
        set_writeback_pro_ep(pro_3,ep_3,c.addr_sgpr,c.addr_vgpr,b.acc_sgpr,c.acc_vgpr,insn_pool);
    }

    avail_addr = insert_tramp(f,pro_3,ep_3,c.epilog_start_included,c.epilog_end_excluded,avail_addr,c.scc_sgpr,insn_pool);
}


int main(int argc, char **argv){
    vector < char *> insn_pool;
    if(argc != 4){
        printf("Usage: %s <binary path> <config_path> <bb_path>  \n", argv[0]);
        return -1;
    }
    
    char *binaryPath = argv[1];
    FILE* fp = fopen(binaryPath,"rb+");
    vector<MyInsn> insns;
    vector<MyInsn> empty;
   
    config c = read_config(argv[2]);

    vector<bb>  vec_bb;
    FILE * bb_file = fopen(argv[3],"r");
    uint32_t start_included, end_excluded, acc_sgpr;
    char tmp1[100],tmp2[100],tmp3[100];
    while(~fscanf(bb_file,"%s %s %s",tmp1,tmp2,tmp3)){
       start_included = strtoul(tmp1,0,0);
       end_excluded = strtoul(tmp2,0,0);
       acc_sgpr = strtoul(tmp3,0,0);

       vec_bb.push_back( bb{start_included,end_excluded,acc_sgpr});
       printf("%u %u %u\n",start_included,end_excluded,acc_sgpr); 
    }
    //test_acc(fp,insn_pool);
    test_acc_new(fp,c,vec_bb,insn_pool);

    for (auto &p : insn_pool){
        free(p);    
    }
    fclose(fp);
    return 0;

}



