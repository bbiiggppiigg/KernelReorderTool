#ifndef __KERNEL_ELF_HELPER__
#define __KDERNEL_ELF_HELPER__
#include <elf.h>
#include <vector>
#include <iostream>
#include <cstring>
#include <cassert>
using namespace std;
void extend_text(FILE * f, int new_text_size);
void set_sgpr_usage( FILE* fp , vector<pair <uint64_t,string>> & kds ,string name, uint32_t new_sgpr_count );
void update_symtab_symbols(FILE * f, uint32_t text_start , uint32_t text_end , uint32_t insert_loc , uint32_t insert_size);
void get_kds(FILE * f, vector<pair<uint64_t, string>> & ret);
uint32_t get_bits(uint32_t value, uint32_t hi , uint32_t low);
uint32_t set_bits(uint32_t old_bits, uint32_t hi, uint32_t low, uint32_t bits);



// The behavior of the following two function depends on the Architecture, right now is written for GFX9 only
void set_lds_usage(FILE* fp, uint32_t kd_offset , uint32_t new_lds_usage );
// ONLY WORKS FOR GFX9 NOW
void set_sgpr_vgpr_usage(FILE * fp , uint32_t kd_offset , uint32_t sgpr_usage ,uint32_t vgpr_usage );



typedef struct {
   uint32_t offset;
   uint32_t data;
} metadata_u32;

uint32_t sgpr_bits_to_count(uint32_t bits);
uint32_t vgpr_bits_to_count(uint32_t bits);


class Others {
    public:
    uint8_t enable_sgpr_private_segment_buffer;
    uint8_t enable_sgpr_dispatch_ptr;
    uint8_t enable_sgpr_queue_ptr;
    uint8_t enable_sgpr_kernarg_segment_ptr;
    uint8_t enable_sgpr_dispatch_id;
    uint8_t enable_sgpr_flat_scratch_init;
    uint8_t enable_sgpr_private_segment_size;
    uint32_t raw_value;
    const uint32_t pgm_others_offset = 0x38;
    Others(FILE * fp , uint32_t kd_offset ){
        fseek(fp,kd_offset + pgm_others_offset , SEEK_SET);
        fread(&raw_value,4,1,fp);


        enable_sgpr_private_segment_buffer = get_bits(raw_value,0,0);
        enable_sgpr_dispatch_ptr = get_bits(raw_value,1,1);
        enable_sgpr_queue_ptr = get_bits(raw_value,2,2);
        enable_sgpr_kernarg_segment_ptr = get_bits(raw_value,3,3);
        enable_sgpr_dispatch_id = get_bits(raw_value,4,4);
        enable_sgpr_flat_scratch_init = get_bits(raw_value,5,5);
        enable_sgpr_private_segment_size = get_bits(raw_value,6,6);
        
        //printf("private_seg_buff = %u\ndispatch_ptr = %u\nqueue_ptr = %u\nkerarg_ptr = %u\ndispatch_id = %u\nscratch_init = %u\nprivate_seg_size = %u\n",enable_sgpr_private_segment_buffer,enable_sgpr_dispatch_ptr,enable_sgpr_queue_ptr,enable_sgpr_kernarg_segment_ptr,enable_sgpr_dispatch_id,enable_sgpr_flat_scratch_init,enable_sgpr_private_segment_size);
    }  
};

#define PRINT(X) printf(#X "= %u\n", X );

class COMPUTE_PGM_RSRC2 {
  public:
    uint8_t enable_private_segment;
    uint8_t user_sgpr_count;
    uint8_t enable_trap_handler;
    uint8_t enable_sgpr_workgroup_id_x;
    uint8_t enable_sgpr_workgroup_id_y;
    uint8_t enable_sgpr_workgroup_id_z;
    
    const uint8_t pgm_rsrc2_offset = 0x34;
    uint32_t raw_value;
    COMPUTE_PGM_RSRC2(FILE * fp , uint32_t kd_offset){
        fseek(fp,kd_offset + pgm_rsrc2_offset , SEEK_SET);
        fread(&raw_value,4,1,fp);

        enable_private_segment = get_bits(raw_value,0,0);
        user_sgpr_count = get_bits(raw_value,5,1);
        enable_trap_handler = get_bits(raw_value,6,6);
        enable_sgpr_workgroup_id_x = get_bits(raw_value,7,7);
        enable_sgpr_workgroup_id_y = get_bits(raw_value,8,8);
        enable_sgpr_workgroup_id_z = get_bits(raw_value,9,9);
        //PRINT(enable_private_segment);
        //PRINT(user_sgpr_count);
        //PRINT(enable_trap_handler);
        //PRINT(enable_sgpr_workgroup_id_x);
        //PRINT(enable_sgpr_workgroup_id_y);
        //PRINT(enable_sgpr_workgroup_id_z);
    }

    COMPUTE_PGM_RSRC2(uint32_t value){
        enable_private_segment = get_bits(value,0,0);
        user_sgpr_count = get_bits(value,5,1);
        enable_trap_handler = get_bits(value,6,6);
        enable_sgpr_workgroup_id_x = get_bits(value,7,7);
        enable_sgpr_workgroup_id_y = get_bits(value,8,8);
        enable_sgpr_workgroup_id_z = get_bits(value,9,9);
        //PRINT(enable_private_segment);
        //PRINT(user_sgpr_count);
        //PRINT(enable_trap_handler);
        PRINT(enable_sgpr_workgroup_id_x);
        PRINT(enable_sgpr_workgroup_id_y);
        PRINT(enable_sgpr_workgroup_id_z);
    }
    
};

class COMPUTE_PGM_RSRC1 {

    public:
    uint8_t granulated_workitem_vgpr_count;
    uint8_t granulated_wavefront_sgpr_count;
    uint8_t priority;
    uint8_t flat_round_mode_32;
    uint8_t float_round_mode_16_64;
    uint8_t float_denorm_mode_32;
    uint8_t float_denorm_mode_16_64;
    uint8_t priv;
    uint8_t enable_dx10_clamp;
    uint8_t debug_mode;
    uint8_t enable_ieee_mode;
    uint8_t bulky;
    uint8_t cdbg_user;
    uint8_t f16_ovfl;
    uint8_t wgp_mode;
    uint8_t mem_ordered;
    uint8_t fwd_progress;

    uint32_t raw_value;

    const uint8_t pgm_rsrc1_offset = 0x30;
    COMPUTE_PGM_RSRC1(FILE * fp, uint32_t kd_offset ){
        fseek(fp,kd_offset + pgm_rsrc1_offset , SEEK_SET);
        fread(&raw_value,4,1,fp);
        granulated_workitem_vgpr_count = get_bits(raw_value,5,0);
        granulated_wavefront_sgpr_count = get_bits(raw_value,9,6);
        /*printf("vgpr_count = %u, sgpr_count = %u\n",
            vgpr_bits_to_count(
                granulated_workitem_vgpr_count
            ),
            sgpr_bits_to_count(granulated_wavefront_sgpr_count));*/
    }
};

class AMDGPU_KERNEL_METADATA{
    public:
        const uint8_t pgm_rsrc2_offset = 0x34;
        const uint8_t pgm_others_offset = 0x38;
        COMPUTE_PGM_RSRC1 * pgm_rsrc1;
        COMPUTE_PGM_RSRC2 * pgm_rsrc2;
        Others * pgm_others;

        int32_t dispatch_ptr;
        int32_t queue_ptr;
        int32_t kernarg_segment_ptr;
        int32_t dispatch_id ;
        int32_t flat_scratch_init;
        int32_t private_segment_size;
        int32_t work_group_id_x;
        int32_t work_group_id_y;
        int32_t work_group_id_z;

        AMDGPU_KERNEL_METADATA(FILE * fp , uint32_t kd_offset){
            uint32_t sgpr_count = 0;
            dispatch_ptr = queue_ptr = kernarg_segment_ptr = dispatch_id = flat_scratch_init = private_segment_size = work_group_id_x = work_group_id_y = work_group_id_z = -1;
            pgm_rsrc1 = new COMPUTE_PGM_RSRC1(fp,kd_offset);
            pgm_rsrc2 = new COMPUTE_PGM_RSRC2(fp,kd_offset);
            pgm_others = new Others(fp,kd_offset);
            if(pgm_others->enable_sgpr_private_segment_buffer){
                sgpr_count += 4;
            }
            if(pgm_others->enable_sgpr_dispatch_ptr){
                dispatch_ptr = sgpr_count;
                sgpr_count += 2;
            }
            if(pgm_others->enable_sgpr_queue_ptr){
                queue_ptr = sgpr_count;
                sgpr_count +=2 ;
            }
            if(pgm_others->enable_sgpr_kernarg_segment_ptr){
                kernarg_segment_ptr = sgpr_count;
                sgpr_count += 2;
            }
            if(pgm_others->enable_sgpr_dispatch_id){
                dispatch_id = sgpr_count;
                sgpr_count += 2;
            }
            if(pgm_others->enable_sgpr_flat_scratch_init){
                flat_scratch_init = sgpr_count;
                sgpr_count += 2;
            }
            if(pgm_others->enable_sgpr_private_segment_size){
                private_segment_size = sgpr_count;
                sgpr_count += 1;
            }
            if(pgm_rsrc2->enable_sgpr_workgroup_id_x){
                work_group_id_x = sgpr_count;
                sgpr_count +=1;
            }
            if(pgm_rsrc2->enable_sgpr_workgroup_id_y){
                work_group_id_y = sgpr_count;
                sgpr_count +=1;
            }
            if(pgm_rsrc2->enable_sgpr_workgroup_id_x){
                work_group_id_z = sgpr_count;
                sgpr_count +=1;
            }
            PRINT(dispatch_ptr);
            PRINT(queue_ptr);
            PRINT(work_group_id_x);
            PRINT(work_group_id_y);
            PRINT(work_group_id_z);
           
        }

    
    
};
#endif
