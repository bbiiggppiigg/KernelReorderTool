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

void analyze_cfg(char * binaryPath, uint32_t & ret_text_start , uint32_t & ret_text_end, vector<CFG_EDGE> & ret_edges ){
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
}

uint32_t get_size(vector<MyInsn> &insns){
    uint32_t ret = 0;
    for (auto & insn : insns){
        ret += insn.size;
    }
    return ret;
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



	analyze_cfg(binaryPath, text_start , text_end, edges); 
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
    {
        vector<MyInsn> compute_warp_id;
        //compute_warp_id.push_back(InsnFactory::create_s_mov_b32(12,9,insn_pool)); // SGPR_13 = WG_ID_Y
        compute_warp_id.push_back(InsnFactory::create_s_mov_b32(124,193,insn_pool)); // EXEC_LOW = 1
        compute_warp_id.push_back(InsnFactory::create_s_lshl_b32(warp_sgpr_pair,132,9,insn_pool)); // SGPR_12 = WG_ID_Y << 4 (assume we know GRID_DIM_X ) 
        compute_warp_id.push_back(InsnFactory::create_s_add_u32(warp_sgpr_pair,warp_sgpr_pair,8,false,insn_pool)); // SGPR_11 += WG_ID_X
        compute_warp_id.push_back(InsnFactory::create_s_movk_i32(warp_sgpr_pair+1,0,insn_pool)); // SGPR_13 = 0 

        compute_warp_id.push_back(InsnFactory::create_v_mov_b32(2 , 128, insn_pool)); // v2 = 0
        compute_warp_id.push_back(InsnFactory::create_v_mov_b32(3 , 128, insn_pool)); // v3 = 0
        compute_warp_id.push_back(InsnFactory::create_v_mov_b32(4 , 128, insn_pool)); // v4 = 0
        
        //compute_warp_id.push_back(InsnFactory::create_ds_write_b64(4,2,insn_pool));   // ds[0:1]=0


        inplace_insert(fp,text_start,text_end,compute_warp_id,branches,0x1000 + target_shift,insn_pool);
        target_shift += get_size(compute_warp_id);
    }
    {
        vector<MyInsn> update_branch_statistic;

        //update_branch_statistic.push_back(InsnFactory::create_s_cmp_eq_u64(106,126,insn_pool)); // 106 is VCC_LOW , 126 is EXEC_LOW
        //update_branch_statistic.push_back(InsnFactory::create_s_mov_b64(106,126,insn_pool)); // 106 is VCC_LOW , 126 is EXEC_LOW

        update_branch_statistic.push_back(InsnFactory::create_s_mov_b64(backup_exec_sgpr_pair,126,insn_pool)); // Use SGPR[16:17] to backup EXEC
        update_branch_statistic.push_back(InsnFactory::create_s_mov_b64(126,129,insn_pool)); // EXEC = 1 
        update_branch_statistic.push_back(InsnFactory::create_s_mul_i32(addr_sgpr_pair,warp_sgpr_pair,129,insn_pool)); // FOR NOW, #NUM_BRANCH = 1, EZ

        //update_branch_statistic.push_back(InsnFactory::create_s_add_u32(12,12,128,false,insn_pool));
        update_branch_statistic.push_back(InsnFactory::create_s_lshl_b32(addr_sgpr_pair,131,addr_sgpr_pair,insn_pool)); // LEFT SHIFT By 3 = times 8 

        update_branch_statistic.push_back(InsnFactory::create_v_mov_b32(4, addr_sgpr_pair, insn_pool)); //  v4 stores the addr

        update_branch_statistic.push_back(InsnFactory::create_s_cmp_eq_u64(backup_exec_sgpr_pair,exec_cond_backup_reg,insn_pool)); // CHECK IF S[16:17] == S[0:1]

        update_branch_statistic.push_back(InsnFactory::create_v_mov_b32(3 , 253, insn_pool)); // v3 stores the value (SCC)

        update_branch_statistic.push_back(InsnFactory::create_ds_add_u32(4,3,insn_pool));
        
        update_branch_statistic.push_back(InsnFactory::create_s_cmp_eq_u64(backup_exec_sgpr_pair,128,insn_pool)); // CHECK IF S[16:17] == 0

        update_branch_statistic.push_back(InsnFactory::create_v_mov_b32(3 , 253, insn_pool)); // v3 stores the value (SCC)
        
        
        
        //update_branch_statistic.push_back(InsnFactory::create_v_mov_b32(9 , 136, insn_pool)); // v3 stores the value (SCC) * NEW
        //update_branch_statistic.push_back(InsnFactory::create_v_mov_b32(8 , 136, insn_pool)); // v3 stores the value (SCC) * NEW

        //update_branch_statistic.push_back(InsnFactory::create_v_mov_b32(3 , 128, insn_pool)); // v3 stores the value (SCC) * NEW

        
        //update_branch_statistic.push_back(InsnFactory::create_v_add_u32(3,3,132,insn_pool)); 
        //
        update_branch_statistic.push_back(InsnFactory::create_ds_write_b64(4,2,insn_pool));   // ds[0:1]=0
        //update_branch_statistic.push_back(InsnFactory::create_ds_add_u32(4,3,insn_pool)); 

        // TODO : DS_INC 
        
        

        update_branch_statistic.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
        update_branch_statistic.push_back(InsnFactory::create_s_mov_b64(126,16,insn_pool)); // Use SGPR[16:17] to backup EXEC
        inplace_insert(fp,text_start,text_end,update_branch_statistic,branches,0x1048 + target_shift,insn_pool);

        // 0x1048
        target_shift+= get_size(update_branch_statistic);
    }

    {
        vector<MyInsn> qq;
        qq.push_back(InsnFactory::create_s_mov_b64(126,129,insn_pool)); // EXEC = 1 
        qq.push_back(InsnFactory::create_v_mov_b32(0 , 0, insn_pool)); // v[0] = s[0]
        qq.push_back(InsnFactory::create_v_mov_b32(1 , 1, insn_pool)); // v[1 ]= s[1]
        qq.push_back(InsnFactory::create_v_mov_b32(4 , 128, insn_pool)); // v4 = 0

        qq.push_back(InsnFactory::create_ds_read_b64(4,2,insn_pool)); // from shared[0] read 64 bits = 2 32_bits, into v[2:3]
        qq.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
        qq.push_back(InsnFactory::create_global_store_dword_x2(2,0,0,insn_pool)); // store v[2:3] in to address poitned by v[0:1]
        inplace_insert_no_update(fp,text_start,text_end,qq,branches,0x10b0 + target_shift,insn_pool);
    }
    // 0x10b0

    get_kds(fp,kds);
    extend_text(fp,text_end-text_start);
    set_lds_usage(fp,kds[0].first,1024);
    set_sgpr_vgpr_usage(fp,kds[0].first,17,9);
	/*to_insert.push_back(InsnFactory::create_s_mov_b32(10,0,insn_pool));
	inplace_insert(fp,text_start,text_end,to_insert,branches,0x1088,insn_pool);
	inplace_insert(fp,text_start,text_end,to_insert,branches,0x1088,insn_pool);
	inplace_insert(fp,text_start,text_end,to_insert,branches,0x1088,insn_pool);*/

    uint32_t GRIM_DIM_X = 16;


	for (auto &p : insn_pool){
		free(p);    
	}



	fclose(fp);
	return 0;

}





