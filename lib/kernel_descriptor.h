#include <cstdint>
#pragma pack(push, 1)
typedef struct __attribute((__packed__)) kernel_descriptor{
    uint32_t group_segment_fixed_size;
    uint32_t private_segment_fixed_size;
    uint32_t kernarg_size;
    uint32_t reserved_0;
    uint64_t kernel_code_entry_byte_offset;
    char     reserved_1[16];
    struct{
        reserved[4];
        /*uint8_t granulated_workitem_vgpr_count : 6;
        uint8_t granulated_wavefront_sgpr_count : 4;
        uint8_t priority : 2;
        uint8_t float_round_mode32 : 2;
        uint8_t float_round_mode_16_64 : 2;
        uint8_t float_denorm_mode_32 : 2;
        uint8_t float_denorm_mode_16_64 : 2;
        uint8_t priv : 1;
        uint8_t enable_dx_clamp : 1;
        uint8_t debug_mode : 1;
        uint8_t enable_ieee_mode : 1;
        uint8_t bulky : 1;
        uint8_t cdbg_user : 1;
        uint8_t fp16_ovfl : 1;
        uint8_t reserved_0 : 2;
        uint8_t wgp_mode : 1;
        uint8_t mem_ordered : 1;
        uint8_t fwd_progress : 1;*/
        uint32_t tmp;
    }compute_pgm_rsrc3;
    //uint32_t compute_pgm_rsrc3;
    struct{
        uint32_t tmp; 
    }compute_pgm_rsrc1;
    struct{
        uint32_t tmp;
    }compute_pgm_rsrc2;
    uint8_t enable_sgpr_private_segment_buffer: 1;
    uint8_t enable_sgpr_dispatch_ptr : 1;
    uint8_t enable_sgpr_kernarg_sgment_ptr : 1;
    uint8_t enable_sgpr_dispatch_id : 1;
    uint8_t enable_sgpr_flat_scratch_init : 1;
    uint8_t enable_sgpr_private_segment_size ;
    //uint8_t reserved_2: 3;
    uint8_t enable_wavefront_size32: 1;
    uint8_t reserved_3 : 1;
    uint8_t reserved_4 : 1;
    uint8_t reserved_5 : 3;
    uint8_t reserved_6 ;
    //uint8_t reserved_7 : 3;
    uint8_t reserved_8 : 5;
}kernel_descriptor;
#pragma pack(pop)
