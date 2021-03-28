#include <elf.h>
#include <vector>
#include <iostream>
#include "CodeObject.h"
#include "InstructionDecoder.h"
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
        printf("creating s_load_dwordx4 sgpr[%d:%d], sgpr[%d:%d], 0x%x:  %x%x\n",sgpr_target_pair,sgpr_addr_pair,offset,cmd_low,cmd_high); 
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
        printf("creating s_load_dwordx4 sgpr[%d:%d], sgpr[%d:%d], 0x%x:  %x%x\n",sgpr_target_pair,sgpr_addr_pair,offset,cmd_low,cmd_high); 
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,8,std::string("s_load_dwordx4 ")+std::to_string(sgpr_target_pair));
    }
    
      


};



void insert_tramp( FILE * f, uint32_t start_included, uint32_t end_excluded, uint32_t tramp_location, vector<char *> & insn_pool){
    void * buffer_block =  malloc(end_excluded-start_included);
    fseek(f,start_included,SEEK_SET);
    fread(buffer_block,1,end_excluded-start_included,f);
    MyInsn to_tramp = InsnFactory::create_s_branch(start_included,tramp_location, insn_pool );
    MyInsn from_tramp = InsnFactory::create_s_branch(tramp_location + end_excluded-start_included, end_excluded, insn_pool);
    fseek(f,start_included,SEEK_SET);
    fwrite(to_tramp.ptr,to_tramp.size,1,f);
    fseek(f,tramp_location,SEEK_SET);
    fwrite(buffer_block,1,end_excluded-start_included,f);
    fwrite(from_tramp.ptr,from_tramp.size,1,f);



    MyInsn memtime = InsnFactory::create_s_store_dword_x4(12,6,0x0, insn_pool);
    MyInsn memtime1 = InsnFactory::create_s_store_dword_x4(0,6,0x10, insn_pool);
    memtime.write(f);
    memtime1.write(f);
    auto movk = InsnFactory::create_s_movk_i32(0,0x4000,insn_pool);
    movk.write(f);
     
}




int main(int argc, char **argv){


    vector < char *> insn_pool;
    if(argc != 5){
        printf("Usage: %s <binary path> <start_included> <end_excluded> <tramp_location> \n", argv[0]);
        return -1;
    }
    
    char *binaryPath = argv[1];
    FILE* fp = fopen(binaryPath,"rb+");
    insert_tramp(fp,atoi(argv[2]),atoi(argv[3]),atoi(argv[4]),insn_pool);
    fclose(fp);

    for (auto &p : insn_pool){
        free(p);    
    }
}




