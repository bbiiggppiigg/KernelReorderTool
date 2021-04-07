#include <elf.h>
#include <vector>
#include <iostream>
#include "CodeObject.h"
#include "InstructionDecoder.h"
#include "CFG.h"
using namespace std;
using namespace Dyninst;
using namespace ParseAPI;

using namespace InstructionAPI;


const char * s_nop = "\x00\x00\x80\xbf";
class MyInsn {
    public:
        const void * ptr;
        unsigned int size;
        string _pretty;
        MyInsn( const void * _ptr , unsigned int _size, string pretty) : ptr(_ptr) , size(_size), _pretty(pretty) {

        };
        void write(FILE* fp){
            fwrite(ptr,size,1,fp);
        }

};

/*class MyWaitCntInsn : public MyInsn {
    public:
    uint32_t insn; 
    MyWaitCntInsn (char * my_ptr , int vmcnt, int lgkmcnt , int expcnt = 0 ) : MyInsn(my_ptr,4, "s_waitcnt"){
        this->insn = 0xbf8c0000;
        uint16_t simm16 = 0 ;

        uint16_t vm_low = vmcnt & 0xf;
        uint16_t vm_high = (vmcnt & 0x30) >> 4;
        uint16_t exp_cnt = expcnt & 0x7; 
        uint16_t lgkm_cnt = lgkmcnt & 0x1f;
        simm16 = (vm_high << 14) | (lgkm_cnt << 8) | (exp_cnt << 4) | (vm_low);
        insn |= simm16;
        std::ostringstream oss; 
        memcpy(my_ptr,&insn,32);

        oss << "s_waitcnt  " << "vmcnt(" << vmcnt << ")" << " lgkmcnt(" << lgkmcnt << ")" << " expcnt(" << expcnt  << ")";
        this->_pretty = oss.str();
        printf("after constructing waitcnt insn, value = %x\n",insn);
        
    };
};*/


class InsnFactory {
    
public:
// SOP1

    static MyInsn create_s_mov_b64( uint32_t sdst, uint32_t ssrc0, vector<char *> & insn_pool ){
        uint32_t cmd = 0xbe800000;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 4 );
        uint32_t op = 1;
        cmd  = ( cmd | (sdst<< 16) | (op << 8) |ssrc0);
        memcpy( cmd_ptr ,&cmd,  4 );
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,4,std::string("s_mov_b64 "));
    }


//
    static MyInsn create_s_nop( vector<char *> & insn_pool ){
        uint32_t cmd = 0xbf800000;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 4 );
        memcpy( cmd_ptr ,&cmd,  4 );
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,4,std::string("s_nop "));
    }


// 
    static MyInsn create_s_wait_cnt( vector<char *> & insn_pool ){
        uint32_t cmd = 0xbf8c007f;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 4 );
        memcpy( cmd_ptr ,&cmd,  4 );
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,4,std::string("s_waitcnt_0 "));
    }
// VOP1

    static MyInsn create_v_mov_b32(  uint32_t vdst, uint32_t src  , vector<char *> & insn_pool ){
        uint32_t cmd = 0x7e000000;
        uint32_t op = 1;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 4 );
        cmd = ( cmd | (vdst << 17) | ( op << 9)  | src);
        memcpy( cmd_ptr ,&cmd,  4 );
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,4,std::string("v_mov_b32 "));
    }

// SOPK
    static MyInsn create_s_movk_i32(  uint32_t target_sgpr, int16_t simm16  , vector<char *> & insn_pool ){
        uint32_t cmd = 0xb0000000;
        uint32_t op = 0x0;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 4 );
        cmd = ( cmd | (op << 23) | (target_sgpr << 16)  | simm16 );
        memcpy( cmd_ptr ,&cmd,  4 );
        printf("creating s_movk_i32 s[%u] = %d, cmd = 0x%x\n",target_sgpr,simm16,cmd); 
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,4,std::string("s_movk_i32 ")+std::to_string(simm16));
    }
// SOP2
   static MyInsn create_s_and_b32(  uint32_t sdst, uint32_t ssrc1, uint32_t ssrc0, vector<char *> & insn_pool ){
        uint32_t cmd = 0x80000000;
        uint32_t op = 12;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 4 );
        cmd = ( cmd | (op << 23) | ( sdst << 16) | (ssrc1 << 8)  | ssrc0 );
        memcpy( cmd_ptr ,&cmd,  4 );
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,4,std::string("s_and_b32 "));
    }


   static MyInsn create_s_add_u32(  uint32_t sdst, uint32_t ssrc1, uint32_t ssrc0  , bool useImm , vector<char *> & insn_pool ){
        uint32_t cmd = 0x80000000;
        uint32_t op = 0x0;

        if(useImm){
            char * cmd_ptr = (char *   ) malloc(sizeof(char) * 8 );
            cmd = ( cmd | (op << 23) | ( sdst << 16) | (ssrc1 << 8)  | 0xff );
            memcpy( cmd_ptr ,&cmd,  4 );
            memcpy( cmd_ptr+4 ,&ssrc0,  4 );
            insn_pool.push_back(cmd_ptr);
            return MyInsn(cmd_ptr,8,std::string("s_add_u32 "));

            
        }else{
            char * cmd_ptr = (char *   ) malloc(sizeof(char) * 4 );
            cmd = ( cmd | (op << 23) | ( sdst << 16) | (ssrc1 << 8)  | ssrc0 );
            memcpy( cmd_ptr ,&cmd,  4 );
            insn_pool.push_back(cmd_ptr);
            return MyInsn(cmd_ptr,4,std::string("s_add_u32 "));
        }
    }


   static MyInsn create_s_addc_u32(  uint32_t sdst, uint32_t ssrc1, uint32_t ssrc0  , bool useImm , vector<char *> & insn_pool ){
        uint32_t cmd = 0x80000000;
        uint32_t op = 0x4;

        if(useImm){
            char * cmd_ptr = (char *   ) malloc(sizeof(char) * 8 );
            cmd = ( cmd | (op << 23) | ( sdst << 16) | (ssrc1 << 8)  | 0xff );
            memcpy( cmd_ptr ,&cmd,  4 );
            memcpy( cmd_ptr+4 ,&ssrc0,  4 );
            insn_pool.push_back(cmd_ptr);
            return MyInsn(cmd_ptr,8,std::string("s_add_u32 "));

            
        }else{
            char * cmd_ptr = (char *   ) malloc(sizeof(char) * 4 );
            cmd = ( cmd | (op << 23) | ( sdst << 16) | (ssrc1 << 8)  | ssrc0 );
            memcpy( cmd_ptr ,&cmd,  4 );
            insn_pool.push_back(cmd_ptr);
            return MyInsn(cmd_ptr,4,std::string("s_add_u32 "));
        }
    }
   static MyInsn create_s_sub_u32(  uint32_t sdst, uint32_t ssrc0, uint32_t ssrc1  , bool useImm , vector<char *> & insn_pool ){
        uint32_t cmd = 0x80000000;
        uint32_t op = 0x1;

        if(useImm){
            char * cmd_ptr = (char *   ) malloc(sizeof(char) * 8 );
            cmd = ( cmd | (op << 23) | ( sdst << 16) | (ssrc1 << 8)  | 0xff );
            memcpy( cmd_ptr ,&cmd,  4 );
            memcpy( cmd_ptr+4 ,&ssrc0,  4 );
            insn_pool.push_back(cmd_ptr);
            return MyInsn(cmd_ptr,8,std::string("s_add_u32 "));

            
        }else{
            char * cmd_ptr = (char *   ) malloc(sizeof(char) * 4 );
            cmd = ( cmd | (op << 23) | ( sdst << 16) | (ssrc1 << 8)  | ssrc0 );
            memcpy( cmd_ptr ,&cmd,  4 );
            insn_pool.push_back(cmd_ptr);
            return MyInsn(cmd_ptr,4,std::string("s_sub_u32 "));
        }
    }


   static MyInsn create_s_subb_u32(  uint32_t sdst, uint32_t ssrc0, uint32_t ssrc1  , bool useImm , vector<char *> & insn_pool ){
        uint32_t cmd = 0x80000000;
        uint32_t op = 0x5;

        if(useImm){
            char * cmd_ptr = (char *   ) malloc(sizeof(char) * 8 );
            cmd = ( cmd | (op << 23) | ( sdst << 16) | (ssrc1 << 8)  | 0xff );
            memcpy( cmd_ptr ,&cmd,  4 );
            memcpy( cmd_ptr+4 ,&ssrc0,  4 );
            insn_pool.push_back(cmd_ptr);
            return MyInsn(cmd_ptr,8,std::string("s_subb_u32 "));

            
        }else{
            char * cmd_ptr = (char *   ) malloc(sizeof(char) * 4 );
            cmd = ( cmd | (op << 23) | ( sdst << 16) | (ssrc1 << 8)  | ssrc0 );
            memcpy( cmd_ptr ,&cmd,  4 );
            insn_pool.push_back(cmd_ptr);
            return MyInsn(cmd_ptr,4,std::string("s_add_u32 "));
        }
    }



//
    static MyInsn create_s_branch( uint32_t pc , uint32_t target  , vector<char *> & insn_pool ){
        uint32_t cmd = 0xbf800000;
        uint32_t op = 0x2;
        uint16_t simm16 = (( target - pc  - 4 )  / 4)  & 0xffff;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 4 );
        cmd = ( cmd | (op << 16) | simm16 );
        memcpy( cmd_ptr ,&cmd,  4 );
        printf("creating s_branch from %x to %x, cmd = 0x%x\n",pc,target,cmd); 
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,4,std::string("s_branch ")+std::to_string(simm16));
    }
    static MyInsn create_s_memtime( uint32_t sgpr_pair  , vector<char *> & insn_pool ){
        uint32_t cmd_low = 0xc0000000;
        uint32_t cmd_high = 0x0;
        uint32_t op = 0x24;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 8 );
        cmd_low = ( cmd_low | (op << 18) | sgpr_pair << 6);
        memcpy( cmd_ptr ,&cmd_low,  4 );
        memcpy( cmd_ptr +4 ,&cmd_high,  4 );
        printf("creating s_memtime sgpr%d 0x%x%x\n",sgpr_pair,cmd_low,cmd_high); 
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,8,std::string("s_memtime ")+std::to_string(sgpr_pair));
    }
    static MyInsn create_s_load_dword_x4( uint32_t sgpr_target_pair, uint32_t sgpr_addr_pair  ,  uint32_t offset,vector<char *> & insn_pool ){
        uint32_t cmd_low = 0xc0000000;
        uint32_t cmd_high = 0x0;
        uint32_t op = 0x2;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 8 );
        uint32_t imm = 1;
        cmd_low = ( cmd_low | (op << 18) | (imm << 17) |   sgpr_target_pair << 6 | (sgpr_addr_pair >> 1) );

        cmd_high = ( cmd_high | offset );
        memcpy( cmd_ptr ,&cmd_low,  4 );
        memcpy( cmd_ptr +4 ,&cmd_high,  4 );
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,8,std::string("s_load_dwordx4 ")+std::to_string(sgpr_target_pair));
    }
    static MyInsn create_s_store_dword_x4( uint32_t sgpr_target_pair, uint32_t sgpr_addr_pair  ,  uint32_t offset,vector<char *> & insn_pool ){
        uint32_t cmd_low = 0xc0000000;
        uint32_t cmd_high = 0x0;
        uint32_t op = 18;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 8 );
        uint32_t imm = 1;
        cmd_low = ( cmd_low | (op << 18) | (imm << 17) |   sgpr_target_pair << 6 | (sgpr_addr_pair >> 1) );

        cmd_high = ( cmd_high | offset );
        memcpy( cmd_ptr ,&cmd_low,  4 );
        memcpy( cmd_ptr +4 ,&cmd_high,  4 );
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,8,std::string("s_load_dwordx4 ")+std::to_string(sgpr_target_pair));
    }
    
     static MyInsn create_s_store_dword_x2( uint32_t s_data_pair, uint32_t s_base_pair  ,  uint32_t offset,vector<char *> & insn_pool ){
        uint32_t cmd_low = 0xc0000000;
        uint32_t cmd_high = 0x0;
        uint32_t op = 17;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 8 );
        uint32_t imm = 0;
        cmd_low = ( cmd_low | (op << 18) | (imm << 17) |   s_data_pair << 6 | (s_base_pair >> 1) );

        cmd_high = ( cmd_high | offset );
        memcpy( cmd_ptr ,&cmd_low,  4 );
        memcpy( cmd_ptr +4 ,&cmd_high,  4 );
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,8,std::string("s_store_dword_x2 "));
    }
    
      static MyInsn create_flat_store_dword_x2( uint32_t s_data_pair, uint32_t s_base_pair  ,  uint32_t offset,vector<char *> & insn_pool ){
        uint32_t cmd_low = 0xdc000000;
        uint32_t cmd_high = 0x0;
        uint32_t op = 29;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 8 );
        cmd_low = (cmd_low | (op<< 18 )  | offset );

        cmd_high = ( cmd_high | s_data_pair << 8 | s_base_pair );
        memcpy( cmd_ptr ,&cmd_low,  4 );
        memcpy( cmd_ptr +4 ,&cmd_high,  4 );
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,8,std::string("flat_store_dword_x4 "));
    }
     


};


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

typedef struct bb{
    uint32_t start_included;
    uint32_t end_excluded;
    uint32_t acc_sgpr;
}bb;

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

