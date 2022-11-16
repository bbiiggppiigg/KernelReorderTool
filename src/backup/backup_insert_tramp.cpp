#include <elf.h>
#include <vector>
#include <iostream>
#include "CodeObject.h"
#include "InstructionDecoder.h"
#include "CFG.h"
#include "InsnFactory.h"
#include "kernel_elf_helper.h"

using namespace std;
using namespace Dyninst;
using namespace ParseAPI;
using namespace InstructionAPI;




typedef struct symtab_symbol{
	uint32_t index;
	uint32_t st_value;
	uint32_t st_size;
}symtab_symbol;




void update_symtab_symbols(FILE * f, uint32_t text_start , uint32_t text_end , uint32_t insert_loc , uint32_t insert_size);



void update_branches(FILE* fp , vector<MyBranchInsn> &branches, uint32_t insert_loc , uint32_t insert_size){
    for ( auto & branch : branches ){
        branch.update_for_insertion(insert_loc,insert_size);
    }

    for ( auto & branch : branches ){
        branch.write_to_file(fp);
    }
}

// Inplace Insertion of Code Snippet
// This should be easier 
// Keep track of a global structure of edges, we only need to modify the edges that go across the specified insert location
//

void inplace_insert(FILE * fp , const uint32_t text_start , uint32_t & text_end , vector<MyInsn> & insns, vector<MyBranchInsn> & branches, uint32_t insert_location, vector<char *> &insn_pool){    

	void * file_buffer = malloc(sizeof(char *) * (text_end - insert_location));

	fseek(fp,insert_location,SEEK_SET);
	fread(file_buffer, 1 , text_end - insert_location , fp );
	fseek(fp,insert_location,SEEK_SET);
	uint32_t size_acc = 0;
	for(auto & insn : insns){
		insn.write(fp);
		size_acc+= insn.size;

	}
	fwrite(file_buffer,1,text_end,fp);
	//printf("before update symtab sytmbols\n");
	update_symtab_symbols(fp, text_start ,text_end, insert_location, size_acc);
    update_branches(fp,branches,insert_location,size_acc);
	text_end += size_acc;


}

void inplace_insert_no_update(FILE * fp , const uint32_t text_start , uint32_t & text_end , vector<MyInsn> & insns, vector<MyBranchInsn> & branches, uint32_t insert_location, vector<char *> &insn_pool){    

	void * file_buffer = malloc(sizeof(char *) * (text_end - insert_location));

	fseek(fp,insert_location,SEEK_SET);
	fread(file_buffer, 1 , text_end - insert_location , fp );
	fseek(fp,insert_location,SEEK_SET);
	uint32_t size_acc = 0;
	for(auto & insn : insns){
		insn.write(fp);
		size_acc+= insn.size;

	}
	fwrite(file_buffer,1,text_end,fp);
	//printf("before update symtab sytmbols\n");
	text_end += size_acc;


}



typedef struct {
	Address branch_address;
	Address target_address;
    char cmd[64];
    uint32_t length;
} CFG_EDGE;

void analyze_binary(char * binaryPath, uint32_t & ret_text_start , uint32_t & ret_text_end, vector<CFG_EDGE> & ret_edges , uint32_t & after_waitcnt,uint32_t & before_endpgm){
	map<Address,bool> seen;
	vector<Function *> funcs;
	SymtabCodeSource * sts;
	CodeObject * co;
	sts = new SymtabCodeSource(binaryPath);
	co = new CodeObject( sts);
	co -> parse();
	uint32_t text_start= -1;
	uint32_t text_end = 0;
	const CodeObject::funclist & all = co->funcs();
	auto fit = all.begin();
	for (int i =0 ; fit != all.end(); fit++ , i++){
		Function * f = * fit;
		InstructionDecoder decoder(f->isrc()->getPtrToInstruction(f->addr()),InstructionDecoder::maxInstructionLength,f->region()->getArch());

		auto bit = f->blocks().begin();
		for (; bit != f->blocks().end(); ++ bit){
			Block * b = * bit;
			if(b->start() < text_start)
				text_start = b->start();
			if(b->end() > text_end)
				text_end = b->end();
			if(seen.find(b->start())!=seen.end())
				continue;
			seen[b->start()] = true;
			for (auto & edges : b->targets()){

				Address branch_addr = edges->src()->lastInsnAddr();
                void * insn_ptr = f->isrc()->getPtrToInstruction(branch_addr);
				Instruction instr = decoder.decode((unsigned char *) insn_ptr);

				if(edges->type() == COND_TAKEN){
					printf("DEALING WITH DRIRECT BRANCH");
					cout << instr.format() << endl;
					cout << "branching to " << std::hex<<edges->trg()->start() << endl;
					CFG_EDGE cfg_edge;
					cfg_edge.branch_address = branch_addr;
					cfg_edge.target_address = edges->trg()->start();
                    memset(cfg_edge.cmd,0,sizeof(cfg_edge.cmd));
                    memcpy( cfg_edge.cmd, (const char *) insn_ptr, instr.size()  );
                    //printf("%x\n", * (uint32_t *) cfg_edge.cmd);
                    
                    cout << std::hex << cfg_edge.cmd << endl;
					cfg_edge.length = instr.size();
					ret_edges.push_back(cfg_edge);
				}
			}

		}

	}
	ret_text_start = text_start;
	ret_text_end = text_end;

	fit = all.begin();
	Function * f = * fit;
	InstructionDecoder decoder(f->isrc()->getPtrToInstruction(f->addr()),InstructionDecoder::maxInstructionLength,f->region()->getArch());
    uint32_t offset = 0 ;
    Instruction instr;
    Address baseAddr = f->addr();
    auto fbl = f->blocks().end();
    fbl--;
    Block * b = *fbl;
    Address lastAddr = b->last();

    printf("Last Address = %x\n", lastAddr);
    before_endpgm = lastAddr;
    while(baseAddr + offset <= lastAddr){
        instr = decoder.decode((unsigned char * ) f->isrc()->getPtrToInstruction(baseAddr + offset));
        if(instr.format().find("s_waitcnt") != string::npos){
            printf("Found s_waitcnt at %x\n",baseAddr + offset );
            after_waitcnt = baseAddr + offset + instr.size();    
            return;
        }
        offset += instr.size();
    }
    
}

uint32_t get_size(vector<MyInsn> &insns){
    uint32_t ret = 0;
    for (auto & insn : insns){
        ret += insn.size;
    }
    return ret;
}


#define M0 124
#define EXEC 126
#define EXEC_LOW 126
#define EXEC_HI 127
#define S_0 128
#define S_1 129
#define S_2 130
#define S_3 131
#define S_4 132
#define S_8 136
#define S_MINUS_1 193
#define SCC 253


#define VGPR0 0
#define VGPR1 1
#define VGPR2 2
#define VGPR3 3
#define VGPR4 4
#define VGPR5 5
#define VGPR6 6
#define VGPR7 7
#define VGPR8 8
#define VGPR9 9

#define SGPR0 0
#define SGPR9 9
#define SGPR8 8


/**
 * Ininitailization of Instrumentation
 * 1. Backup useful information ( WG_ID_Y / WG_ID_X )
 * 2. Compute Per Warp ID Base Adress
 * 3. Initialize Vector Registers that are later used
 * TODO: WE DON'T KNOW GRIM_DIM_X
 *
 */

typedef uint16_t reg;
typedef uint16_t sreg;
typedef uint16_t vreg;
void setup_initailization(vector<MyInsn> & init_insns,sreg warp_sgpr_pair,sreg s_backup_writeback_addr , sreg s_writeback_addr , vector<char *> & insn_pool){


        init_insns.push_back(InsnFactory::create_s_mov_b32(M0,S_MINUS_1,insn_pool)); // M0 = -1
        init_insns.push_back(InsnFactory::create_s_lshl_b32(warp_sgpr_pair,S_4,SGPR9,insn_pool)); // SGPR_12 = WG_ID_Y << 4 (assume we know GRID_DIM_X ) 
        init_insns.push_back(InsnFactory::create_s_add_u32(warp_sgpr_pair,warp_sgpr_pair,SGPR8,false,insn_pool)); // SGPR_10 += WG_ID_X (SGPR8)
        init_insns.push_back(InsnFactory::create_s_movk_i32(warp_sgpr_pair+1,S_0,insn_pool)); // SGPR_11 = 0 

        init_insns.push_back(InsnFactory::create_v_mov_b32(VGPR2 , S_0, insn_pool)); // v2 = 0
        init_insns.push_back(InsnFactory::create_v_mov_b32(VGPR3 , S_0, insn_pool)); // v3 = 0
        init_insns.push_back(InsnFactory::create_v_mov_b32(VGPR4 , S_0, insn_pool)); // v4 = 0
        init_insns.push_back(InsnFactory::create_v_mov_b32(VGPR5 , S_0, insn_pool)); // v5 = 0

        init_insns.push_back(InsnFactory::create_ds_write_b64(VGPR4,VGPR4,insn_pool));   // ds[0:1]=0
        init_insns.push_back(InsnFactory::create_ds_write_b64(VGPR4,VGPR4,insn_pool));   // ds[0:1]=0
     
        init_insns.push_back(InsnFactory::create_s_mov_b32(M0,S_MINUS_1,insn_pool)); // M0 = -1
        init_insns.push_back(InsnFactory::create_v_mov_b32(VGPR5,S_MINUS_1,insn_pool)); // v5 = -1

        init_insns.push_back(InsnFactory::create_s_mov_b32(s_backup_writeback_addr,s_writeback_addr,insn_pool)); // M0 = -1
        init_insns.push_back(InsnFactory::create_s_wait_cnt(insn_pool));

}

/**
 * This function insert insturmentation at after the s_and_saveexec_b64 instruction to record branch statistics
 * Determined by Instrumentation
     * s_addr_cal : Scalar register preserved for address calculation
     * v_lds_addr : Vector register used for storing lds addr
     * v_lds_data : Vector register used for storing lds data
     * s_backup_exec : Scalar register used to backup exec mask 
 * Determined by binary
    * s_backup_execcond : The register that in s_and_saveexec_b64
 */ 

void per_branch_instrumentation(vector<MyInsn> & instr_insns, sreg s_warp_id ,sreg s_addr_cal, vreg v_lds_addr, vreg v_lds_data , vreg v_const_minus_1 ,sreg s_backup_exec, sreg s_backup_execcond , vector<char *> & insn_pool){


        instr_insns.push_back(InsnFactory::create_s_mov_b64(s_backup_exec,EXEC,insn_pool)); // Use SGPR[16:17] to backup EXEC
        instr_insns.push_back(InsnFactory::create_s_mov_b64(EXEC,S_1,insn_pool)); // EXEC = 1 
        instr_insns.push_back(InsnFactory::create_s_mul_i32(s_addr_cal,s_warp_id,S_1,insn_pool)); // FOR NOW, #NUM_BRANCH = 1, EZ

        instr_insns.push_back(InsnFactory::create_s_lshl_b32(s_addr_cal,S_3, s_addr_cal ,insn_pool)); // LEFT SHIFT By 3 = times 8 

        instr_insns.push_back(InsnFactory::create_v_mov_b32(v_lds_addr, s_addr_cal, insn_pool)); //  v4 stores the addr

        instr_insns.push_back(InsnFactory::create_s_cmp_eq_u64(s_backup_exec,s_backup_execcond,insn_pool)); // CHECK IF S[16:17] == S[0:1]
        instr_insns.push_back(InsnFactory::create_v_mov_b32(v_lds_data , SCC, insn_pool)); // v3 stores the value (SCC)

        instr_insns.push_back(InsnFactory::create_ds_add_u32(v_lds_addr,v_lds_data,insn_pool));
        
        instr_insns.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
        instr_insns.push_back(InsnFactory::create_s_cmp_eq_u64(s_backup_exec,S_0,insn_pool)); // CHECK IF S[16:17] == 0
        instr_insns.push_back(InsnFactory::create_v_mov_b32(v_lds_data , SCC, insn_pool)); // v3 stores the value (SCC)
        instr_insns.push_back(InsnFactory::create_ds_add_u32(v_lds_addr,v_lds_data,insn_pool));

        instr_insns.push_back(InsnFactory::create_s_wait_cnt(insn_pool));

        instr_insns.push_back(InsnFactory::create_v_add_u32(v_lds_addr,v_lds_addr,S_4,insn_pool)); 
        instr_insns.push_back(InsnFactory::create_ds_inc_u32(v_lds_addr,v_const_minus_1,insn_pool));
        
        instr_insns.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
        instr_insns.push_back(InsnFactory::create_s_mov_b64(EXEC,s_backup_exec,insn_pool)); // Use SGPR[16:17] to backup EXEC

}


void per_branch_writeback(vector<MyInsn> & writeback_insns,sreg s_addr_cal, vreg v_lds_addr, vreg v_lds_data , vreg v_global_addr  , vector<char *> & insn_pool){
        writeback_insns.push_back(InsnFactory::create_s_mov_b64(EXEC,S_1,insn_pool)); // EXEC = 1 
        writeback_insns.push_back(InsnFactory::create_v_mov_b32(v_global_addr , s_addr_cal, insn_pool)); // v[0] = s[0]
        writeback_insns.push_back(InsnFactory::create_v_mov_b32(v_global_addr+1 , s_addr_cal, insn_pool)); // v[1 ]= s[1]
        writeback_insns.push_back(InsnFactory::create_v_mov_b32(v_lds_addr , S_0, insn_pool)); // v4 = 0

        writeback_insns.push_back(InsnFactory::create_ds_read_b64(v_lds_addr,v_lds_data,insn_pool)); // from shared[0] read 64 bits = 2 32_bits, into v[2:3]
        writeback_insns.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
        writeback_insns.push_back(InsnFactory::create_global_store_dword_x2(v_lds_data,v_global_addr,0,insn_pool)); // store v[2:3] in to address poitned by v[0:1]

}



int main(int argc, char **argv){


	vector < char *> insn_pool;
	if(argc != 5){
		printf("Usage: %s <binary path> <start_included> <end_excluded> <tramp_location> \n", argv[0]);
		return -1;
	}

	char *binaryPath = argv[1];
	FILE* fp = fopen(binaryPath,"rb+");
	uint32_t text_start, text_end;
	vector<CFG_EDGE> edges;
    vector<pair<uint64_t, string>> kds;


    uint32_t after_waitcnt, before_endpgm;
	analyze_binary(binaryPath, text_start , text_end, edges, after_waitcnt, before_endpgm); 
    vector<MyBranchInsn> branches;
    for (auto & edge : edges ) {
        branches.push_back(
            InsnFactory::convert_to_branch_insn(edge.branch_address, edge.target_address, edge.cmd, edge.length, insn_pool)
        );
    }
	printf("text_start at %x , text_end at %x \n",text_start, text_end );

    uint32_t exec_cond_backup_reg = 0;

    uint32_t warp_sgpr_pair = 10;
    uint32_t addr_sgpr_pair = 12;
    uint32_t backup_exec_sgpr_pair =14;
    uint32_t target_shift = 0;
    uint32_t s_writeback_addr = 0 ;
    uint32_t s_backup_writeback_addr = 16;
    {
        vector<MyInsn> compute_warp_id;
        setup_initailization(compute_warp_id, warp_sgpr_pair,s_backup_writeback_addr,s_writeback_addr, insn_pool);
        inplace_insert(fp,text_start,text_end,compute_warp_id,branches,0x1000 + target_shift,insn_pool);
        target_shift += get_size(compute_warp_id);
    }
    {
        vector<MyInsn> update_branch_statistic;

        per_branch_instrumentation(update_branch_statistic, warp_sgpr_pair , addr_sgpr_pair, VGPR4, VGPR3, VGPR5 , backup_exec_sgpr_pair, exec_cond_backup_reg ,insn_pool);
        inplace_insert(fp,text_start,text_end,update_branch_statistic,branches,0x1048 + target_shift,insn_pool);
        // 0x1048
        target_shift+= get_size(update_branch_statistic);
    }

    {
        vector<MyInsn> writeback_insns;

        per_branch_writeback(writeback_insns, s_backup_writeback_addr, VGPR4 , VGPR2 , VGPR0  ,insn_pool);
        inplace_insert_no_update(fp,text_start,text_end,writeback_insns,branches,0x10b0 + target_shift,insn_pool);
    }
    // 0x10b0

    get_kds(fp,kds);
    extend_text(fp,text_end-text_start);
    set_lds_usage(fp,kds[0].first,1024/* this is capped at 0x7f for noew */);
    update_lds_usage(fp,0x7f);
    set_sgpr_vgpr_usage(fp,kds[0].first,17,10);
    string kernel_name = kds[0].second.substr(0,kds[0].second.find(".kd"));
    update_symtab_symbol_size(fp,kernel_name.c_str(),text_end-text_start);
    uint32_t GRIM_DIM_X = 16;

	for (auto &p : insn_pool){
		free(p);    
	}



	fclose(fp);
	return 0;

}





