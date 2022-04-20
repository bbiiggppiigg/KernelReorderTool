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



uint32_t TARGET_ID = 0;
#if 0
#define m_INIT 0
#define m_BRANCH 0
#define m_BID 0
#define m_WRITEBACK 0
#define m_WRITE_PER_B 0 
#define m_END 0
#endif

typedef uint16_t reg;
typedef uint16_t sreg;
typedef uint16_t vreg;


void setup_initailization(vector<MyInsn> & ret , config c , vector<char *> & insn_pool){

    /**
     * To calculate the local warp id, we need to calculate threadId_Y * Threads_PER_block_X + threadID_X / 64
     * Here we reuse warp_sgpr_pair to do so
     */
    // First step load from dispatch ptr size of block 
    //
    // GLOBAL WORK GROUP ID : (WG_ID_Z * #WG_PER_Y + WG_ID_Y) * #WG_PWE_X + WG_ID_X
    uint32_t TMP_SGPR0 = c.TMP_SGPR0;
    uint32_t TMP_SGPR1 = c.TMP_SGPR1;
    uint32_t GLOBAL_WAVEFRONT_ID = c.GLOBAL_WAVEFRONT_ID;
    uint32_t LOCAL_WAVEFRONT_ID = c.LOCAL_WAVEFRONT_ID;
    uint32_t WORK_GROUP_ID = c.WORK_GROUP_ID;
    uint32_t BACKUP_WRITEBACK_ADDR = c.BACKUP_WRITEBACK_ADDR;
    uint32_t DS_ADDR = c.DS_ADDR;
    uint32_t DS_DATA_0 = c.DS_DATA_0;
    uint32_t DS_DATA_1 = c.DS_DATA_1;
    uint32_t V_MINUS_1 = c.V_MINUS_1;

    ret.push_back(InsnFactory::create_s_mov_b64(c.TIMER_1,S_0,insn_pool)); // EXEC = 1 
    ret.push_back(InsnFactory::create_s_mov_b64(c.TIMER_2,S_0,insn_pool)); // EXEC = 1 
    #ifdef m_CONSEQ
        ret.push_back(InsnFactory::create_s_memtime(c.TIMER_1,insn_pool));
        ret.push_back(InsnFactory::create_s_memtime(c.TIMER_2,insn_pool));
    #endif
    #ifdef m_INIT
        ret.push_back(InsnFactory::create_s_memtime(c.TIMER_1,insn_pool));
    #endif
    if(c.work_group_id_z_enabled){
        ret.push_back(InsnFactory::create_s_mov_b32(TMP_SGPR0,c.work_group_id_z,false,insn_pool));  
        ret.push_back(InsnFactory::create_s_mov_b32(TMP_SGPR1,c.grid_dim_y,true,insn_pool));
        ret.push_back(InsnFactory::create_s_mul_i32(TMP_SGPR0,TMP_SGPR1,TMP_SGPR0,insn_pool));
    }else{
        ret.push_back(InsnFactory::create_s_mov_b32(TMP_SGPR0,0,true,insn_pool));  
    }

    if(c.work_group_id_y_enabled){
        ret.push_back(InsnFactory::create_s_add_u32(TMP_SGPR0,TMP_SGPR0,c.work_group_id_y,false,insn_pool));
    }
    ret.push_back(InsnFactory::create_s_mov_b32(TMP_SGPR1,c.grid_dim_x,true,insn_pool));  
    ret.push_back(InsnFactory::create_s_mul_i32(WORK_GROUP_ID,TMP_SGPR1,TMP_SGPR0,insn_pool));

    if(c.work_group_id_x_enabled){
        ret.push_back(InsnFactory::create_s_add_u32(WORK_GROUP_ID,WORK_GROUP_ID,c.work_group_id_x,false,insn_pool));
    }
    // AFTER THIS STEP, a global WG_ID is stored in WORK_GROUP_ID 


    // THREAD_ID = (THREAD_ID_Z * WG_DIM_Y +  TID_Y ) * WG_DIM_X + TID_x
    if(c.work_item_id_enabled > 1){ // vgpr2 enabled
        ret.push_back(InsnFactory::create_v_readfirstlane_b32(TMP_SGPR0,258,insn_pool));  // 258 is VGPR2 in this encoding
        ret.push_back(InsnFactory::create_s_mov_b32(TMP_SGPR1,c.work_group_dim_y,true,insn_pool));
        ret.push_back(InsnFactory::create_s_mul_i32(TMP_SGPR0,TMP_SGPR1,TMP_SGPR0,insn_pool));
    }else{
        ret.push_back(InsnFactory::create_s_mov_b32(TMP_SGPR0,0,true,insn_pool));  
    }
    if(c.work_item_id_enabled > 0){ // vgpr2 enabled
        ret.push_back(InsnFactory::create_v_readfirstlane_b32(TMP_SGPR1,257,insn_pool));  // 257 is VGPR1 in this encoding
        ret.push_back(InsnFactory::create_s_add_u32(TMP_SGPR0,TMP_SGPR1,TMP_SGPR0,false,insn_pool));
    }
    ret.push_back(InsnFactory::create_s_mov_b32(TMP_SGPR1,c.work_group_dim_x,true,insn_pool));  
    ret.push_back(InsnFactory::create_s_mul_i32(TMP_SGPR0,TMP_SGPR1,TMP_SGPR0,insn_pool));





    ret.push_back(InsnFactory::create_v_readfirstlane_b32(TMP_SGPR1,256,insn_pool));  // 256 is VGPR0 in this encoding
    ret.push_back(InsnFactory::create_s_add_u32(TMP_SGPR0,TMP_SGPR1,TMP_SGPR0,false,insn_pool));
    // AFTER THIS STEP, we have a thread ID, now we want warp id , divide by 64

    ret.push_back(InsnFactory::create_s_lshr_b32(LOCAL_WAVEFRONT_ID,S_6,TMP_SGPR0 ,insn_pool)); 


    // NOw we want to calculate global wavefront id 
    // Simple : GLOBAL_WAVEFRONT_ID = WORK_GROUP_ID * # WAVE_FRONT_PER_WORK_GROUP + LOCAL_WAVEFRONT_ID
    //


    ret.push_back(InsnFactory::create_s_mov_b32(TMP_SGPR1,c.wavefront_per_work_group ,true,insn_pool));
    ret.push_back(InsnFactory::create_s_mul_i32(TMP_SGPR0,WORK_GROUP_ID,TMP_SGPR1,insn_pool));
    ret.push_back(InsnFactory::create_s_add_u32(GLOBAL_WAVEFRONT_ID,TMP_SGPR0,LOCAL_WAVEFRONT_ID,false,insn_pool));




    ret.push_back(InsnFactory::create_s_mov_b32(M0,S_MINUS_1,false,insn_pool)); // Initialize M0 to -1 to access shared memory


    //* Here we initialize shared memory with 0, and vregs with 0 except one holding -1 for the purpose of ds_inc

    ret.push_back(InsnFactory::create_s_mov_b32(TMP_SGPR0, c.lds_base,true,insn_pool));
    ret.push_back(InsnFactory::create_v_mov_b32(DS_ADDR , TMP_SGPR0, insn_pool)); 
    ret.push_back(InsnFactory::create_v_mov_b32(DS_DATA_0 , S_0, insn_pool)); 
    ret.push_back(InsnFactory::create_v_mov_b32(DS_DATA_1 , S_0, insn_pool)); 
    ret.push_back(InsnFactory::create_v_mov_b32(V_MINUS_1 , S_MINUS_1, insn_pool)); 

    for ( uint32_t i =0 ; i < c.num_branches; i++){
        ret.push_back(InsnFactory::create_ds_write_b64(DS_ADDR,DS_DATA_0,insn_pool));   // ds[0:1]=0
        ret.push_back(InsnFactory::create_v_add_u32(DS_ADDR,DS_ADDR,S_8,insn_pool)); 
    }


    // Backup Writeback Address 
    ret.push_back(InsnFactory::create_s_load_dwordx2(BACKUP_WRITEBACK_ADDR,c.kernarg_segment_ptr,c.writeback_param_offset,insn_pool)); 
    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    #ifdef m_INIT
        ret.push_back(InsnFactory::create_s_memtime(c.TIMER_2,insn_pool));
    #endif


}



void per_branch_instrumentation(vector<MyInsn> & ret , uint32_t branch_id , uint32_t execcond , config c, vector<char *> & insn_pool){


    uint32_t BACKUP_EXEC = c.BACKUP_EXEC ;
    uint32_t DS_ADDR_OFFSET = c.TMP_SGPR0; // temp scalar register used to calculate the lds address, which willl later be moved into a vector register
    uint32_t DS_DATA = c.DS_DATA_0; 
    uint32_t EXECCOND = execcond;
    uint32_t V_DS_ADDR_OFFSET = c.DS_ADDR;
    #ifdef m_BRANCH
        ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
        if(TARGET_ID == branch_id)
            ret.push_back(InsnFactory::create_s_memtime(c.TIMER_1,insn_pool));
    #endif

    ret.push_back(InsnFactory::create_s_mov_b64(BACKUP_EXEC,EXEC,insn_pool)); // backup EXEC
    ret.push_back(InsnFactory::create_s_mov_b64(EXEC,S_1,insn_pool)); // EXEC = 1 
    ret.push_back(InsnFactory::create_s_mul_i32(DS_ADDR_OFFSET,c.LOCAL_WAVEFRONT_ID, c.num_branches+2 +128  ,insn_pool)); 
    ret.push_back(InsnFactory::create_s_add_u32(DS_ADDR_OFFSET,DS_ADDR_OFFSET,branch_id+128,false,insn_pool));
    ret.push_back(InsnFactory::create_s_lshl_b32(DS_ADDR_OFFSET,S_3, DS_ADDR_OFFSET  ,insn_pool)); // LEFT SHIFT By 3 = times 8 
    ret.push_back(InsnFactory::create_s_add_u32(DS_ADDR_OFFSET, DS_ADDR_OFFSET,c.lds_base,true  ,insn_pool)); // LEFT SHIFT By 3 = times 8 

    ret.push_back(InsnFactory::create_v_mov_b32(V_DS_ADDR_OFFSET, DS_ADDR_OFFSET, insn_pool)); //  vgpr stores the addr
    // SO NOW THIS ADDRESS is basically 
    //
    // LDS usage is per work group, thus we only need to use local wavefront id to index it
    // The problem is that the application can itself already be using shared memory , so we add and base to it
    // Moreover, for each wavefront, for each branch, 8 bytes are required , thus
    //
    // DS_ADDR_OFFSET = (LOCAL_WAVEFRONT_ID * NUM_BRANCHES +  BRANCH_ID ) * 8 + LDS_BASE 
    //


    //instr_insns.push_back(InsnFactory::create_v_mov_b32(v_lds_addr, s_addr_cal, insn_pool)); //  vgpr stores the addr


    ret.push_back(InsnFactory::create_s_cmp_eq_u64(BACKUP_EXEC,EXECCOND,insn_pool)); // CHECK IF backuped_exec == exec && cond
    ret.push_back(InsnFactory::create_v_mov_b32( DS_DATA, SCC, insn_pool)); // vgpr stores the value (SCC)
    ret.push_back(InsnFactory::create_ds_add_u32( V_DS_ADDR_OFFSET, DS_DATA,insn_pool)); // increment the counter
    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));

    ret.push_back(InsnFactory::create_s_cmp_eq_u64(BACKUP_EXEC,S_0,insn_pool)); // CHECK IF S[16:17] == 0
    ret.push_back(InsnFactory::create_v_mov_b32(DS_DATA , SCC, insn_pool)); // v3 stores the value (SCC)
    ret.push_back(InsnFactory::create_ds_add_u32(V_DS_ADDR_OFFSET,DS_DATA,insn_pool));
    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));

    ret.push_back(InsnFactory::create_v_add_u32(V_DS_ADDR_OFFSET,V_DS_ADDR_OFFSET,S_4,insn_pool)); 
    ret.push_back(InsnFactory::create_ds_inc_u32(V_DS_ADDR_OFFSET,c.V_MINUS_1,insn_pool));

    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    ret.push_back(InsnFactory::create_s_mov_b64(EXEC,BACKUP_EXEC,insn_pool)); // Use SGPR[16:17] to backup EXEC
    #ifdef m_BRANCH
        if(TARGET_ID == branch_id)
            ret.push_back(InsnFactory::create_s_memtime(c.TIMER_2,insn_pool));
    #endif



}

/*
 * To setup the write back, what we need to do is 
 * 1. Let LDS_ADDR points to LDS_BASE 
 * 2. Let WRITE_BACK_PTR points to WRITEBACK_ADDR + OFFSET
 */
void setup_writeback(vector<MyInsn> & ret , config c, vector<char *> & insn_pool){
    // DATA Collection is per warp, so we disabled rest of the threads
    ret.push_back(InsnFactory::create_s_mov_b64(EXEC,S_1,insn_pool)); // EXEC = 1 

    #ifdef m_WRITEBACK
        ret.push_back(InsnFactory::create_s_memtime(c.TIMER_1,insn_pool));
    #endif




    //ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    //  simple
    ret.push_back(InsnFactory::create_v_mov_b32_const(c.DS_ADDR , c.lds_base, insn_pool)); 



    uint32_t GLOBAL_WRITEBACK_PTR = c.TMP_SGPR0; // we use TMP_SGPR0 TMP_SGPR1 as they are continuous pair of SGPR
    //
    // GLOBAL_WRITEBACK_PTR should points to WRITEBACK_ADDR +  OFFSET + GLOBAL_WAVEFRONT_ID * NUM_BRANCHES * 8 
    //

    ret.push_back(InsnFactory::create_s_mul_i32(GLOBAL_WRITEBACK_PTR,c.GLOBAL_WAVEFRONT_ID,c.num_branches+2+128,insn_pool)); 
    ret.push_back(InsnFactory::create_s_lshl_b32(GLOBAL_WRITEBACK_PTR,S_3, GLOBAL_WRITEBACK_PTR,insn_pool)); // LEFT SHIFT By 3 = times 8 


    ret.push_back(InsnFactory::create_s_add_u32(GLOBAL_WRITEBACK_PTR   , GLOBAL_WRITEBACK_PTR , c.BACKUP_WRITEBACK_ADDR , false ,insn_pool));
    ret.push_back(InsnFactory::create_s_addc_u32(GLOBAL_WRITEBACK_PTR+1,                   S_0, c.BACKUP_WRITEBACK_ADDR+1 , false ,insn_pool));  

    ret.push_back(InsnFactory::create_s_add_u32(GLOBAL_WRITEBACK_PTR   , GLOBAL_WRITEBACK_PTR , c.writeback_offset, true ,insn_pool));
    ret.push_back(InsnFactory::create_s_addc_u32(GLOBAL_WRITEBACK_PTR+1,                   S_0, GLOBAL_WRITEBACK_PTR +1, false ,insn_pool));  

    // Replace the add with 

    ret.push_back(InsnFactory::create_v_mov_b32(c.v_global_addr , GLOBAL_WRITEBACK_PTR, insn_pool)); // v[0] = s[0]
    ret.push_back(InsnFactory::create_v_mov_b32(c.v_global_addr+1 , GLOBAL_WRITEBACK_PTR+1, insn_pool)); // v[1 ]= s[1] 
    #ifdef m_WRITEBACK
        ret.push_back(InsnFactory::create_s_memtime(c.TIMER_2,insn_pool));
    #endif




}


void per_branch_writeback(vector<MyInsn> & ret,  config c , uint32_t bid, vector<char *> & insn_pool){
    #ifdef m_WRITE_PER_B
        if(bid == TARGET_ID)
            ret.push_back(InsnFactory::create_s_memtime(c.TIMER_1,insn_pool));
    #endif


    ret.push_back(InsnFactory::create_ds_read_b64(c.DS_ADDR,c.DS_DATA_0,insn_pool)); // from shared[0] read 64 bits = 2 32_bits, into v[2:3]
    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    ret.push_back(InsnFactory::create_global_store_dword_x2(c.DS_DATA_0,c.v_global_addr,0,insn_pool)); // store v[2:3] in to address poitned by v[0:1]

    ret.push_back(InsnFactory::create_v_add_u32(c.DS_ADDR,c.DS_ADDR,S_8,insn_pool)); 
    ret.push_back(InsnFactory::create_v_add_co_u32(c.v_global_addr,c.v_global_addr,S_8,insn_pool)); 
    ret.push_back(InsnFactory::create_v_addc_co_u32(c.v_global_addr+1,c.v_global_addr+1,S_0,insn_pool)); 
    #ifdef m_WRITE_PER_B
        if(bid == TARGET_ID)
            ret.push_back(InsnFactory::create_s_memtime(c.TIMER_2,insn_pool));
    #endif




}

void memtime_epilogue(vector<MyInsn> & ret,  config c ,vector<char *> & insn_pool){





    //ret.push_back(InsnFactory::create_s_memtime(c.TIMER_2,insn_pool));

    // HERE WE REUSE the Vector Register for DS_DATA to write back results


    ret.push_back(InsnFactory::create_v_mov_b32(c.DS_DATA_0 , c.TIMER_1 , insn_pool)); 
    ret.push_back(InsnFactory::create_v_mov_b32(c.DS_DATA_1 , c.TIMER_1+1, insn_pool)); 

    ret.push_back(InsnFactory::create_global_store_dword_x2(c.DS_DATA_0,c.v_global_addr,0,insn_pool)); // store v[2:3] in to address poitned by v[0:1]

    ret.push_back(InsnFactory::create_v_add_co_u32(c.v_global_addr,c.v_global_addr,S_8,insn_pool)); 
    ret.push_back(InsnFactory::create_v_addc_co_u32(c.v_global_addr+1,c.v_global_addr+1,S_0,insn_pool)); 


    ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));

    ret.push_back(InsnFactory::create_v_mov_b32(c.DS_DATA_0 , c.TIMER_2 , insn_pool)); // v[0] = s[0]
    ret.push_back(InsnFactory::create_v_mov_b32(c.DS_DATA_1 , c.TIMER_2+1, insn_pool)); // v[1 ]= s[1] 

    ret.push_back(InsnFactory::create_global_store_dword_x2(c.DS_DATA_0,c.v_global_addr,0,insn_pool)); // store v[2:3] in to address poitned by v[0:1]


}





int main(int argc, char **argv){

    vector < char *> insn_pool;



    if(argc < 3){
        printf("Usage: %s <binary path> <config_file>\n",argv[0]);
    }
    if(argc >=4){
        TARGET_ID = atoi(argv[3]);
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
        analyze_binary(binaryPath, edges,  save_mask_insns, func_start , func_end); 


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

            inplace_insert(fp,func_start,text_end,init_insns,branches, func_start+ target_shift,kernel_bounds,insn_pool);
            printf("patching start at address offset %u\n", func_start);
            target_shift += get_size(init_insns);
        }


        uint32_t branch_id =0 ;
        for( const auto & pair_addr_sgpr : save_mask_insns){
            auto addr = pair_addr_sgpr.first;
            auto exec_cond_sgpr = pair_addr_sgpr.second;
            //printf("branch id = %d, exec_cond_sgpr = %d\n",branch_id,exec_cond_sgpr);
            vector<MyInsn> update_branch_statistic;
            per_branch_instrumentation(update_branch_statistic, branch_id  , exec_cond_sgpr , c ,insn_pool);
            inplace_insert(fp,func_start,text_end,update_branch_statistic,branches, addr + target_shift,kernel_bounds,insn_pool);
            target_shift+= get_size(update_branch_statistic);
            branch_id++;
        }
        {
            vector<MyInsn> writeback_insns;

            setup_writeback(writeback_insns, c ,insn_pool);
            for ( uint32_t i =0 ; i < num_branches ; i++)
                per_branch_writeback(writeback_insns, c,i , insn_pool);


            memtime_epilogue(writeback_insns, c, insn_pool);
            printf("before inserting writeback\n");
            inplace_insert(fp,func_start,text_end,writeback_insns,branches, func_end - 4 + target_shift,kernel_bounds,insn_pool);
            target_shift+= get_size(writeback_insns);

        }
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
            inplace_insert(fp,func_start,text_end,nops,branches, func_end +target_shift,kernel_bounds,insn_pool);
            nop_size += get_size(nops);
        }
        // 0x10b0


        extend_text(fp,target_shift+target_shift);
        set_sgpr_vgpr_usage(fp,c.kd_addr,c.WORK_GROUP_ID+4,c.v_global_addr+4);
        update_symtab_symbol_size(fp,c.name.c_str(), func_end - func_start + target_shift );
        printf(" increase symbol for kernel %s of size = %u, with size %u instrumentation \n", c.name.c_str(), func_end - func_start, target_shift);
        fclose(fp);
    }

    for (auto &p : insn_pool){
        free(p);    
    }



    return 0;

}





