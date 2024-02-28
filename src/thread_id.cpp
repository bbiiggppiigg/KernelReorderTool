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

#define MEASURE_BASE



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
void setup_initailization2(vector<MyInsn> & ret , config &c , vector<char *> & insn_pool){

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

  uint32_t V_TMP = 4;
  uint32_t V_TMP1 = 5;
  uint32_t V_ADDR = 6;
  uint32_t V_ADDR1 = 7;
  uint32_t V_DEBUG1 = 8;
  uint32_t V_DEBUG2 = 9;
  if(c.vgpr_max < 10)
    c.vgpr_max = 10;

  uint32_t S_PERWF_OFFSET;
  if (c.vgpr_spill ){
    S_PERWF_OFFSET = FIRST_FREE_SGPR + 10;
  }else{
    S_PERWF_OFFSET = c.PER_WAVEFRONT_OFFSET;
  }

  ret.push_back(InsnFactory::create_s_memtime(S_TIMER,insn_pool));
  //ret.push_back(InsnFactory::create_s_mov_b32(M0,S_MINUS_1,false,insn_pool)); // Initialize M0 to -1 to access shared memory
  printf("first uninitialized sgpr = %u, kernarg_segment_ptr = %u \n",FIRST_FREE_SGPR,c.kernarg_segment_ptr);
  printf("work iten enabled = %d\n",c.work_item_id_enabled);
  assert(c.kernarg_segment_ptr != (unsigned) -1);
  if((FIRST_FREE_SGPR % 4) == 0){
    ret.push_back(InsnFactory::create_s_load_dwordx4(FIRST_FREE_SGPR,S_KERNARG, c.old_kernarg_size+12 ,insn_pool));
  }else{
    ret.push_back(InsnFactory::create_s_load_dwordx2(FIRST_FREE_SGPR,S_KERNARG, c.old_kernarg_size+12 ,insn_pool));
    ret.push_back(InsnFactory::create_s_load_dwordx2(FIRST_FREE_SGPR+2,S_KERNARG, c.old_kernarg_size+20 ,insn_pool));
  }

  ret.push_back(InsnFactory::create_s_load_dwordx2(S_WBADDR,S_KERNARG , c.old_kernarg_size ,insn_pool));
  printf("old kernarg size = %u\n",c.old_kernarg_size);
  ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));

  {
    // 1 If Threre are blocks along the y dimension
    //    1-1 If There are threads along the y dimension
    //      G_TID_Y = BID_Y * B_DIM_Y + TID_Y
    //    2-2 Else
    // 2 If there aren't any blocks along the y-dimension
    //    2-1 
    //    2-2
    if(c.work_group_id_y_enabled){
      ret.push_back(InsnFactory::create_s_mul_i32(S_TMP1,S_BLOCKDIM_Y,c.work_group_id_y,insn_pool)); 
      ret.push_back(InsnFactory::create_v_mov_b32(V_TMP1 , S_TMP1, insn_pool)); 
      if(c.work_item_id_enabled > 0){
        ret.push_back(InsnFactory::create_v_add_u32(V_TMP1 , V_TMP1, 257, insn_pool));
      }
    }else{
       if(c.work_item_id_enabled > 0){
        ret.push_back(InsnFactory::create_v_mov_b32(V_TMP1 , 257, insn_pool)); 
      }
    }
    ret.push_back(InsnFactory::create_v_mov_b32(V_DEBUG1 , V_TMP1+256, insn_pool)); 
  }



  {
    // Calculate Thread_ID_X
    ret.push_back(InsnFactory::create_s_mul_i32(S_TMP,S_BLOCKDIM_X,c.work_group_id_x,insn_pool)); 
    ret.push_back(InsnFactory::create_v_mov_b32(V_TMP , S_TMP, insn_pool));
    ret.push_back(InsnFactory::create_v_add_u32(V_TMP , V_TMP, 256, insn_pool)); 
  }

  { // Calculate Flattend Thread ID // in V_TMP
    ret.push_back(InsnFactory::create_s_mul_i32(S_TMP,S_BLOCKDIM_X,S_GRIDDIM_X,insn_pool)); // 128 * 512 = 65536  
    ret.push_back(InsnFactory::create_v_mul_lo_u32(V_TMP1,V_TMP1+256,S_TMP,insn_pool)); 
    ret.push_back(InsnFactory::create_v_add_co_u32(V_TMP , V_TMP+256, V_TMP1+256, insn_pool)); 
    ret.push_back(InsnFactory::create_v_mov_b32(V_DEBUG2 , V_TMP+256, insn_pool)); 

  }

  ret.push_back(InsnFactory::create_v_lshl_b32(V_TMP , V_TMP, 128+2, insn_pool)); 


  ret.push_back(InsnFactory::create_v_mov_b32(V_ADDR,S_WBADDR,insn_pool));
  ret.push_back(InsnFactory::create_v_mov_b32(V_ADDR1,S_WBADDR+1,insn_pool));
  ret.push_back(InsnFactory::create_v_add_co_u32(V_ADDR,V_ADDR,256+V_TMP,insn_pool));
  ret.push_back(InsnFactory::create_v_addc_co_u32(V_ADDR1,V_ADDR1,128,insn_pool));
  //ret.push_back(InsnFactory::create_s_mov_b64(S_TIMER,EXEC,insn_pool));
  //ret.push_back(InsnFactory::create_s_mov_b64(EXEC,S_1,insn_pool));
  //ret.push_back(InsnFactory::create_global_store_dword(V_DEBUG2,V_ADDR+2,0,insn_pool));
  ret.push_back(InsnFactory::create_global_store_dword(V_DEBUG2,V_ADDR,0,insn_pool));
  ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
  //ret.push_back(InsnFactory::create_s_mov_b64(EXEC,S_TIMER,insn_pool));

}



// SAVED :64 bit SGPR PER_WAVEFRONT_OFFSET = S_ADDR
// NEED : 32 bit SGPR S_DATA, for storing SCC

void per_branch_instrumentation(vector<MyInsn> & ret , uint32_t branch_id , uint32_t execcond , config c, vector<char *> & insn_pool){


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


    ret.push_back(InsnFactory::create_v_writelane_b32(c.V_SPILL,130,S_PERWF_OFFSET,insn_pool)); // VSPILL
    ret.push_back(InsnFactory::create_v_writelane_b32(c.V_SPILL,131,S_PERWF_OFFSET+1,insn_pool)); // VSPILL

    ret.push_back(InsnFactory::create_v_writelane_b32(c.V_SPILL,132,SDATA,insn_pool)); // VSPILL


    ret.push_back(InsnFactory::create_v_readlane_b32(S_PERWF_OFFSET,128,c.V_SPILL+256,insn_pool));
    ret.push_back(InsnFactory::create_v_readlane_b32(S_PERWF_OFFSET+1,129,c.V_SPILL+256,insn_pool));
  }


  // S_ADDR_PAIR = PER_WAVEFRONT_BASE + ( BRANCH_ID * 8 ( each branch takes 8 bytes ) )
  ret.push_back(InsnFactory::create_s_cmp_eq_u64(EXEC,EXECCOND,insn_pool)); // CHECK IF backuped_exec == exec && cond
  ret.push_back(InsnFactory::create_s_mov_b32(SDATA, SCC, false ,insn_pool));
  ret.push_back(InsnFactory::create_s_atomic_add(SDATA, S_ADDR , local_offset , insn_pool));
  ret.push_back(InsnFactory::create_s_cmp_eq_u64(EXEC,S_0,insn_pool)); // CHECK IF backuped_exec = 0
  ret.push_back(InsnFactory::create_s_mov_b32(SDATA, SCC, false ,insn_pool));
  ret.push_back(InsnFactory::create_s_atomic_add(SDATA, S_ADDR , local_offset , insn_pool));
  ret.push_back(InsnFactory::create_s_mov_b32(SDATA, 193, false ,insn_pool));
  ret.push_back(InsnFactory::create_s_atomic_inc(SDATA, S_ADDR , local_offset+4 , insn_pool));





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

  //ret.push_back(InsnFactory::create_s_store_dword_x2(c.TIMER_1 ,c.TMP_SGPR0, my_offset, insn_pool)); 

  ret.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
  ret.push_back(InsnFactory::create_s_store_dword_x2(S_TIMER ,S_PERWF_OFFSET,my_offset +  8, insn_pool)); 
  ret.push_back(InsnFactory::create_s_dcache_wb(insn_pool));
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


    printf("before initialization \n");
    {
      vector<MyInsn> init_insns;
      setup_initailization2(init_insns,c,insn_pool);

      inplace_insert( mkds, mkd_id, init_insns, mkds[mkd_id].start_addr);
      //inplace_insert( cur_kd, init_insns, cur_kd.start_addr, mkds);
      target_shift += getSize(init_insns);
    }
    dumpBuffers(mkds);
    vector<MyInsn> writeback_insns;
    uint32_t my_offset = num_branches * 8;
    set_sgpr_vgpr_usage(fp,c.kd_addr,c.sgpr_max,c.vgpr_max);
    propogate_mkd_update(mkds,mkd_id); 
    dumpBuffers(mkds);

    for(unsigned long ii =0 ; ii < mkds.size(); ii++){
      update_function_symbol(fp,mkds[ii].name.c_str(),mkds[ii].start_addr, mkds[ii].end_addr - mkds[ii].start_addr);
    }

  }
  fclose(fp);
  finalize(binaryPath,mkds,text_offset);

  for (auto &p : insn_pool){
    free(p);    
  }



  return 0;

}





