#include "InstrUtil.h"
#include <cassert>
using namespace std;
class MyInsn;

uint32_t insert_tramp( FILE * f, vector<MyInsn> & prologues, vector<MyInsn> & epilogues,uint32_t start_included, uint32_t end_excluded, uint32_t tramp_location, uint32_t scc_store_index ,  vector<char *> & insn_pool){
    // Read the block to patch in to a buffer
    unsigned int bb_size = end_excluded - start_included;
    void * buffer_block =  malloc(bb_size);
    fseek(f,start_included,SEEK_SET);
    fread(buffer_block,1,bb_size,f);

    // setup branch from original block to tramp
    fseek(f,start_included,SEEK_SET);
    InsnFactory::create_s_branch(start_included,tramp_location, insn_pool ).write(f);
    
    
    auto save_scc = InsnFactory::create_s_mov_b64(scc_store_index,253,insn_pool);
    auto restore_scc = InsnFactory::create_s_and_b32(scc_store_index,scc_store_index,scc_store_index,insn_pool);

    unsigned int f_index = 0;
    fseek(f,tramp_location,SEEK_SET);
    if ( prologues.size() != 0 ){
        save_scc.write(f);
        f_index+= save_scc.size;
        for ( auto &insn : prologues ){
            insn.write(f);
            f_index += insn.size;    
        }
        restore_scc.write(f);
        f_index+= restore_scc.size;

    }

    fwrite(buffer_block,1,bb_size,f);
    f_index += bb_size;

    if ( epilogues.size() != 0 ){
        save_scc.write(f);
        f_index+= save_scc.size;

        for ( auto &insn : epilogues ){
            insn.write(f);
            f_index += insn.size;    
        }
        restore_scc.write(f);
        f_index+= restore_scc.size;


    }

    InsnFactory::create_s_branch(tramp_location + f_index, end_excluded, insn_pool).write(f);
    return tramp_location + f_index + 4;
}


void set_memtime_pro_ep(vector<MyInsn> & pro, vector<MyInsn> &epi , uint32_t first_record_sgpr, uint32_t second_record_sgpr, uint32_t accum_sgpr,vector<char *> & insn_pool){
    
    pro.push_back(InsnFactory::create_s_memtime(first_record_sgpr,insn_pool));
    epi.push_back(InsnFactory::create_s_memtime(second_record_sgpr,insn_pool));
    epi.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    epi.push_back(InsnFactory::create_s_sub_u32(first_record_sgpr,second_record_sgpr,first_record_sgpr,false,insn_pool));
    epi.push_back(InsnFactory::create_s_subb_u32(first_record_sgpr+1,second_record_sgpr+1,first_record_sgpr+1,false,insn_pool));
    epi.push_back(InsnFactory::create_s_add_u32(accum_sgpr,first_record_sgpr,accum_sgpr,false,insn_pool));
    epi.push_back(InsnFactory::create_s_addc_u32(accum_sgpr+1,first_record_sgpr+1,accum_sgpr+1,false,insn_pool));

 
}

void set_counter_pro_ep(vector<MyInsn> & pro, vector<MyInsn> &epi ,  uint32_t accum_sgpr,vector<char *> & insn_pool){
    
    epi.push_back(InsnFactory::create_s_add_u32(accum_sgpr,accum_sgpr,129,false,insn_pool));
    epi.push_back(InsnFactory::create_s_addc_u32(accum_sgpr+1,accum_sgpr+1,128,false,insn_pool));
}


void set_writeback_pro_ep(vector<MyInsn> & pro, vector<MyInsn> &epi , uint32_t addr_sgpr,uint32_t addr_vgpr, uint32_t acc_sgpr, uint32_t acc_vgpr,vector<char *> & insn_pool){
    
    epi.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    epi.push_back(InsnFactory::create_s_add_u32( addr_sgpr,addr_sgpr ,136  ,false, insn_pool));
    epi.push_back(InsnFactory::create_s_addc_u32( addr_sgpr +1 , addr_sgpr+1  , 128 , false , insn_pool));
    epi.push_back(InsnFactory::create_v_mov_b32( addr_vgpr, addr_sgpr , insn_pool));
    epi.push_back(InsnFactory::create_v_mov_b32( addr_vgpr+1, addr_sgpr+1 , insn_pool));
    epi.push_back(InsnFactory::create_v_mov_b32( acc_vgpr,acc_sgpr , insn_pool));
    epi.push_back(InsnFactory::create_v_mov_b32( acc_vgpr+1,acc_sgpr+1 , insn_pool));
    epi.push_back(InsnFactory::create_flat_store_dword_x2(acc_vgpr,addr_vgpr,0x0,insn_pool ));
}

void move_block_to(FILE * fp, uint32_t start_included, uint32_t end_excluded, uint32_t target_addr){
    assert( target_addr < start_included || target_addr >= end_excluded);

    uint32_t start_addr,end_addr,split_addr; 
    if(target_addr < start_included){
        end_addr = end_excluded;
        start_addr = target_addr;
        split_addr = start_included;
    }else{
        end_addr = target_addr + end_excluded- start_included;
        start_addr = start_included;
        split_addr = end_excluded;
    }
    uint32_t buf_size = end_addr - start_addr;
    uint32_t sh_size = end_addr - split_addr;
    uint32_t fh_size = split_addr - start_addr;
    void * buffer = malloc(sizeof(char) * (buf_size));    
    fseek(fp,start_addr,SEEK_SET);
    fread(buffer,1,buf_size,fp);
    fseek(fp,start_addr,SEEK_SET);
    printf("writing to address : %x\n",start_addr);
    fwrite(buffer+split_addr-start_addr,sh_size,1,fp);
    printf("writing to address : %x\n",start_addr+sh_size);
    fwrite(buffer,fh_size,1,fp);
    printf("first 4bytes of buffer = %x%x\n", * (uint32_t *) buffer, *((uint32_t *) buffer + 1));

}








