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

#include "config.h"
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
/*
void insert_instr(vector<MyInsn> & ret, vector<MyInsn> & new_insn){
    for (auto & insn : new_insn){
        ret.push_back(insn);
    } 
    return;
}

void create_v_mov_b64(vector<MyInsn> & ret, uint32_t dst ,uint32-u
*/
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

    uint32_t TMP_SGPR0 = c.TMP_SGPR0;
    uint32_t TMP_SGPR1 = c.TMP_SGPR1;
    uint32_t GLOBAL_WAVEFRONT_ID = c.GLOBAL_WAVEFRONT_ID;
    uint32_t LOCAL_WAVEFRONT_ID = c.LOCAL_WAVEFRONT_ID;
    uint32_t WORK_GROUP_ID = c.WORK_GROUP_ID;
    uint32_t BACKUP_WRITEBACK_ADDR = c.BACKUP_WRITEBACK_ADDR;
    
    
    ret.push_back(InsnFactory::create_s_memtime(c.TIMER_1,insn_pool));
    //ret.push_back(InsnFactory::create_s_mov_b32(M0,S_MINUS_1,false,insn_pool)); // Initialize M0 to -1 to access shared memory
    printf("first uninitialized sgpr = %u\n",c.first_uninitalized_sgpr);
    if((c.first_uninitalized_sgpr % 4) == 0){
        ret.push_back(InsnFactory::create_s_load_dwordx4(c.first_uninitalized_sgpr,c.kernarg_segment_ptr, c.old_kernarg_size+12 ,insn_pool));
    }else{
        ret.push_back(InsnFactory::create_s_load_dwordx2(c.first_uninitalized_sgpr,c.kernarg_segment_ptr, c.old_kernarg_size+12 ,insn_pool));
        ret.push_back(InsnFactory::create_s_load_dwordx2(c.first_uninitalized_sgpr+2,c.kernarg_segment_ptr, c.old_kernarg_size+28 ,insn_pool));
    }

    ret.push_back(InsnFactory::create_s_load_dwordx2(BACKUP_WRITEBACK_ADDR,c.kernarg_segment_ptr, c.old_kernarg_size ,insn_pool));
    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));

    if(c.work_group_id_z_enabled){
        ret.push_back(InsnFactory::create_s_mov_b32(TMP_SGPR0,c.work_group_id_z,false,insn_pool));  
        ret.push_back(InsnFactory::create_s_mul_i32(TMP_SGPR0,c.first_uninitalized_sgpr+1,TMP_SGPR0,insn_pool));
    }else{
        ret.push_back(InsnFactory::create_s_mov_b32(TMP_SGPR0,0,true,insn_pool));  
    }

    if(c.work_group_id_y_enabled){
        ret.push_back(InsnFactory::create_s_add_u32(TMP_SGPR0,TMP_SGPR0,c.work_group_id_y,false,insn_pool));
    }
    ret.push_back(InsnFactory::create_s_mul_i32(WORK_GROUP_ID,c.first_uninitalized_sgpr,TMP_SGPR0,insn_pool));

    ret.push_back(InsnFactory::create_s_load_dword(c.first_uninitalized_sgpr,c.kernarg_segment_ptr, c.old_kernarg_size+8 ,insn_pool));

    if(c.work_group_id_x_enabled){
        ret.push_back(InsnFactory::create_s_add_u32(WORK_GROUP_ID,WORK_GROUP_ID,c.work_group_id_x,false,insn_pool));
    }
    // AFTER THIS STEP, a global WG_ID is stored in WORK_GROUP_ID 


    // THREAD_ID = (THREAD_ID_Z * WG_DIM_Y +  TID_Y ) * WG_DIM_X + TID_x
    if(c.work_item_id_enabled > 1){ // TID_Z
        ret.push_back(InsnFactory::create_v_readfirstlane_b32(TMP_SGPR0,258,insn_pool));  // 258 is VGPR2 in this encoding
        ret.push_back(InsnFactory::create_s_mul_i32(TMP_SGPR0,c.first_uninitalized_sgpr+3,TMP_SGPR0,insn_pool));
    }else{
        ret.push_back(InsnFactory::create_s_mov_b32(TMP_SGPR0,0,true,insn_pool));  
    }
    if(c.work_item_id_enabled > 0){ // TID_Y
        ret.push_back(InsnFactory::create_v_readlane_b32(TMP_SGPR1,128,257,insn_pool));
        ret.push_back(InsnFactory::create_s_add_u32(TMP_SGPR0,TMP_SGPR1,TMP_SGPR0,false,insn_pool));
    }
    ret.push_back(InsnFactory::create_s_mov_b32(TMP_SGPR1,c.first_uninitalized_sgpr+2,true,insn_pool));  
    ret.push_back(InsnFactory::create_s_mul_i32(TMP_SGPR0,TMP_SGPR1,TMP_SGPR0,insn_pool));



    ret.push_back(InsnFactory::create_v_readlane_b32(TMP_SGPR1,128,256,insn_pool));
    ret.push_back(InsnFactory::create_s_add_u32(TMP_SGPR0,TMP_SGPR1,TMP_SGPR0,false,insn_pool));
    // AFTER THIS STEP, we have a thread ID, now we want warp id , divide by 64
    ret.push_back(InsnFactory::create_s_lshr_b32(LOCAL_WAVEFRONT_ID,S_6,TMP_SGPR0 ,insn_pool)); 


    // NOw we want to calculate global wavefront id 
    // Simple : GLOBAL_WAVEFRONT_ID = WORK_GROUP_ID * # WAVE_FRONT_PER_WORK_GROUP + LOCAL_WAVEFRONT_ID
    //


    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    ret.push_back(InsnFactory::create_s_mul_i32(TMP_SGPR0,WORK_GROUP_ID,c.first_uninitalized_sgpr,insn_pool));
    ret.push_back(InsnFactory::create_s_add_u32(GLOBAL_WAVEFRONT_ID,TMP_SGPR0,LOCAL_WAVEFRONT_ID,false,insn_pool));




    uint32_t PER_WAVEFRONT_OFFSET = c.TMP_SGPR0; 
    // we use TMP_SGPR0 TMP_SGPR1 as they are continuous pair of SGPR
    // PER_WAVEFRONT_OFFSET should points to WRITEBACK_ADDR +  OFFSET + GLOBAL_WAVEFRONT_ID * NUM_BRANCHES * 8 
    uint32_t local_offset =  (c.num_branches + 2 ) * 8;

    ret.push_back(InsnFactory::create_s_mov_b32(PER_WAVEFRONT_OFFSET,local_offset ,true,insn_pool));
    ret.push_back(InsnFactory::create_s_mul_i32(PER_WAVEFRONT_OFFSET,c.GLOBAL_WAVEFRONT_ID,PER_WAVEFRONT_OFFSET,insn_pool)); 


    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    ret.push_back(InsnFactory::create_s_add_u32(PER_WAVEFRONT_OFFSET   , PER_WAVEFRONT_OFFSET , c.BACKUP_WRITEBACK_ADDR , false ,insn_pool));
    ret.push_back(InsnFactory::create_s_addc_u32(PER_WAVEFRONT_OFFSET+1,                   S_0, c.BACKUP_WRITEBACK_ADDR+1 , false ,insn_pool));  


}



void per_branch_instrumentation(vector<MyInsn> & ret , uint32_t branch_id , uint32_t execcond , config c, vector<char *> & insn_pool){


    uint32_t SDATA  = c.BACKUP_EXEC ; // 1 
    uint32_t EXECCOND = execcond; // 2
    uint32_t S_ADDR = c.TMP_SGPR0; // 2
    uint32_t local_offset = (branch_id)  * 8;

    // S_ADDR_PAIR = PER_WAVEFRONT_BASE + ( BRANCH_ID * 8 ( each branch takes 8 bytes ) )
    
    ret.push_back(InsnFactory::create_s_cmp_eq_u64(EXEC,EXECCOND,insn_pool)); // CHECK IF backuped_exec == exec && cond
    ret.push_back(InsnFactory::create_s_mov_b32(SDATA, SCC, false ,insn_pool));
    ret.push_back(InsnFactory::create_s_atomic_add(SDATA, S_ADDR , local_offset , insn_pool));
    ret.push_back(InsnFactory::create_s_cmp_eq_u64(EXEC,S_0,insn_pool)); // CHECK IF backuped_exec = 0
    ret.push_back(InsnFactory::create_s_mov_b32(SDATA, SCC, false ,insn_pool));
    ret.push_back(InsnFactory::create_s_atomic_add(SDATA, S_ADDR , local_offset , insn_pool));
    ret.push_back(InsnFactory::create_s_mov_b32(SDATA, 193, false ,insn_pool));
    ret.push_back(InsnFactory::create_s_atomic_inc(SDATA, S_ADDR , local_offset+4 , insn_pool));
    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
}

/*
 * To setup the write back, what we need to do is 
 * 1. Let LDS_ADDR points to LDS_BASE 
 * 2. Let WRITE_BACK_PTR points to WRITEBACK_ADDR + OFFSET
 */
void setup_writeback(vector<MyInsn> & ret , config c, vector<char *> & insn_pool){
    // DATA Collection is per warp, so we disabled rest of the threads
    ret.push_back(InsnFactory::create_s_mov_b64(EXEC,S_1,insn_pool)); // EXEC = 1 

    uint32_t PER_WAVEFRONT_OFFSET =c.TMP_SGPR0;

    ret.push_back(InsnFactory::create_v_mov_b32(c.v_global_addr , PER_WAVEFRONT_OFFSET, insn_pool)); // v[0] = s[0]
    ret.push_back(InsnFactory::create_v_mov_b32(c.v_global_addr+1 , PER_WAVEFRONT_OFFSET+1, insn_pool)); // v[1 ]= s[1] 



}


void memtime_epilogue(vector<MyInsn> & ret,  config c , uint32_t my_offset ,vector<char *> & insn_pool){





    ret.push_back(InsnFactory::create_s_memtime(c.TIMER_2,insn_pool));

    ret.push_back(InsnFactory::create_s_store_dword_x2(c.TIMER_1 ,c.TMP_SGPR0, my_offset, insn_pool)); 

    // HERE WE REUSE the Vector Register for DS_DATA to write back results
    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));

    ret.push_back(InsnFactory::create_s_store_dword_x2(c.TIMER_2 ,c.TMP_SGPR0, my_offset+8, insn_pool)); 
}





int main(int argc, char **argv){

    vector < char *> insn_pool;



    if(argc !=3){
        printf("Usage: %s <binary path> <config_file>\n",argv[0]);
    }

    char *binaryPath = argv[1];
    char *configPath = argv[2];

    //vector<pair<uint64_t, string>> kds;
    //get_kds(fp,kds);
    //
    FILE* fp = fopen(binaryPath,"rb+");


    vector<kernel_bound> kernel_bounds ; 
    uint32_t text_end; // address after last instr
    getKernelBounds(binaryPath,kernel_bounds,text_end);

    vector<config> configs;
    read_config(fp,configPath,configs,kernel_bounds);
    fclose(fp);
    printf("after reading config\n");
    for(auto &c : configs){

        fp = fopen(binaryPath,"rb+");

        uint32_t target_shift = 0;
        uint32_t func_start = 0;
        uint32_t func_end = 0;
        for( auto & kb : kernel_bounds){
            if(kb.name == c.name){
                func_start = kb.first;
                func_end = kb.last;
            }
        }
        if(func_start ==0){
            assert(0 && " kernel name not found ");
        }


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

        uint32_t num_branches = save_mask_insns.size();
        c.num_branches = num_branches;

        printf("before initialization \n");
        {
            vector<MyInsn> init_insns;
            setup_initailization(init_insns,c,insn_pool);

            inplace_insert(fp,func_start,text_end,init_insns,branches, func_start+ target_shift,kernel_bounds,endpgms,insn_pool);
            printf("patching start at address offset %u\n", func_start);
            target_shift += get_size(init_insns);
        }

#ifndef MEASURE_BASE 
        uint32_t branch_id =0 ;
        for( const auto & pair_addr_sgpr : save_mask_insns){
            auto addr = pair_addr_sgpr.first;
            auto exec_cond_sgpr = pair_addr_sgpr.second;
            printf("branch id = %d, exec_cond_sgpr = %d\n",branch_id,exec_cond_sgpr);
            vector<MyInsn> update_branch_statistic;
            per_branch_instrumentation(update_branch_statistic, branch_id  , exec_cond_sgpr , c ,insn_pool);
            inplace_insert(fp,func_start,text_end,update_branch_statistic,branches, addr + target_shift,kernel_bounds,endpgms,insn_pool);
            target_shift+= get_size(update_branch_statistic);
            branch_id++;
        }
#endif
        vector<MyInsn> writeback_insns;
        uint32_t my_offset = num_branches * 8;
        setup_writeback(writeback_insns, c ,insn_pool);
        memtime_epilogue(writeback_insns, c, my_offset , insn_pool);
        printf("before inserting writeback\n");

        for (const auto & endpgm : endpgms){
            //printf("before ending, endpgms position = %p\n",endpgm);
            inplace_insert(fp,func_start,text_end,writeback_insns,branches, endpgm ,kernel_bounds,endpgms,insn_pool);
            target_shift+= get_size(writeback_insns);


        }
        /*{
            vector<MyInsn> writeback_insns;
            uint32_t my_offset = num_branches * 8;
            setup_writeback(writeback_insns, c ,insn_pool);
            memtime_epilogue(writeback_insns, c, my_offset , insn_pool);
            printf("before inserting writeback\n");
            inplace_insert(fp,func_start,text_end,writeback_insns,branches, func_end - 4 + target_shift,kernel_bounds,endpgms,insn_pool);
            target_shift+= get_size(writeback_insns);

        }*/
        
        printf("target_shift =%u\n",target_shift);
        uint32_t nop_size = 0;
        if(target_shift%256!=0){
            uint32_t remain = 256 - (target_shift%256);
            uint32_t num_nops = remain / 4;
            vector<MyInsn> nops;
            printf("number of remain = %u, num_nops = %u\n", remain,num_nops);
            for(uint32_t i=0; i <num_nops ;i++){
                nops.push_back(InsnFactory::create_s_nop(insn_pool));
            }
            inplace_insert(fp,func_start,text_end,nops,branches, func_end +target_shift,kernel_bounds,endpgms,insn_pool);
            nop_size += get_size(nops);
        }
        // 0x10b0


        extend_text(fp,target_shift+target_shift);
        set_sgpr_vgpr_usage(fp,c.kd_addr,c.sgpr_max,c.vgpr_max);
        printf("updating sgpr max to %u, vgpr_max to %u\n",c.sgpr_max,c.vgpr_max);
        update_symtab_symbol_size(fp,c.name.c_str(), func_end - func_start + target_shift );
        printf(" increase symbol for kernel %s of size = %u, with size %u instrumentation \n", c.name.c_str(), func_end - func_start, target_shift);
        fclose(fp);
    }

    for (auto &p : insn_pool){
        free(p);    
    }



    return 0;

}




