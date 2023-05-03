#include <elf.h>
#include <vector>
#include <iostream>
#include "CodeObject.h"
#include "InstructionDecoder.h"
#include "CFG.h"
#include "InsnFactory.h"
#include "kernel_elf_helper.h"
#include "INIReader.h"
#include "../lib/AMDHSAKernelDescriptor.h"

#include "helper.h"
using namespace std;
using namespace Dyninst;
using namespace ParseAPI;
using namespace InstructionAPI;





#define M0 124
#define EXEC 126
#define EXEC_LOW 126
#define EXEC_HI 127
#define S_0 128
#define S_1 129
#define S_2 130
#define S_3 131
#define S_4 132
#define S_6 134
#define S_8 136
#define S_16 144
#define S_MINUS_1 193
#define SCC 253


#define VGPR0 0
#define VGPR1 1
#define VGPR2 2
#define VGPR3 3
#define VGPR4 4
#define VGPR5 5
#define VGPR6 6
#define VGPR7 7
#define VGPR8 8
#define VGPR9 9

#define SGPR0 0
#define SGPR6 6
#define SGPR8 8
#define SGPR9 9


/**
 * Ininitailization of Instrumentation
 * 1. Backup useful information ( WG_ID_Y / WG_ID_X )
 * 2. Compute Per Warp ID Base Adress
 * 3. Initialize Vector Registers that are later used
 * TODO: WE DON'T KNOW GRIM_DIM_X
 *
 *
 *
 *  // (WG_ID_Z * WG_GRID_DIM_Y + WG_ID_Y) * WG_DIM_X + WG_ID_X; // global work
 *
 */

typedef uint16_t reg;
typedef uint16_t sreg;
typedef uint16_t vreg;
void gen_empty(vector<MyInsn> & ret , config c , vector<char *> & insn_pool){

    /**
     * To calculate the local warp id, we need to calculate threadId_Y * Threads_PER_block_X + threadID_X / 64
     * Here we reuse warp_sgpr_pair to do so
     */
    // First step load from dispatch ptr size of block 
    //
    // GLOBAL WORK GROUP ID : (WG_ID_Z * #WG_PER_Y + WG_ID_Y) * #WG_PWE_X + WG_ID_X
    //ret.push_back(InsnFactory::create_s_add_u32(c.kernarg_segment_ptr   , S_16  , c.kernarg_segment_ptr, false ,insn_pool));
    //ret.push_back(InsnFactory::create_s_addc_u32(c.kernarg_segment_ptr+1   , S_0  , c.kernarg_segment_ptr+1, false ,insn_pool));

    uint32_t FIRST_FREE_SGPR = c.first_uninitalized_sgpr; 
    uint32_t S_KERNARG = c.kernarg_segment_ptr;

    uint32_t S_WBADDR = FIRST_FREE_SGPR;
    uint32_t S_TIMER = FIRST_FREE_SGPR+2;

    ret.push_back(InsnFactory::create_s_memtime(S_TIMER,insn_pool));
    //ret.push_back(InsnFactory::create_s_mov_b32(M0,S_MINUS_1,false,insn_pool)); // Initialize M0 to -1 to access shared memory
    printf("first uninitialized sgpr = %u, kernarg_segment_ptr = %u \n",FIRST_FREE_SGPR,c.kernarg_segment_ptr);
    assert(c.kernarg_segment_ptr != (unsigned) -1);

    c.old_kernarg_size = 0;
    ret.push_back(InsnFactory::create_s_load_dwordx2(S_WBADDR,S_KERNARG , c.old_kernarg_size ,insn_pool));
    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    ret.push_back(InsnFactory::create_s_store_dword_x2(S_TIMER ,S_WBADDR, 0, insn_pool)); 
    ret.push_back(InsnFactory::create_s_store_dword_x2(S_WBADDR, S_WBADDR, 8 , insn_pool)); 
    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    ret.push_back(InsnFactory::create_s_memtime(S_TIMER,insn_pool));
    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    ret.push_back(InsnFactory::create_s_store_dword_x2(S_TIMER ,S_WBADDR, 8, insn_pool)); 
    ret.push_back(InsnFactory::create_s_endpgm(insn_pool));
}


void setup_initailization(vector<MyInsn> & ret , config c , vector<char *> & insn_pool){

    /**
     * To calculate the local warp id, we need to calculate threadId_Y * Threads_PER_block_X + threadID_X / 64
     * Here we reuse warp_sgpr_pair to do so
     */
    // First step load from dispatch ptr size of block 
    //
    // GLOBAL WORK GROUP ID : (WG_ID_Z * #WG_PER_Y + WG_ID_Y) * #WG_PWE_X + WG_ID_X
    //ret.push_back(InsnFactory::create_s_add_u32(c.kernarg_segment_ptr   , S_16  , c.kernarg_segment_ptr, false ,insn_pool));
    //ret.push_back(InsnFactory::create_s_addc_u32(c.kernarg_segment_ptr+1   , S_0  , c.kernarg_segment_ptr+1, false ,insn_pool));

    
    uint32_t FIRST_FREE_SGPR = c.first_uninitalized_sgpr; 
    uint32_t S_KERNARG = c.kernarg_segment_ptr;
    uint32_t S_GRIDDIM_X = FIRST_FREE_SGPR;
    uint32_t S_GRIDDIM_Y = FIRST_FREE_SGPR+1;
    uint32_t S_BLOCKDIM_X = FIRST_FREE_SGPR+2;
    uint32_t S_BLOCKDIM_Y = FIRST_FREE_SGPR+3;
    uint32_t S_TMP = FIRST_FREE_SGPR+4;
    uint32_t S_TMP1 = FIRST_FREE_SGPR+5;
    uint32_t S_WBADDR = FIRST_FREE_SGPR+6;
    uint32_t S_TIMER = FIRST_FREE_SGPR+8;

    uint32_t S_WGID = S_GRIDDIM_X;
    uint32_t S_LWFID = S_GRIDDIM_Y;
    uint32_t S_WARP_PER_BLOCK = S_BLOCKDIM_Y;
    uint32_t S_GWFID = S_BLOCKDIM_X;
    uint32_t S_DEBUG = FIRST_FREE_SGPR + 12; 
    uint32_t S_DEBUG_2 = FIRST_FREE_SGPR + 13; 
    uint32_t S_PERWF_OFFSET;
    if (c.vgpr_spill ){
        S_PERWF_OFFSET = FIRST_FREE_SGPR + 10;
    }else{
        S_PERWF_OFFSET = c.PER_WAVEFRONT_OFFSET;
    }
    
    ret.push_back(InsnFactory::create_s_memtime(S_TIMER,insn_pool));
    //ret.push_back(InsnFactory::create_s_mov_b32(M0,S_MINUS_1,false,insn_pool)); // Initialize M0 to -1 to access shared memory
    printf("first uninitialized sgpr = %u, kernarg_segment_ptr = %u \n",FIRST_FREE_SGPR,c.kernarg_segment_ptr);
    assert(c.kernarg_segment_ptr != (unsigned) -1);

    c.old_kernarg_size = 0;
    if((FIRST_FREE_SGPR % 4) == 0){
        ret.push_back(InsnFactory::create_s_load_dwordx4(FIRST_FREE_SGPR,S_KERNARG, c.old_kernarg_size+12 ,insn_pool));
    }else{
        ret.push_back(InsnFactory::create_s_load_dwordx2(FIRST_FREE_SGPR,S_KERNARG, c.old_kernarg_size+12 ,insn_pool));
        ret.push_back(InsnFactory::create_s_load_dwordx2(FIRST_FREE_SGPR+2,S_KERNARG, c.old_kernarg_size+28 ,insn_pool));
    }

    ret.push_back(InsnFactory::create_s_load_dwordx2(S_WBADDR,S_KERNARG , c.old_kernarg_size ,insn_pool));

    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));


    // Calculating GLOBAL BLOCK ID 
    // GLOBAL BLOCK ID = BLOCK_ID_Z * GRID_DIM_Y * GRID_DIM_X + BLOCK_ID_Y * GRID_DIM_X + BLOCK_ID+X
    //                 = (BLOCK_ID_Z * GRID_DIM_Y + BLOCK_ID_Y) * GRID_DIM_X + BLOCK_ID_X
    if(c.work_group_id_z_enabled){
        

        ret.push_back(InsnFactory::create_s_mul_i32(S_TMP,S_GRIDDIM_Y,c.work_group_id_z,insn_pool)); 
    }else{
        ret.push_back(InsnFactory::create_s_mov_b32(S_TMP,128,false,insn_pool));  
        printf("work group id z not enabled\n");
    }

    if(c.work_group_id_y_enabled){
        ret.push_back(InsnFactory::create_s_add_u32(S_TMP,S_TMP,c.work_group_id_y,false,insn_pool));
    }else{
        printf("work group id y not enabled\n");
    }
    ret.push_back(InsnFactory::create_s_mul_i32(S_WGID,S_GRIDDIM_X,S_TMP,insn_pool));



    if(c.work_group_id_x_enabled){
        ret.push_back(InsnFactory::create_s_add_u32(S_WGID,S_WGID,c.work_group_id_x,false,insn_pool));
    }
   
    ret.push_back(InsnFactory::create_s_mov_b32(S_DEBUG,c.work_group_id_x,false,insn_pool));  
    // AFTER THIS STEP, a global BLOCK ID is stored in S_WGID
    //

    // THREAD_ID = (THREAD_ID_Z * WG_DIM_Y +  TID_Y ) * WG_DIM_X + TID_x
    // T_ID      = (T_ID_Z * BLOCK_DIM_Y + TID_Y ) * BLOCK_DIM_X + TID_X
    if(c.work_item_id_enabled > 1){ // TID_Z
        ret.push_back(InsnFactory::create_v_readfirstlane_b32(S_TMP,258,insn_pool));  // 258 is VGPR2 in this encoding
        ret.push_back(InsnFactory::create_s_mul_i32(S_TMP,S_BLOCKDIM_Y,S_TMP,insn_pool));
    }else{
        ret.push_back(InsnFactory::create_s_mov_b32(S_TMP,128,false,insn_pool));  
    }

    ret.push_back(InsnFactory::create_s_load_dword(S_WARP_PER_BLOCK,S_KERNARG, c.old_kernarg_size+8 ,insn_pool)); // WARPS_PER_BLOCK ?? WHY READ HERE?

    if(c.work_item_id_enabled > 0){ // TID_Y
        ret.push_back(InsnFactory::create_v_readlane_b32(S_TMP1,128,1+256,insn_pool));
        ret.push_back(InsnFactory::create_s_add_u32(S_TMP,S_TMP1,S_TMP,false,insn_pool));
    }


    ret.push_back(InsnFactory::create_s_mul_i32(S_TMP,S_BLOCKDIM_X,S_TMP,insn_pool));
    ret.push_back(InsnFactory::create_v_readlane_b32(S_TMP1,128,0+256,insn_pool)); // S_TMP1 = V1[0]
    ret.push_back(InsnFactory::create_s_add_u32(S_TMP,S_TMP1,S_TMP,false,insn_pool));



    // AFTER THIS STEP, we have a thread ID, now we want warp id , divide by 64
    ret.push_back(InsnFactory::create_s_lshr_b32(S_LWFID,S_6,S_TMP ,insn_pool)); 


    // NOw we want to calculate global wavefront id 
    // Simple : GLOBAL_WAVEFRONT_ID = WORK_GROUP_ID * # WAVE_FRONT_PER_WORK_GROUP + LOCAL_WAVEFRONT_ID
    //


    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool)); // waiting for the write back address to be loaded
    ret.push_back(InsnFactory::create_s_mul_i32(S_TMP,S_WGID,S_WARP_PER_BLOCK,insn_pool));

    ret.push_back(InsnFactory::create_s_mov_b32(S_DEBUG_2,S_WARP_PER_BLOCK,false,insn_pool));  
    ret.push_back(InsnFactory::create_s_add_u32(S_GWFID,S_TMP,S_LWFID,false,insn_pool));


    // At this point our interest is in calculating per wavefront offset
    // PER_WAVEFRONT_OFFSET should points to WRITEBACK_ADDR +  OFFSET + GLOBAL_WAVEFRONT_ID * NUM_BRANCHES * 8 
    // PER WAVEFRONT OFFSET = GLOBAL_WAVE_FRONT_ID * ( NUM_BRANHCES + 2 ) * 8 + WRITE_BACK_ADDR
    uint32_t local_offset =  (c.num_branches + 2 ) * 8;
    printf("c.num_branches = %u, local_offset = %u\n",c.num_branches, local_offset);
    //S_PERWF_OFFSET = S_TIMER;
    ret.push_back(InsnFactory::create_s_mov_b32(S_PERWF_OFFSET,local_offset ,true,insn_pool));
    ret.push_back(InsnFactory::create_s_mul_i32(S_PERWF_OFFSET,S_GWFID,S_PERWF_OFFSET,insn_pool)); 


    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool)); // waiting for the write back address to be loaded
    ret.push_back(InsnFactory::create_s_add_u32(  S_PERWF_OFFSET   , S_PERWF_OFFSET , S_WBADDR   , false ,insn_pool));
    ret.push_back(InsnFactory::create_s_addc_u32( S_PERWF_OFFSET+1 ,             S_0, S_WBADDR+1 , false ,insn_pool));  
    ret.push_back(InsnFactory::create_s_store_dword_x2(S_TIMER ,S_PERWF_OFFSET, c.num_branches * 8, insn_pool)); 

    ret.push_back(InsnFactory::create_s_store_dword_x2(S_PERWF_OFFSET ,S_PERWF_OFFSET, c.num_branches * 8 + 8 , insn_pool)); 

    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    //ret.push_back(InsnFactory::create_s_store_dword( S_DEBUG ,S_PERWF_OFFSET, c.num_branches * 8, insn_pool)); 
    //ret.push_back(InsnFactory::create_s_store_dword( S_DEBUG_2 ,S_PERWF_OFFSET, c.num_branches * 8+4, insn_pool)); 
    if( c.vgpr_spill ){
        ret.push_back(InsnFactory::create_v_writelane_b32(c.V_SPILL,128,S_PERWF_OFFSET,insn_pool)); // VSPILL
        ret.push_back(InsnFactory::create_v_writelane_b32(c.V_SPILL,129,S_PERWF_OFFSET+1,insn_pool)); // VSPILL
    }

}


// SAVED :64 bit SGPR PER_WAVEFRONT_OFFSET = S_ADDR
// NEED : 32 bit SGPR S_DATA, for storing SCC

void per_branch_instrumentation(vector<MyInsn> & ret , uint32_t branch_id , uint32_t execcond , config c, vector<char *> & insn_pool){
    return;
    //
    uint32_t EXECCOND = execcond; // 2
    uint32_t local_offset = (branch_id)  * 8;
    uint32_t S_PERWF_OFFSET;
    uint32_t SDATA;
    if ( c.vgpr_spill ){
        S_PERWF_OFFSET = 100;
        SDATA = 99;
    }else{ 
        SDATA = c.SDATA;
        S_PERWF_OFFSET = c.PER_WAVEFRONT_OFFSET;
    }

    uint32_t S_ADDR = S_PERWF_OFFSET;
    if( c.vgpr_spill ){

        // VSPILL[2:3] = BACKING_UP_SPERWF_OFFSET
        ret.push_back(InsnFactory::create_v_writelane_b32(c.V_SPILL,130,S_PERWF_OFFSET,insn_pool));
        ret.push_back(InsnFactory::create_v_writelane_b32(c.V_SPILL,131,S_PERWF_OFFSET+1,insn_pool));
        // VSPILL[4] = BACKING_UP_SDATA
        ret.push_back(InsnFactory::create_v_writelane_b32(c.V_SPILL,132,SDATA,insn_pool)); // VSPILL

        // S_PERWF_OFFSET = VGPR[0:1]
        ret.push_back(InsnFactory::create_v_readlane_b32(S_PERWF_OFFSET,128,c.V_SPILL+256,insn_pool));
        ret.push_back(InsnFactory::create_v_readlane_b32(S_PERWF_OFFSET+1,129,c.V_SPILL+256,insn_pool));
    }


    //S_ADDR_PAIR = PER_WAVEFRONT_BASE + ( BRANCH_ID * 8 ( each branch takes 8 bytes ) )
    ret.push_back(InsnFactory::create_s_cmp_eq_u64(EXEC,EXECCOND,insn_pool)); // CHECK IF backuped_exec == exec && cond
    ret.push_back(InsnFactory::create_s_mov_b32(SDATA, SCC, false ,insn_pool));
    ret.push_back(InsnFactory::create_s_atomic_add(SDATA, S_ADDR , local_offset , insn_pool));
    ret.push_back(InsnFactory::create_s_cmp_eq_u64(EXEC,S_0,insn_pool)); // CHECK IF backuped_exec = 0
    ret.push_back(InsnFactory::create_s_mov_b32(SDATA, SCC, false ,insn_pool));
    ret.push_back(InsnFactory::create_s_atomic_add(SDATA, S_ADDR , local_offset , insn_pool));
    ret.push_back(InsnFactory::create_s_mov_b32(SDATA, 193, false ,insn_pool));
    ret.push_back(InsnFactory::create_s_atomic_inc(SDATA, S_ADDR , local_offset+4 , insn_pool));
    
    //ret.push_back(InsnFactory::create_s_mov_b32(SDATA, branch_id,true,insn_pool));  
    //ret.push_back(InsnFactory::create_s_store_dword( SDATA ,S_PERWF_OFFSET, local_offset, insn_pool)); 
    //ret.push_back(InsnFactory::create_s_store_dword( SDATA ,S_PERWF_OFFSET, local_offset+4, insn_pool)); 
    
    //uint32_t new_offset = c.num_branches * 8 + 8;
    //if(branch_id %2)
    //    ret.push_back(InsnFactory::create_s_atomic_inc(SDATA, S_ADDR , new_offset , insn_pool));

    if( c.vgpr_spill ){
        ret.push_back(InsnFactory::create_v_readlane_b32(S_PERWF_OFFSET,130,c.V_SPILL+256,insn_pool));
        ret.push_back(InsnFactory::create_v_readlane_b32(S_PERWF_OFFSET+1,131,c.V_SPILL+256,insn_pool));
        ret.push_back(InsnFactory::create_v_readlane_b32(SDATA,132,c.V_SPILL+256,insn_pool));
    }


    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
}


/*
 * To setup the write back, what we need to do is 
 * 1. Let LDS_ADDR points to LDS_BASE 
 * 2. Let WRITE_BACK_PTR points to WRITEBACK_ADDR + OFFSET
 */
// SAVED :  64 bit SGPR TIMESTAMP_1, 64 bit SGPR S_ADDR , 64 bit sgpr PER_WAVEFRONT_OFFSET

/*
void setup_writeback(vector<MyInsn> & ret , config c, vector<char *> & insn_pool){
    // DATA Collection is per warp, so we disabled rest of the threads
    ret.push_back(InsnFactory::create_s_mov_b64(EXEC,S_1,insn_pool)); // EXEC = 1 

    uint32_t PER_WAVEFRONT_OFFSET =c.TMP_SGPR0;

    ret.push_back(InsnFactory::create_v_mov_b32(c.v_global_addr , PER_WAVEFRONT_OFFSET, insn_pool)); // v[0] = s[0]
    ret.push_back(InsnFactory::create_v_mov_b32(c.v_global_addr+1 , PER_WAVEFRONT_OFFSET+1, insn_pool)); // v[1 ]= s[1] 



}*/

// Here we should be free to use what ever register we want
//
// SAVED :  64 bit SGPR TIMESTAMP_1, 64 bit SGPR S_ADDR , 64 bit sgpr PER_WAVEFRONT_OFFSET
void memtime_epilogue(vector<MyInsn> & ret,  config c , uint32_t my_offset ,vector<char *> & insn_pool){

    uint32_t S_PERWF_OFFSET;
    if ( c.vgpr_spill ){
        S_PERWF_OFFSET = 2;
    }else{ 
        S_PERWF_OFFSET = c.PER_WAVEFRONT_OFFSET;
    }

    uint32_t S_TIMER = S_PERWF_OFFSET - 2;
    if( c.vgpr_spill ){
        ret.push_back(InsnFactory::create_v_readlane_b32(S_PERWF_OFFSET,128,256+c.V_SPILL,insn_pool));
        ret.push_back(InsnFactory::create_v_readlane_b32(S_PERWF_OFFSET+1,129,256+c.V_SPILL,insn_pool));
 
    }



    ret.push_back(InsnFactory::create_s_memtime(S_TIMER,insn_pool));



    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    ret.push_back(InsnFactory::create_s_store_dword_x2(S_TIMER ,S_PERWF_OFFSET,my_offset +  8, insn_pool)); 

    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    return;
    uint32_t TMP_1 = S_TIMER - 2;
    uint32_t TMP_2 = S_TIMER - 1;

    ret.push_back(InsnFactory::create_s_mov_b32(TMP_1,0xA,true,insn_pool));  
    ret.push_back(InsnFactory::create_s_mov_b32(TMP_2,0xB,true,insn_pool));  
    ret.push_back(InsnFactory::create_s_store_dword( TMP_1 , S_PERWF_OFFSET, my_offset+8, insn_pool)); 
    ret.push_back(InsnFactory::create_s_store_dword( TMP_2 , S_PERWF_OFFSET, my_offset+12, insn_pool)); 
}




/*
 * What do we need ?
 * An Offset to Text section of the kernel
 * A list of addresses that describes the bound of kernels
 * A list of branches within each kernel
 * A list of symbols within each kernel
 */


int main(int argc, char **argv){

    vector < char *> insn_pool;

    vector<myKernelDescriptor> mkds;

    if(argc !=3){
        printf("Usage: %s <binary path> <config_file>\n",argv[0]);
    }

    char *binaryPath = argv[1];
    char *configPath = argv[2];


    analyze_binary(binaryPath, mkds);
    //vector<pair<uint64_t, string>> kds;
    //get_kds(fp,kds);
    //
    FILE* fp = fopen(binaryPath,"rb+");

    uint32_t text_offset = get_text_offset(fp);
    vector<config> configs;
    read_config(fp,configPath,configs,mkds);
    printf("after reading config\n");
    myKernelDescriptor cur_kd;
    uint32_t mkd_id = -1;
    int kid = 0;
    for(auto &c : configs){
        printf("KERNEL ID :  %d\n",++kid);
        uint32_t target_shift = 0;
        bool found = false;
        uint32_t ii = 0;
        for(auto & mkd : mkds ){
            if(mkd.name == c.name){
                mkd_id = ii;
                found = true; cur_kd = mkd; break;

            }
            ii++;
        }
        if(!found){
            std::cerr << " Error : try instrumenting non-existent kernel" << std::endl;
            exit(-1);
        }
        //vector<CFG_EDGE> &edges = cur_kd.branch_insns;
        vector<pair<uint32_t, uint32_t >> &save_mask_insns = mkds[mkd_id].save_mask_insns;
        vector<uint32_t> & endpgms = mkds[mkd_id].endpgms;
        vector<MyBranchInsn> branches;

        uint32_t num_branches = save_mask_insns.size();
        c.num_branches = num_branches;
        printf("in main, c.num_branches = %u\n",c.num_branches);
        
        {
        
            vector<MyInsn> empties;
            gen_empty(empties,c,insn_pool);
            FILE * f_co_empty = fopen("memtime.text","wb+");
            for( auto & insn : empties){
                fwrite(insn.ptr,1,insn.size,f_co_empty);
            }
            fclose(f_co_empty);
        }

        {
            vector<MyInsn> init_insns;
            setup_initailization(init_insns,c,insn_pool);

            inplace_insert( mkds, mkd_id, init_insns, mkds[mkd_id].start_addr);
            //inplace_insert( cur_kd, init_insns, cur_kd.start_addr, mkds);
            target_shift += getSize(init_insns);
        }

        dumpBuffers(mkds);
#ifndef MEASURE_BASE 
        uint32_t branch_id =0 ;
        for( const auto & pair_addr_sgpr : save_mask_insns){
            auto addr = pair_addr_sgpr.first;
            auto exec_cond_sgpr = pair_addr_sgpr.second;
            //printf("0x%x: branch id = %d, exec_cond_sgpr = %d\n",addr,branch_id,exec_cond_sgpr);
            vector<MyInsn> update_branch_statistic;
            per_branch_instrumentation(update_branch_statistic, branch_id  , exec_cond_sgpr , c ,insn_pool);
            
            inplace_insert( mkds,mkd_id, update_branch_statistic, addr);
            target_shift+= getSize(update_branch_statistic);
            branch_id++;
        }
#endif
        vector<MyInsn> writeback_insns;
        uint32_t my_offset = num_branches * 8;
        //setup_writeback(writeback_insns, c ,insn_pool);
        memtime_epilogue(writeback_insns, c, my_offset , insn_pool);
        vector<uint32_t> new_endpgms;
        new_endpgms.push_back(0x10ac);
        for (const auto & endpgm : endpgms){
            printf("now endpgm at address = %x\n",endpgm);
        //for (const auto & endpgm : new_endpgms){
            inplace_insert( mkds,mkd_id, writeback_insns, endpgm);
            target_shift+= getSize(writeback_insns);
        }
        set_sgpr_vgpr_usage(fp,c.kd_addr,c.sgpr_max,c.vgpr_max);
        propogate_mkd_update(mkds,mkd_id); 
        dumpBuffers(mkds);

        for(unsigned long ii =0 ; ii < mkds.size(); ii++){
            printf("updateing kernel with name %s with new address = %x, new size = %u\n",mkds[ii].name.c_str(),mkds[ii].start_addr,mkds[ii].end_addr - mkds[ii].start_addr);
            update_function_symbol(fp,mkds[ii].name.c_str(),mkds[ii].start_addr, mkds[ii].end_addr - mkds[ii].start_addr);
        }
        //break;
    }
    fclose(fp);
    finalize(binaryPath,mkds,text_offset);

    for (auto &p : insn_pool){
        free(p);    
    }



    return 0;

}





