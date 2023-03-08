#include <vector>
#include <iostream>
#include "CodeObject.h"
#include "InsnFactory.h"
#include "kernel_elf_helper.h"
#include "../lib/AMDHSAKernelDescriptor.h"
#include "KdUtils.h"
#include "KernelDescriptor.h"
#include <fstream>


using namespace std;
using namespace Dyninst;
using namespace ParseAPI;
using namespace InstructionAPI;
typedef struct {
    uint32_t sgpr_max; // id of highest used scalar register in the binary
    uint32_t vgpr_max; // id of highest used vector register in the binary
    uint32_t old_kernarg_size;

    uint32_t work_group_id_x_enabled;
    uint32_t work_group_id_y_enabled;
    uint32_t work_group_id_z_enabled;

    uint32_t work_item_id_enabled ; // >0 => threadIdy  in VGPR1 , > 1 => threadIdz in VGPR2

    uint32_t dispatch_ptr;
    uint32_t queue_ptr;
    uint32_t dispatch_id;
    uint32_t private_segment_size;
    uint32_t kernarg_segment_ptr = -1;

    uint32_t work_group_id_x;
    uint32_t work_group_id_y;
    uint32_t work_group_id_z;


    // derived fields
    uint32_t wavefront_per_work_group;
    uint32_t num_wg_per_grid_x; // number of work groups per grid,
    uint32_t num_wg_per_grid_y;
    uint32_t num_wg_per_grid_z;

    uint32_t num_branches;


    uint32_t TMP_SGPR0;
    uint32_t TMP_SGPR1;

    uint32_t GLOBAL_WAVEFRONT_ID;
    uint32_t LOCAL_WAVEFRONT_ID;
    uint32_t WORK_GROUP_ID;
    uint32_t BACKUP_WRITEBACK_ADDR;

    uint32_t DEBUG_SGPR0;
    uint32_t DEBUG_SGPR1;


    // VGPRS
    uint32_t DS_ADDR;
    uint32_t DS_DATA_0;
    uint32_t DS_DATA_1;
    uint32_t V_MINUS_1;

    uint32_t TIMER_1;
    uint32_t TIMER_2;
    uint32_t BACKUP_EXEC;

    uint32_t v_global_addr;
    uint32_t kd_addr; // Address of the kernel descriptor
    string name ; // kernel name
    uint32_t code_entry_offset;
    uint32_t func_start;
    uint32_t func_end;
    
    uint32_t first_uninitalized_sgpr;
    // 
    //
} config;


typedef struct kernel_bound{
    uint32_t first; // Address of the first instruction
    uint32_t last; // Address after the last instruction, should always be s_endpgm
    string name; // Name of Kernel
    uint32_t kdAddr; // Address of the kernel descriptor
    KernelDescriptor *kd;
    //if (isKernelDescriptor(symbol)) {
    //KernelDescriptor kd(symbol);
    /*~kernel_bound()
    {
        delete(kd);
    }*/
} kernel_bound;

typedef struct {
    uint32_t branch_address;
    uint32_t target_address;
    char cmd[64];
    uint32_t length;
} CFG_EDGE;

/**
 * Make use of SymtabAPI to collect inforamtion about kernel descriptors / kernels bounds and the last instruction of the file
 */ 
void getKernelBounds(char * binaryPath, vector<kernel_bound> & kernel_bounds, uint32_t & last_instr );

/**
 * Make use of dyninst to anaylyze the kernel binary
 * store the start  / end of text section in @ret_text_start / @ret_text_end
 * store all edges in @ret_edges, to modify the branch targets later
 * 
 *
 */

void analyze_binary(char * binaryPath, vector<CFG_EDGE> & ret_edges , vector<std::pair<uint32_t,uint32_t>>  & save_mask_insns , uint32_t func_start , uint32_t func_end , vector<uint32_t> & endpgms);


uint32_t get_sgpr(config &conf, bool pair = false );


uint32_t get_vgpr(config &conf, bool pair = false );


void read_config(FILE * fp, char * configPath , vector<config> &configs , vector<kernel_bound> &kernel_bounds);
void update_symtab_symbols(FILE * f, uint32_t text_start , uint32_t text_end , uint32_t insert_loc , uint32_t insert_size);



void update_branches(FILE* fp , vector<MyBranchInsn> &branches, uint32_t insert_loc , uint32_t insert_size);

// Inplace Insertion of Code Snippet
// This should be easier 
// Keep track of a global structure of edges, we only need to modify the edges that go across the specified insert location
//
//
// Special Case
// When we insert code to the destination of a branch, what do we want??
// We want the basic block entry to stay there and the branch instruction unmodified
//
//
//
void inplace_insert(FILE * fp , const uint32_t text_start , uint32_t & text_end , vector<MyInsn> & insns, vector<MyBranchInsn> & branches, uint32_t insert_location,  vector<kernel_bound> &kbs ,vector<uint32_t> &endpgms,vector<char *> &insn_pool);
   
void offsetted_inplace_insert(uint32_t offset , FILE * fp , const uint32_t text_start , uint32_t & text_end , vector<MyInsn> & insns, vector<MyBranchInsn> & branches, uint32_t insert_location,  vector<kernel_bound> &kbs ,vector<uint32_t> &endpgms,vector<char *> &insn_pool);
uint32_t get_size(vector<MyInsn> &insns);

void update_kernel_bound(FILE * fp , vector<kernel_bound> & kbs, uint32_t insert_location, uint32_t insert_size);

