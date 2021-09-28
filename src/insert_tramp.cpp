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
    

    uint32_t buffer_size = text_end - insert_location;
	void * file_buffer = malloc(sizeof(char *) * (buffer_size));
    printf("inserting instructions to address %x\n",insert_location);
	fseek(fp,insert_location,SEEK_SET);
	fread(file_buffer, 1 , buffer_size , fp );
	fseek(fp,insert_location,SEEK_SET);
	uint32_t size_acc = 0;
	for(auto & insn : insns){
		insn.write(fp);
		size_acc+= insn.size;

	}
	fwrite(file_buffer,1,buffer_size,fp);
	update_symtab_symbols(fp, text_start ,text_end, insert_location, size_acc);
    update_branches(fp,branches,insert_location,size_acc);
	text_end += size_acc;


}

void inplace_insert_no_update(FILE * fp , const uint32_t text_start , uint32_t & text_end , vector<MyInsn> & insns, vector<MyBranchInsn> & branches, uint32_t insert_location, vector<char *> &insn_pool){    

    uint32_t buffer_size = text_end - insert_location;
	void * file_buffer = malloc(sizeof(char *) * (buffer_size));

	fseek(fp,insert_location,SEEK_SET);
	fread(file_buffer, 1 , buffer_size , fp );
	fseek(fp,insert_location,SEEK_SET);
	uint32_t size_acc = 0;
	for(auto & insn : insns){
		insn.write(fp);
		size_acc+= insn.size;

	}
	fwrite(file_buffer,1, buffer_size ,fp);
	//printf("before update symtab sytmbols\n");
	text_end += size_acc;


}



typedef struct {
	Address branch_address;
	Address target_address;
    char cmd[64];
    uint32_t length;
} CFG_EDGE;

void analyze_binary(char * binaryPath, uint32_t & ret_text_start , uint32_t & ret_text_end, vector<CFG_EDGE> & ret_edges , 
                    uint32_t & after_waitcnt,uint32_t & before_endpgm, vector<std::pair<uint32_t,uint32_t>>  & save_mask_insns ){
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

				if(edges->type() == COND_TAKEN || edges->type() == DIRECT || edges->type() == CALL ){
					printf("DEALING WITH DRIRECT BRANCH\n");
					cout << instr.format() << endl;
					cout << "branching from " << branch_addr <<  " to " << std::hex<<edges->trg()->start() << endl;
                  
					CFG_EDGE cfg_edge;
					cfg_edge.branch_address = branch_addr;
					cfg_edge.target_address = edges->trg()->start();
                    memset(cfg_edge.cmd,0,sizeof(cfg_edge.cmd));
                    memcpy( cfg_edge.cmd, (const char *) insn_ptr, instr.size()  );
                    
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

    printf("Last Address = %lx\n", lastAddr);
    before_endpgm = lastAddr;
    after_waitcnt = 0;
    while(baseAddr + offset <= lastAddr){
        instr = decoder.decode((unsigned char * ) f->isrc()->getPtrToInstruction(baseAddr + offset));
        if(instr.format().find("s_waitcnt") != string::npos && after_waitcnt ==0 ){
            printf("Found s_waitcnt at %lx\n",baseAddr + offset );
            after_waitcnt = baseAddr + offset;
        }else if(instr.format().find("s_and_saveexec") != string::npos){
            //std::cout <<" instr = " << instr.format() << std::endl;
            auto pos = instr.format().find("SGPR");
            uint32_t sgpr;
            sscanf(instr.format().substr(pos+4).c_str() , "%d",&sgpr );
            printf("s and save exec address = %lx , sgpr %d\n",baseAddr + offset + instr.size()  ,sgpr);
            save_mask_insns.push_back(make_pair(baseAddr+offset + instr.size() , sgpr));
            //auto pos = instr.format().find("s_and_saveexecs[");
            //auto pos2 =instr.format().find(":");
            //std::cout << "Testing !!!!!! : " << instr.format().substr(pos,pos2-pos) << std::endl;
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
#define S_6 134
#define S_8 136
#define S_16 144
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
#define SGPR6 6
#define SGPR8 8
#define SGPR9 9


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
void setup_initailization(vector<MyInsn> & init_insns,sreg warp_sgpr_pair,sreg s_backup_writeback_addr , uint32_t writeback_param_offset  ,const AMDGPU_KERNEL_METADATA & metadata ,const uint32_t grid_dim , const uint32_t num_warps_per_block ,const uint32_t num_branches ,vreg ds_addr , vreg ds_data_0, vreg ds_data_1 ,vreg v_minus_1, sreg s_local_warp_id ,vector<char *> & insn_pool){

        /**
         * To calculate the local warp id, we need to calculate threadId_Y * Threads_PER_block_X / 64
         * Here we reuse warp_sgpr_pair to do so
         */
        // First step load from dispatch ptr size of block 
        init_insns.push_back(InsnFactory::create_s_load_dword(warp_sgpr_pair,metadata.dispatch_ptr,4,insn_pool)); 

        init_insns.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
        // Format is block_size_y || block_size_x, so bit operation to remove upper half
        init_insns.push_back(InsnFactory::create_s_and_b32(warp_sgpr_pair,0xffff,warp_sgpr_pair,true,insn_pool)); 
        // Here we read thread_id_y 
        init_insns.push_back(InsnFactory::create_v_readfirstlane_b32(s_local_warp_id,256+VGPR1,insn_pool)); 
        // Thread_ID_Y * THREADS_PER_BLOCK_X
        init_insns.push_back(InsnFactory::create_s_mul_i32(s_local_warp_id, s_local_warp_id, warp_sgpr_pair ,insn_pool)); // TMP = WG_ID * GRID_DIM_X
        // Divide by 64
        init_insns.push_back(InsnFactory::create_s_lshr_b32(s_local_warp_id,S_6, s_local_warp_id ,insn_pool)); 


        /**
         * We want to calculate a global wavefront id ( for global write back) and a local wavefront id ( for ds cache writes)
         * The first step is to calculate the block id WG_ID_Y * GRID_DIM_X (# blocks along x axis)+ WG_ID_X
         * S[TMP  ] = WGID_Y * GRID_DIM_X  TODO: (assuming we know GRID_DIM_X )
         * S[TMP  ] += WGID_X
         * S[TMP+1] = 0
         * TODO: Assume working in two dimension
         */ 
        //if(metadata.work_group_id_z != -1)
        //    init_insns.push_back(InsnFactory::create_s_mul_i32(warp_sgpr_pair,metadata.work_group_id_z ,grid_dim+128,insn_pool)); // TMP = WG_ID * GRID_DIM_X
        //else if(metadata.work_group_id_y != -1)
        //    init_insns.push_back(InsnFactory::create_s_mul_i32(warp_sgpr_pair,metadata.work_group_id_y ,grid_dim+128,insn_pool)); // TMP = WG_ID * GRID_DIM_X
        //init_insns.push_back(InsnFactory::create_s_lshl_b32(warp_sgpr_pair,S_16, metadata.work_group_id_y ,insn_pool)); // TMP= WG_ID_Y << 4 (assume we know GRID_DIM_X ) 
        //
        //
        init_insns.push_back(InsnFactory::create_s_mul_i32(warp_sgpr_pair,metadata.work_group_id_y ,grid_dim+128,insn_pool)); // TMP = WG_ID * GRID_DIM_X
        init_insns.push_back(InsnFactory::create_s_add_u32(warp_sgpr_pair,warp_sgpr_pair, metadata.work_group_id_x,false,insn_pool)); // SGPR_10 += WG_ID_X (SGPR8)

        
        /**
         * A global wavefront Id = work_group_id * num_wavefronts_per_work_group + local_wavefront_id
         */

        init_insns.push_back(InsnFactory::create_s_mul_i32(warp_sgpr_pair,warp_sgpr_pair ,num_warps_per_block+128,insn_pool)); 
        //init_insns.push_back(InsnFactory::create_s_lshl_b32(warp_sgpr_pair,S_2, warp_sgpr_pair ,insn_pool)); // TODO: This is assuming 4 warps per block? 
        init_insns.push_back(InsnFactory::create_s_add_u32(warp_sgpr_pair,warp_sgpr_pair,s_local_warp_id,false,insn_pool));
        init_insns.push_back(InsnFactory::create_s_addc_u32(warp_sgpr_pair+1,warp_sgpr_pair+1,S_0,false,insn_pool));

        init_insns.push_back(InsnFactory::create_s_mov_b32(M0,S_MINUS_1,false,insn_pool)); // Initialize M0 to -1 to access shared memory

        init_insns.push_back(InsnFactory::create_s_load_dwordx2(s_backup_writeback_addr,metadata.kernarg_segment_ptr,writeback_param_offset,insn_pool)); 
        // Backup Writeback Address 


        /**
         * Here we initialize shared memory with 0, and vregs with 0 except one holding -1 for the purpose of ds_inc
         * 
         *
         */  
 
        init_insns.push_back(InsnFactory::create_v_mov_b32(ds_addr , S_0, insn_pool)); 
        init_insns.push_back(InsnFactory::create_v_mov_b32(ds_data_0 , S_0, insn_pool)); 
        init_insns.push_back(InsnFactory::create_v_mov_b32(ds_data_1 , S_0, insn_pool)); 
        init_insns.push_back(InsnFactory::create_v_mov_b32(v_minus_1 , S_MINUS_1, insn_pool)); 
        for ( uint32_t i =0 ; i < num_branches; i++){
            init_insns.push_back(InsnFactory::create_ds_write_b64(ds_addr,ds_data_0,insn_pool));   // ds[0:1]=0
            init_insns.push_back(InsnFactory::create_v_add_u32(ds_addr,ds_addr,S_8,insn_pool)); 
        }
    
        init_insns.push_back(InsnFactory::create_s_mov_b32(M0,S_MINUS_1,false,insn_pool)); // M0 = -1

        //init_insns.push_back(InsnFactory::create_s_mov_b64(s_backup_writeback_addr,s_writeback_addr,insn_pool)); 
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

void per_branch_instrumentation(vector<MyInsn> & instr_insns, sreg s_warp_id ,sreg s_addr_cal, uint32_t branch_id , vreg v_lds_addr, vreg v_lds_data , vreg v_const_minus_1 ,sreg s_backup_exec, sreg s_backup_execcond , uint32_t num_branches,sreg s_local_warp_id , vector<char *> & insn_pool){


        instr_insns.push_back(InsnFactory::create_s_mov_b64(s_backup_exec,EXEC,insn_pool)); // backup EXEC
        instr_insns.push_back(InsnFactory::create_s_mov_b64(EXEC,S_1,insn_pool)); // EXEC = 1 
        /**
         * Where do we write the branch statistics ? 
         * DS[ #branch_id ][0] = #branch_id * 8 
         * How do we deal with the case of non-64 block size ???? 
         *
         */
        instr_insns.push_back(InsnFactory::create_s_mul_i32(s_addr_cal,s_local_warp_id, num_branches +128  ,insn_pool)); // FOR NOW, #NUM_BRANCH = 1, EZ
        instr_insns.push_back(InsnFactory::create_s_add_u32(s_addr_cal,s_addr_cal,branch_id+128,false,insn_pool));

        instr_insns.push_back(InsnFactory::create_s_lshl_b32(s_addr_cal,S_3, s_addr_cal  ,insn_pool)); // LEFT SHIFT By 3 = times 8 

        instr_insns.push_back(InsnFactory::create_v_mov_b32(v_lds_addr, s_addr_cal, insn_pool)); //  vgpr stores the addr

        instr_insns.push_back(InsnFactory::create_s_cmp_eq_u64(s_backup_exec,s_backup_execcond,insn_pool)); // CHECK IF backuped_exec == exec && cond
        instr_insns.push_back(InsnFactory::create_v_mov_b32(v_lds_data , SCC, insn_pool)); // vgpr stores the value (SCC)

        instr_insns.push_back(InsnFactory::create_ds_add_u32(v_lds_addr,v_lds_data,insn_pool)); // increment the counter
        
        instr_insns.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
        instr_insns.push_back(InsnFactory::create_s_cmp_eq_u64(s_backup_exec,S_0,insn_pool)); // CHECK IF S[16:17] == 0
        instr_insns.push_back(InsnFactory::create_v_mov_b32(v_lds_data , SCC, insn_pool)); // v3 stores the value (SCC)
        instr_insns.push_back(InsnFactory::create_ds_add_u32(v_lds_addr,v_lds_data,insn_pool));

        instr_insns.push_back(InsnFactory::create_s_wait_cnt(insn_pool));

        instr_insns.push_back(InsnFactory::create_v_add_u32(v_lds_addr,v_lds_addr,S_4,insn_pool)); 
        instr_insns.push_back(InsnFactory::create_ds_inc_u32(v_lds_addr,v_const_minus_1,insn_pool));
        //instr_insns.push_back(InsnFactory::create_ds_write_b32(v_lds_addr,v_const_minus_1,insn_pool));   // ds[0:1]=0
        //instr_insns.push_back(InsnFactory::create_ds_inc_u32(v_lds_addr,v_const_minus_1,insn_pool))
        
        instr_insns.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
        instr_insns.push_back(InsnFactory::create_s_mov_b64(EXEC,s_backup_exec,insn_pool)); // Use SGPR[16:17] to backup EXEC

}




void setup_writeback(vector<MyInsn> & writeback_insns,sreg s_backup_writeback_addr ,sreg s_addr_cal, sreg s_warp_id , uint32_t num_branches , vreg v_lds_addr, vreg v_lds_data , vreg v_global_addr  ,uint32_t writeback_offset, sreg s_writeback_offset  ,vector<char *> & insn_pool){


    // TODO: Setup a register for storing the offset
    //
    //
    writeback_insns.push_back(InsnFactory::create_s_mov_b32(s_writeback_offset, writeback_offset, true, insn_pool));
    writeback_insns.push_back(InsnFactory::create_s_mov_b64(EXEC,S_1,insn_pool)); // EXEC = 1 
    writeback_insns.push_back(InsnFactory::create_s_mul_i32(s_warp_id,s_warp_id,num_branches+128,insn_pool)); // FOR NOW, #NUM_BRANCH = 1, EZ
    writeback_insns.push_back(InsnFactory::create_s_lshl_b32(s_warp_id,S_3, s_warp_id ,insn_pool)); // LEFT SHIFT By 3 = times 8 
    writeback_insns.push_back(InsnFactory::create_s_add_u32(s_warp_id,s_warp_id, s_backup_writeback_addr , false ,insn_pool));
    writeback_insns.push_back(InsnFactory::create_s_addc_u32(s_warp_id+1,S_0, s_backup_writeback_addr+1 , false ,insn_pool));  
    

    // Replace the add with 
    writeback_insns.push_back(InsnFactory::create_s_add_u32(s_warp_id,s_warp_id, s_writeback_offset , false ,insn_pool));
    writeback_insns.push_back(InsnFactory::create_s_addc_u32(s_warp_id+1,S_0, s_warp_id+1 , false ,insn_pool));  

    writeback_insns.push_back(InsnFactory::create_v_mov_b32(v_global_addr , s_warp_id, insn_pool)); // v[0] = s[0]
    writeback_insns.push_back(InsnFactory::create_v_mov_b32(v_global_addr+1 , s_warp_id+1, insn_pool)); // v[1 ]= s[1] 



    // TODO: Replace S_0 with LDS_OFFSET
    writeback_insns.push_back(InsnFactory::create_v_mov_b32(v_lds_addr , S_0, insn_pool)); 


}

void per_branch_writeback(vector<MyInsn> & writeback_insns,sreg s_backup_writeback_addr ,sreg s_addr_cal, sreg s_warp_id , uint32_t num_branches , vreg v_lds_addr, vreg v_lds_data , vreg v_global_addr  , vector<char *> & insn_pool){
    puts("Per Branch WriteBack !");
        /*writeback_insns.push_back(InsnFactory::create_s_mov_b64(EXEC,S_1,insn_pool)); // EXEC = 1 

        writeback_insns.push_back(InsnFactory::create_s_mul_i32(s_warp_id,s_warp_id,num_branches+128,insn_pool)); // FOR NOW, #NUM_BRANCH = 1, EZ
        writeback_insns.push_back(InsnFactory::create_s_lshl_b32(s_warp_id,S_3, s_warp_id ,insn_pool)); // LEFT SHIFT By 3 = times 8 
        writeback_insns.push_back(InsnFactory::create_s_add_u32(s_warp_id,s_warp_id, s_backup_writeback_addr , false ,insn_pool));
        
        writeback_insns.push_back(InsnFactory::create_s_addc_u32(s_warp_id+1,S_0, s_backup_writeback_addr+1 , false ,insn_pool));  

        writeback_insns.push_back(insnfactory::create_v_mov_b32(v_global_addr , s_warp_id, insn_pool)); // v[0] = s[0]
        writeback_insns.push_back(insnfactory::create_v_mov_b32(v_global_addr+1 , s_warp_id+1, insn_pool)); // v[1 ]= s[1] 

        writeback_insns.push_back(InsnFactory::create_v_mov_b32(v_lds_addr , S_0, insn_pool)); */
        writeback_insns.push_back(InsnFactory::create_ds_read_b64(v_lds_addr,v_lds_data,insn_pool)); // from shared[0] read 64 bits = 2 32_bits, into v[2:3]
        writeback_insns.push_back(InsnFactory::create_s_wait_cnt(insn_pool));
        writeback_insns.push_back(InsnFactory::create_global_store_dword_x2(v_lds_data,v_global_addr,0,insn_pool)); // store v[2:3] in to address poitned by v[0:1]
        // TODO: ADD 8 to both v_lds_addr and v_global_addr
        //
        writeback_insns.push_back(InsnFactory::create_v_add_u32(v_lds_addr,v_lds_addr,S_8,insn_pool)); 

        writeback_insns.push_back(InsnFactory::create_v_add_co_u32(v_global_addr,v_global_addr,S_8,insn_pool)); 
        writeback_insns.push_back(InsnFactory::create_v_addc_co_u32(v_global_addr+1,v_global_addr+1,S_0,insn_pool)); 


}



int main(int argc, char **argv){


	vector < char *> insn_pool;
	if(argc < 8){
		printf("Usage: %s <binary path> sgpr_max vgpr_max grid_dim_x num_warps_per_block writeback_param_offset writeback_offset \n", argv[0]);
		return -1;
	}
    int sgpr_max = atoi(argv[2]);
    int vgpr_max = atoi(argv[3]);
    uint32_t grid_dim_x=  atoi(argv[4]);  
    uint32_t num_warps_per_block = atoi(argv[5]);
    uint32_t writeback_param_offset = atoi(argv[6]);
    uint32_t writeback_offset = atoi(argv[7]);

	char *binaryPath = argv[1];
	FILE* fp = fopen(binaryPath,"rb+");
	uint32_t text_start, text_end;
	vector<CFG_EDGE> edges;
    vector<pair<uint64_t, string>> kds;
    vector<pair<uint32_t, uint32_t >> save_mask_insns;

    get_kds(fp,kds);
    auto metadata = AMDGPU_KERNEL_METADATA(fp,kds[0].first);


    uint32_t after_waitcnt, before_endpgm;
	analyze_binary(binaryPath, text_start , text_end, edges, after_waitcnt, before_endpgm, save_mask_insns); 
    puts("DONE ANALYZING");
    vector<MyBranchInsn> branches;
    for (auto & edge : edges ) {
        branches.push_back(
            InsnFactory::convert_to_branch_insn(edge.branch_address, edge.target_address, edge.cmd, edge.length, insn_pool)
        );
    }
	printf("text_start at %x , text_end at %x \n",text_start, text_end );

    if ( (sgpr_max %2) == 0){
        sgpr_max +=1;
    }
    uint32_t warp_sgpr_pair = sgpr_max + 1;
    uint32_t addr_sgpr_pair = warp_sgpr_pair + 2;
    uint32_t backup_exec_sgpr_pair = addr_sgpr_pair +2;
    uint32_t s_backup_writeback_addr = backup_exec_sgpr_pair+2;
    uint32_t s_writeback_offset = s_backup_writeback_addr+2;
    uint32_t s_local_warp_id = s_writeback_offset+1;

    vreg ds_addr = vgpr_max + 1;
    vreg ds_data_0 = ds_addr + 1;
    vreg ds_data_1  = ds_data_0 + 1;
    vreg v_minus_1 = ds_data_1 + 1;


    uint32_t target_shift = 0;
    uint32_t num_branches = save_mask_insns.size();


    {
        vector<MyInsn> init_insns;
        setup_initailization(init_insns, warp_sgpr_pair,s_backup_writeback_addr,writeback_param_offset, metadata,grid_dim_x , num_warps_per_block ,num_branches,ds_addr,ds_data_0,ds_data_1,v_minus_1,s_local_warp_id,insn_pool);
        inplace_insert(fp,text_start,text_end,init_insns,branches, after_waitcnt + target_shift,insn_pool);
        target_shift += get_size(init_insns);
    }

    
    uint32_t branch_id =0 ;
    for( const auto & pair_addr_sgpr : save_mask_insns){
        auto addr = pair_addr_sgpr.first;
        auto sgpr = pair_addr_sgpr.second;
        vector<MyInsn> update_branch_statistic;
        per_branch_instrumentation(update_branch_statistic, warp_sgpr_pair , addr_sgpr_pair , branch_id  , ds_addr , ds_data_0, v_minus_1 , backup_exec_sgpr_pair, sgpr , num_branches ,s_local_warp_id ,insn_pool);
        inplace_insert(fp,text_start,text_end,update_branch_statistic,branches, addr + target_shift,insn_pool);
        target_shift+= get_size(update_branch_statistic);
        branch_id++;
    }

    /*{
        vector<MyInsn> update_branch_statistic;

        per_branch_instrumentation(update_branch_statistic, warp_sgpr_pair , addr_sgpr_pair , 0 , ds_addr , ds_data_0, v_minus_1 , backup_exec_sgpr_pair, exec_cond_backup_reg ,insn_pool);
        inplace_insert(fp,text_start,text_end,update_branch_statistic,branches,0x1048 + target_shift,insn_pool);
        // 0x1048
        target_shift+= get_size(update_branch_statistic);
    }*/

    {
        vector<MyInsn> writeback_insns;

        setup_writeback(writeback_insns, s_backup_writeback_addr , 6 , warp_sgpr_pair , num_branches , ds_addr , ds_data_0 , VGPR0 , writeback_offset , s_writeback_offset ,insn_pool);
        for ( uint32_t i =0 ; i < num_branches ; i++)
            per_branch_writeback(writeback_insns, s_backup_writeback_addr , 6 , warp_sgpr_pair , num_branches , ds_addr , ds_data_0 , VGPR0  ,insn_pool);
        inplace_insert_no_update(fp,text_start,text_end,writeback_insns,branches, before_endpgm+ target_shift,insn_pool);
    }
    // 0x10b0

    extend_text(fp,text_end-text_start);
    set_lds_usage(fp,kds[0].first,1024/* this is capped at 0x7f for now */);
    update_lds_usage(fp,0x7f);
    set_sgpr_vgpr_usage(fp,kds[0].first,sgpr_max+24,vgpr_max + 4);
    string kernel_name = kds[0].second.substr(0,kds[0].second.find(".kd"));
    update_symtab_symbol_size(fp,kernel_name.c_str(),text_end-text_start);

	for (auto &p : insn_pool){
		free(p);    
	}



	fclose(fp);
	return 0;

}





