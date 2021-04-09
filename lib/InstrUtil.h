#ifndef INSTR_UTIL_H
#define INSTR_UTIL_H
#include <vector>
#include <iostream>
#include "InsnFactory.h"
using namespace std;
class MyInsn;

uint32_t insert_tramp( FILE * f, vector<MyInsn> & prologues, vector<MyInsn> & epilogues,uint32_t start_included, uint32_t end_excluded, uint32_t tramp_location, uint32_t scc_store_index ,  vector<char *> & insn_pool);

void set_memtime_pro_ep(vector<MyInsn> & pro, vector<MyInsn> &epi , uint32_t first_record_sgpr, uint32_t second_record_sgpr, uint32_t accum_sgpr,vector<char *> & insn_pool);


void set_counter_pro_ep(vector<MyInsn> & pro, vector<MyInsn> &epi ,  uint32_t accum_sgpr,vector<char *> & insn_pool);

void set_writeback_pro_ep(vector<MyInsn> & pro, vector<MyInsn> &epi , uint32_t addr_sgpr,uint32_t addr_vgpr, uint32_t acc_sgpr, uint32_t acc_vgpr,vector<char *> & insn_pool);

typedef struct bb {
    uint32_t start_included;
    uint32_t end_excluded;
    uint32_t acc_sgpr;
}bb;
#endif
