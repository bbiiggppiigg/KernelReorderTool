#ifndef INSN_FACTORY_H
#define INSN_FACTORY_H
#include <vector>
#include <iostream>
#include <cstring>
using namespace std;

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

class InsnFactory {
    
public:
// SOP1
    static MyInsn create_s_mov_b32( uint32_t sdst, uint32_t ssrc0, vector<char *> & insn_pool ){
        uint32_t cmd = 0xbe800000;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 4 );
        uint32_t op = 0;
        cmd  = ( cmd | (sdst<< 16) | (op << 8) |ssrc0);
        memcpy( cmd_ptr ,&cmd,  4 );
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,4,std::string("s_mov_b64 "));
    }


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
 static MyInsn create_s_getreg_b32(  uint32_t target_sgpr, uint8_t size, uint8_t offset, uint8_t hwRegId , vector<char *> & insn_pool ){
        uint32_t cmd = 0xb0000000;
        uint32_t op = 17;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 4 );
        size -=1;
        cmd = ( cmd | (op << 23) |  (target_sgpr << 16 ) |(size << 11) | ( offset << 6)  | hwRegId );
        memcpy( cmd_ptr ,&cmd,  4 );
        insn_pool.push_back(cmd_ptr);
        printf("creating instruction s_getreg_b32 = %x\n",cmd);
        return MyInsn(cmd_ptr,4,std::string("s_getreg ")+std::to_string(hwRegId));
    }

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

// VOP2
   static MyInsn create_v_add_u32(  uint32_t vdst, uint32_t vsrc1, uint32_t src0, vector<char *> & insn_pool ){
        uint32_t cmd = 0x0;
        uint32_t op = 52;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 4 );
        cmd = ( cmd | (op << 25) | ( vdst << 17) | (vsrc1 << 9)  | src0 );
        memcpy( cmd_ptr ,&cmd,  4 );
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,4,std::string("s_and_b32 "));
    }
// VOP3AB
   static MyInsn create_v_mul_lo_u32(  uint32_t vdst, uint32_t src1, uint32_t src0, vector<char *> & insn_pool ){

        uint32_t cmd_low = 0xd0000000;
        uint32_t cmd_high = 0x0;
        uint32_t op = 645;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 8 );
        cmd_low = ( cmd_low | (op << 16)   | vdst );
        cmd_high = ( cmd_high | (src1 << 9)  | src0 );
        memcpy( cmd_ptr ,&cmd_low,  4 );
        memcpy( cmd_ptr+4 ,&cmd_high,  4 );
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,8,std::string("s_and_b32 "));
    }


// SOP2
   static MyInsn create_s_lshr_b32(  uint32_t sdst, uint32_t ssrc1, uint32_t ssrc0, vector<char *> & insn_pool ){
        uint32_t cmd = 0x80000000;
        uint32_t op = 30;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 4 );
        cmd = ( cmd | (op << 23) | ( sdst << 16) | (ssrc1 << 8)  | ssrc0 );
        memcpy( cmd_ptr ,&cmd,  4 );
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,4,std::string("s_and_b32 "));
    }
   static MyInsn create_s_lshl_b32(  uint32_t sdst, uint32_t ssrc1, uint32_t ssrc0, vector<char *> & insn_pool ){
        uint32_t cmd = 0x80000000;
        uint32_t op = 28;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 4 );
        cmd = ( cmd | (op << 23) | ( sdst << 16) | (ssrc1 << 8)  | ssrc0 );
        memcpy( cmd_ptr ,&cmd,  4 );
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,4,std::string("s_and_b32 "));
    }

  static MyInsn create_s_mul_i32(  uint32_t sdst, uint32_t ssrc1, uint32_t ssrc0, vector<char *> & insn_pool ){
        uint32_t cmd = 0x80000000;
        uint32_t op = 36;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 4 );
        cmd = ( cmd | (op << 23) | ( sdst << 16) | (ssrc1 << 8)  | ssrc0 );
        memcpy( cmd_ptr ,&cmd,  4 );
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,4,std::string("s_and_b32 "));
    }


   static MyInsn create_s_and_b32(  uint32_t sdst, uint32_t ssrc1, uint32_t ssrc0, vector<char *> & insn_pool ){
        uint32_t cmd = 0x80000000;
        uint32_t op = 12;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 4 );
        cmd = ( cmd | (op << 23) | ( sdst << 16) | (ssrc1 << 8)  | ssrc0 );
        memcpy( cmd_ptr ,&cmd,  4 );
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,4,std::string("s_and_b32 "));
    }
   static MyInsn create_s_and_b32(  uint32_t sdst, uint32_t ssrc1, uint32_t ssrc0, uint32_t simm32 ,vector<char *> & insn_pool ){
        uint32_t cmd = 0x80000000;
        uint32_t op = 12;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 8 );
        cmd = ( cmd | (op << 23) | ( sdst << 16) | (ssrc1 << 8)  | ssrc0 );
        printf("cmd_ptr = %llx\n", *(unsigned long long int * ) cmd_ptr+4);
        memcpy( cmd_ptr ,&cmd,  4 );
        printf("cmd_ptr = %llx\n", *(unsigned long long int * ) cmd_ptr+4);
        memcpy( cmd_ptr+4 ,&simm32,  4 );
        printf("cmd_ptr = %llx\n", *(unsigned long long int * ) cmd_ptr+4);
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,8,std::string("s_and_b32 "));
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
        return MyInsn(cmd_ptr,8,std::string("flat_store_dword_x2 "));
    }
      static MyInsn create_flat_store_dword( uint32_t s_data_pair, uint32_t s_base_pair  ,  uint32_t offset,vector<char *> & insn_pool ){
        uint32_t cmd_low = 0xdc000000;
        uint32_t cmd_high = 0x0;
        uint32_t op = 28;
        char * cmd_ptr = (char *   ) malloc(sizeof(char) * 8 );
        cmd_low = (cmd_low | (op<< 18 )  | offset );

        cmd_high = ( cmd_high | s_data_pair << 8 | s_base_pair );
        memcpy( cmd_ptr ,&cmd_low,  4 );
        memcpy( cmd_ptr +4 ,&cmd_high,  4 );
        insn_pool.push_back(cmd_ptr);
        return MyInsn(cmd_ptr,8,std::string("flat_store_dword "));
    }

};

#endif
