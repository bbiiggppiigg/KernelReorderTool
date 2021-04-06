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

#define WAIT_CONST 14


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

class MyWaitCntInsn : public MyInsn {
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
};


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


void test_accumulation(FILE * f, vector<char *> &insn_pool){
    // 1. Backup Address ( Prologue) 
    // 2. 
    //      a. Insert MemTime at start
    //      b. 
    //            i. Insert MemTime at end
    //            ii. Peforme Subtraction
    // 3. Write Back Resulti (Epilogue)
    uint32_t avail_addr = 0;
    vector<MyInsn> pro_1, ep_1;

    ep_1.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    ep_1.push_back(InsnFactory::create_s_mov_b64(16,0,insn_pool));
    ep_1.push_back(InsnFactory::create_s_mov_b64(22,128,insn_pool));
    avail_addr = insert_tramp(f,pro_1,ep_1,4096,4120,5388,24,insn_pool);

    vector<MyInsn> pro_2, ep_2;
    /*pro_2.push_back(InsnFactory::create_s_memtime(18,insn_pool));
    ep_2.push_back(InsnFactory::create_s_memtime(20,insn_pool));
    ep_2.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    ep_2.push_back(InsnFactory::create_s_sub_u32(18,20,18,false,insn_pool));
    ep_2.push_back(InsnFactory::create_s_subb_u32(19,21,19,false,insn_pool));
    ep_2.push_back(InsnFactory::create_s_add_u32(22,18,22,false,insn_pool));
    ep_2.push_back(InsnFactory::create_s_addc_u32(23,19,23,false,insn_pool));*/

    ep_2.push_back(InsnFactory::create_s_add_u32(22,22,1,true,insn_pool));
    avail_addr = insert_tramp(f,pro_2,ep_2,0x10f4,0x13d0,avail_addr,24,insn_pool);

    
    vector<MyInsn> pro_3, ep_3;

    //ep_3.push_back(InsnFactory::create_s_memtime(22,insn_pool));
    //ep_3.push_back(InsnFactory::create_s_wait_cnt(insn_pool));


    ep_3.push_back(InsnFactory::create_s_add_u32( 16,16 ,0x4010  ,true, insn_pool));
    ep_3.push_back(InsnFactory::create_s_addc_u32( 17, 17  , 128 , false , insn_pool));

    ep_3.push_back(InsnFactory::create_v_mov_b32( 6,16 , insn_pool));
    ep_3.push_back(InsnFactory::create_v_mov_b32( 7,17 , insn_pool));

    ep_3.push_back(InsnFactory::create_v_mov_b32( 8,22 , insn_pool));
    ep_3.push_back(InsnFactory::create_v_mov_b32( 9,23 , insn_pool));
    ep_3.push_back(InsnFactory::create_flat_store_dword_x2(8,6,0x0,insn_pool ));

    avail_addr = insert_tramp(f,pro_3,ep_3,0x1500,0x1508,avail_addr,24,insn_pool);




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

/*
    uint32_t start_included, end_excluded , tramp_location;

    start_included = atoi(argv[2]);
    end_excluded = atoi(argv[3]);
    tramp_location = atoi(argv[4]);
*/
    test_accumulation(fp,insn_pool);
    fclose(fp);

    for (auto &p : insn_pool){
        free(p);    
    }
   
    /*map<Address,bool> seen;
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
            if(seen.find(b->start()) != seen.end())
                continue;
            seen[b->start()] = true;

            auto edges = b->targets().begin();
            for ( ; edges != b->targets().end(); ++ edges){
                if(!*edges) continue;
            }
        
        }

    }
    std::cout <<"number of basic blocks " << seen.size() << std::endl;
    */

    return 0;

}





/*
    insns.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    insns.push_back(InsnFactory::create_s_add_u32( 10,0 ,0x4010  ,true, insn_pool));
    insns.push_back(InsnFactory::create_s_addc_u32( 11, 1  , 128 , false , insn_pool));
    insns.push_back(InsnFactory::create_v_mov_b32( 19,11 , insn_pool));
    insns.push_back(InsnFactory::create_v_mov_b32( 18,10 , insn_pool));
    insns.push_back(InsnFactory::create_s_memtime(16,insn_pool));
    insns.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    insns.push_back(InsnFactory::create_v_mov_b32( 21,17 , insn_pool));
    insns.push_back(InsnFactory::create_v_mov_b32( 20,16 , insn_pool));
    insns.push_back(InsnFactory::create_flat_store_dword_x2(20,18,0x0,insn_pool ));

    insns.push_back(InsnFactory::create_s_add_u32( 10,0 ,0x4010 + 8 ,true, insn_pool));
    insns.push_back(InsnFactory::create_s_addc_u32( 11, 1  , 128 , false , insn_pool));
    insns.push_back(InsnFactory::create_v_mov_b32( 19,11 , insn_pool));
    insns.push_back(InsnFactory::create_v_mov_b32( 18,10 , insn_pool));
    insns.push_back(InsnFactory::create_s_memtime(18,insn_pool));
    insns.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
    insns.push_back(InsnFactory::create_v_mov_b32( 21,19 , insn_pool));
    insns.push_back(InsnFactory::create_v_mov_b32( 20,18 , insn_pool));
    insns.push_back(InsnFactory::create_flat_store_dword_x2(20,18,0x0,insn_pool ));

    insns.push_back(InsnFactory::create_s_add_u32( 10,0 ,0x4010 + 16 ,true, insn_pool));
    insns.push_back(InsnFactory::create_s_addc_u32( 11, 1  , 128 , false , insn_pool));
    insns.push_back(InsnFactory::create_v_mov_b32( 19,11 , insn_pool));
    insns.push_back(InsnFactory::create_v_mov_b32( 18,10 , insn_pool));


    insns.push_back(InsnFactory::create_s_sub_u32( 16 , 18, 16 , false, insn_pool));
    insns.push_back(InsnFactory::create_s_subb_u32( 17, 19  , 17 , false , insn_pool));
    insns.push_back(InsnFactory::create_v_mov_b32( 21,17 , insn_pool));
    insns.push_back(InsnFactory::create_v_mov_b32( 20,16 , insn_pool));
    insns.push_back(InsnFactory::create_flat_store_dword_x2(20,18,0x0,insn_pool ));
*/
