#ifndef INSTR_UTIL_H
#define INSTR_UTIL_H
#include <vector>
#include <iostream>
#include "InsnFactory.h"
using namespace std;
class MyInsn;

/**
 * @brief Create a trampoline by branching to \p tramp_location, with \p proglogues and \p epilogues surrounding a copy of the instructions between \p start_included and \p end_excluded
 */


uint32_t insert_tramp( FILE * f, vector<MyInsn> & prologues, vector<MyInsn> & epilogues,uint32_t start_included, uint32_t end_excluded, uint32_t tramp_location, uint32_t scc_store_index ,  vector<char *> & insn_pool);


/**
 * @brief Store the required prologue and epilogue into \p pro and \p epi for caculating accumulated time spent 
 */

void set_memtime_pro_ep(vector<MyInsn> & pro, vector<MyInsn> &epi , uint32_t first_record_sgpr, uint32_t second_record_sgpr, uint32_t accum_sgpr,vector<char *> & insn_pool);


/**
 * @brief Store the required prologue and epilogue into \p pro and \p epi for incrementing counter value
 */

void set_counter_pro_ep(vector<MyInsn> & pro, vector<MyInsn> &epi ,  uint32_t accum_sgpr,vector<char *> & insn_pool);

/**
 * @brief Store the required prologue and epilogue into \p pro and \p epi for getting hardware reg value
 */


void set_getreg_pro_ep(vector<MyInsn> & pro, vector<MyInsn> &epi ,  uint32_t accum_sgpr, uint32_t size , uint32_t offset, uint32_t hwRegId, vector<char *> & insn_pool);

/**
 * @brief Store the required prologue and epilogue into \p pro and \p epi for writing register values into memory so it can be copied back to host 
 *
 */


void set_writeback_pro_ep(vector<MyInsn> & pro, vector<MyInsn> &epi , uint32_t addr_sgpr,uint32_t addr_vgpr, uint32_t acc_sgpr, uint32_t acc_vgpr,vector<char *> & insn_pool);

/**
 * @brief simple structure for specifying basic block and where per block results should be stored
 */
typedef struct bb {
    uint32_t start_included;
    uint32_t end_excluded;
    uint32_t acc_sgpr;
}bb;


/**
 * @brief Inplace movement of a code block of range [\p start_included , \p end_excluded) to start at \p target_addr
 */
void move_block_to(FILE * f, uint32_t start_included, uint32_t end_excluded, uint32_t target_addr);

#endif
