#include "CodeObject.h"
#include <elf.h>
#include <vector>
#include <iostream>
#include "CodeObject.h"
#include "InstructionDecoder.h"
#include "CFG.h"
#include "InsnFactory.h"
#include "kernel_elf_helper.h"
#include "INIReader.h"
#include "../lib/AMDHSAKernelDescriptor.h"

#include "Function.h"
using namespace std;
using namespace Dyninst;
using namespace ParseAPI;
using namespace InstructionAPI;
typedef struct {
    uint32_t sgpr_max; // id of highest used scalar register in the binary
    uint32_t vgpr_max; // id of highest used vector register in the binary
    uint32_t warps_per_block; // number of wavefront per thread block
    uint32_t writeback_param_offset; // byte offset to the parameter that stores address to perform writeback
    uint32_t writeback_offset; // offset to the first address after the original address bound, where we store data to be write back
    uint32_t lds_base; // amount of shared memory used in the uninstrumented binary, we will use shared memory space after this address

    uint32_t work_group_dim_x;
    uint32_t work_group_dim_y;
    uint32_t work_group_dim_z;

    uint32_t grid_dim_x; // here this means number of blocks along x -axis
    uint32_t grid_dim_y;
    uint32_t grid_dim_z;

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
    
    // 
    //
} config;

typedef struct {
    uint32_t first; // Address of the first instruction
    uint32_t last; // Address after the last instruction, should always be s_endpgm
    string name; // Name of Kernel
    uint32_t kdAddr; // Address of the kernel descriptor
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
void getKernelBounds(char * binaryPath, vector<kernel_bound> & kernel_bounds, uint32_t & last_instr ){
    SymtabAPI::Symtab * symTab;
    string binaryPathStr(binaryPath);
    printf("before calling openFile\n");
    SymtabAPI::Symtab::openFile(symTab, binaryPathStr);
    vector<SymtabAPI::Module *> modules;

    printf("before calling getAllModules \n");
    symTab->getAllModules(modules);
    last_instr = 0;
    for (const auto & module : modules){
        vector<SymtabAPI::Symbol *> symbols;
        module->getAllSymbols(symbols);
        for(const auto & symbol : symbols){
            if (symbol->getType() == SymtabAPI::Symbol::ST_FUNCTION && symbol->isInSymtab()){
                uint32_t offset = symbol->getOffset();
                uint32_t size = symbol->getSize();
                string name = symbol->getMangledName();
                bool found = false;
                string kd_name = name + ".kd";
                Address kdAddr;
                for (const auto & symbol2 : symbols){
                    string name2 = symbol2->getMangledName();
                    if(name2 == kd_name){
                        found = true;
                        kdAddr = symbol2->getOffset();
                        break;
                    }
                }
                if(!found){
                    printf("failed to find corresponding kernel descriptor");
                    exit(-1);
                }

                kernel_bound kb;
                kb.first = offset;
                kb.last = offset + size;
                kb.name = name;
                kb.kdAddr = kdAddr;
                kernel_bounds.push_back(kb);
                //std::cout <<" symbol name = " << name << " offset = " << offset << " size = " << size << std::endl;

                if(kb.last > last_instr){
                    last_instr = kb.last;
                }
            }
        }
    }
    printf("before closing symtab\n");
    SymtabAPI::Symtab::closeSymtab(symTab);
    printf("after closing symtab\n");
}

/**
 * Make use of dyninst to anaylyze the kernel binary
 * store the start  / end of text section in @ret_text_start / @ret_text_end
 * store all edges in @ret_edges, to modify the branch targets later
 * 
 *
 */

void analyze_binary(char * binaryPath, vector<CFG_EDGE> & ret_edges , vector<std::pair<uint32_t,uint32_t>>  & save_mask_insns , uint32_t func_start , uint32_t func_end ){
    map<Address,bool> seen;
    vector<Function *> funcs;
    SymtabCodeSource * sts;
    CodeObject * co;
    printf("in %s \n",__func__);
    sts = new SymtabCodeSource(binaryPath);
    printf("after sts\n");
    co = new CodeObject( sts);
    printf("after co\n");
    co -> parse();
    printf("after parse\n");




    uint32_t text_start= -1;
    uint32_t text_end = 0;
    const CodeObject::funclist & all = co->funcs();
    auto fit = all.begin();
    for (int i =0 ; fit != all.end(); fit++ , i++){
        Function * f = * fit;
        if(f->addr() != func_start)
            continue;
        printf("Creating a decoder, parsing region addr = %x, len = %lu\n",f->addr(), InstructionDecoder::maxInstructionLength);
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
                    //printf("DEALING WITH DRIRECT BRANCH\n");
                    cout << instr.format() << endl;
                    //cout << "branching from " << branch_addr <<  " to " << std::hex<<edges->trg()->start() << endl;

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

    fit = all.begin();
    Function * f = * fit;
    InstructionDecoder decoder(f->isrc()->getPtrToInstruction(func_start),InstructionDecoder::maxInstructionLength,f->region()->getArch());
    uint32_t offset = 0 ;
    Instruction instr;
    Address baseAddr = func_start;
    Address lastAddr = func_end;

    printf("Start Address = %lx , Last Address = %lx\n", baseAddr, lastAddr);
    while(baseAddr + offset < lastAddr){
        printf("decoding instruction at address %lx\n", baseAddr + offset);
        instr = decoder.decode((unsigned char * ) f->isrc()->getPtrToInstruction(baseAddr + offset));
        printf("instruction is %s\n", instr.format().c_str());
        if(instr.format().find("s_and_saveexec") != string::npos){
            std::vector < InstructionAPI::Operand> operands;
            instr.getOperands(operands);
            for (auto & opr : operands){

                if(opr.isWritten()){
                    auto name = opr.format(Arch_amdgpu_cdna2);
                    auto pos = name.find("%sgpr");
                    uint32_t sgpr;
                    sscanf(name.substr(pos+5).c_str(),"%d",&sgpr);
                    printf("s and save exec address = %lx , sgpr %d\n",baseAddr + offset + instr.size()  ,sgpr);
                    save_mask_insns.push_back(make_pair(baseAddr+offset + instr.size() , sgpr));

                }
            }
        }
        offset += instr.size();
    }
    delete co;
    delete sts;
    printf("return from %s\n" , __func__);
}




void read_config(FILE * fp, char * configPath , vector<config> &configs , vector<kernel_bound> &kernel_bounds){
    INIReader reader(configPath);
    if(reader.ParseError() !=0){
        std::cout << "Can't load " << std::string(configPath)  << std::endl;     

        exit(-1);

    }
    auto &sections = reader.Sections();
    for (auto section : sections){
        config c;
        std::string kernel_name = reader.Get(section,"kernel_name","");
        std::string kd_name = kernel_name+".kd";
        bool found = false;
        uint32_t kd_offset = -1;
       
       for(auto kb : kernel_bounds){
            if(kb.name == kernel_name){
                c.func_start = kb.first;
                c.func_end = kb.last;
                kd_offset = kb.kdAddr;
                c.kd_addr = kd_offset;
                c.name = kernel_name;
                found = true;
            }
        }
        if(!found){
            puts("ERROR! wrong kernel name specified in cofig file");
            exit(-1);
        }
 
        c.sgpr_max = reader.GetInteger(section,"sgpr_max",-1); 
        c.vgpr_max = reader.GetInteger(section,"vgpr_max",-1); 
        c.grid_dim_x = reader.GetInteger(section,"grid_dim_x",-1); 
        c.warps_per_block = reader.GetInteger(section,"warps_per_block",-1); 
        c.writeback_param_offset = reader.GetInteger(section,"writeback_param_offset",-1); 
        c.writeback_offset = reader.GetInteger(section,"writeback_offset",-1); 
        c.lds_base = reader.GetInteger(section,"lds_base",-1);

        c.work_group_dim_x = reader.GetInteger(section,"work_group_dim_x",1);
        c.work_group_dim_y = reader.GetInteger(section,"work_group_dim_y",1);
        c.work_group_dim_z = reader.GetInteger(section,"work_group_dim_z",1);

        c.grid_dim_x = reader.GetInteger(section,"grid_dim_x",1);
        c.grid_dim_y = reader.GetInteger(section,"grid_dim_y",1);
        c.grid_dim_z = reader.GetInteger(section,"grid_dim_z",1);

        llvm::amdhsa::kernel_descriptor_t fkd;
        fseek(fp,kd_offset,SEEK_SET);
        fread(&fkd,sizeof(fkd),1,fp);
        uint32_t code_entry_offset = fkd.kernel_code_entry_byte_offset;
        c.code_entry_offset = code_entry_offset;
        c.wavefront_per_work_group = c.work_group_dim_x  * c.work_group_dim_y * c.work_group_dim_z / 64;



        c.work_group_id_x_enabled = AMDHSA_BITS_GET(fkd.compute_pgm_rsrc2,llvm::amdhsa::COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_ID_X);
        c.work_group_id_y_enabled = AMDHSA_BITS_GET(fkd.compute_pgm_rsrc2,llvm::amdhsa::COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_ID_Y);
        c.work_group_id_z_enabled = AMDHSA_BITS_GET(fkd.compute_pgm_rsrc2,llvm::amdhsa::COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_ID_Z);
        c.work_item_id_enabled = AMDHSA_BITS_GET(fkd.compute_pgm_rsrc2,llvm::amdhsa::COMPUTE_PGM_RSRC2_ENABLE_VGPR_WORKITEM_ID);
        uint32_t sgpr_count=0;

        sgpr_count = AMDHSA_BITS_GET(fkd.kernel_code_properties,llvm::amdhsa::KERNEL_CODE_PROPERTY_ENABLE_SGPR_PRIVATE_SEGMENT_BUFFER) ? 4 : 0;


        if (AMDHSA_BITS_GET(fkd.kernel_code_properties,llvm::amdhsa::KERNEL_CODE_PROPERTY_ENABLE_SGPR_DISPATCH_PTR)){
            c.dispatch_ptr = sgpr_count;
            sgpr_count += 2;
            printf("dispatch ptr = %u\n",c.dispatch_ptr);
        }

        if(AMDHSA_BITS_GET(fkd.kernel_code_properties,llvm::amdhsa::KERNEL_CODE_PROPERTY_ENABLE_SGPR_QUEUE_PTR)){
            c.queue_ptr = sgpr_count;
            sgpr_count +=2 ;

            printf("queue ptr = %u\n",c.queue_ptr);
        }

        if(AMDHSA_BITS_GET(fkd.kernel_code_properties,llvm::amdhsa::KERNEL_CODE_PROPERTY_ENABLE_SGPR_KERNARG_SEGMENT_PTR )){
            c.kernarg_segment_ptr = sgpr_count;
            sgpr_count += 2;

            printf("kerarg seg ptr = %u\n",c.kernarg_segment_ptr);
        }



        if(AMDHSA_BITS_GET(fkd.kernel_code_properties,llvm::amdhsa::KERNEL_CODE_PROPERTY_ENABLE_SGPR_DISPATCH_ID )){
            c.dispatch_id = sgpr_count;
            sgpr_count += 2;

            printf("dispatch id = %u\n",c.dispatch_id);
        }


        if(AMDHSA_BITS_GET(fkd.kernel_code_properties,llvm::amdhsa::KERNEL_CODE_PROPERTY_ENABLE_SGPR_FLAT_SCRATCH_INIT )){
            //c.flat_scratch_init = sgpr_count;
            //
            sgpr_count += 2;
        }

        if(AMDHSA_BITS_GET(fkd.kernel_code_properties,llvm::amdhsa::KERNEL_CODE_PROPERTY_ENABLE_SGPR_PRIVATE_SEGMENT_SIZE )){
            c.private_segment_size = sgpr_count;
            sgpr_count += 1;

            printf("private segment size   = %u\n",c.private_segment_size);
        }

        if(c.work_group_id_x_enabled){
            c.work_group_id_x = sgpr_count;
            sgpr_count +=1;
            printf("workgroup_id_x = %u\n",c.work_group_id_x);
        }

        if(c.work_group_id_y_enabled){
            c.work_group_id_y = sgpr_count;
            sgpr_count +=1;
        }

        if(c.work_group_id_z_enabled){
            c.work_group_id_z = sgpr_count;
            sgpr_count +=1;
        }



        uint32_t sgpr_max = c.sgpr_max;
        if ( (sgpr_max %2) ){
            sgpr_max +=1;
        }

        c.TMP_SGPR0 = sgpr_max +1;
        c.TMP_SGPR1 = c.TMP_SGPR0 +1;
        c.GLOBAL_WAVEFRONT_ID = c.TMP_SGPR1+1;
        c.LOCAL_WAVEFRONT_ID = c.GLOBAL_WAVEFRONT_ID+1;
        c.BACKUP_WRITEBACK_ADDR = c.LOCAL_WAVEFRONT_ID+2;
        c.TIMER_1 = c.BACKUP_WRITEBACK_ADDR+2;
        printf("timer 1 = %u\n",c.TIMER_1);
        c.TIMER_2 = c.TIMER_1+2;
        c.BACKUP_EXEC = c.TIMER_2 + 2;
        c.WORK_GROUP_ID = c.BACKUP_EXEC +1;


        c.DS_ADDR = c.vgpr_max + 1;
        c.DS_DATA_0 = c.DS_ADDR + 1;
        c.DS_DATA_1 = c.DS_DATA_0+1;
        c.V_MINUS_1 = c.DS_DATA_1+1;
        c.v_global_addr = c.V_MINUS_1+2;
        configs.push_back(c);
    }
}

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
//
// Special Case
// When we insert code to the destination of a branch, what do we want??
// We want the basic block entry to stay there and the branch instruction unmodified
//
//
//
void inplace_insert(FILE * fp , const uint32_t text_start , uint32_t & text_end , vector<MyInsn> & insns, vector<MyBranchInsn> & branches, uint32_t insert_location,  vector<kernel_bound> &kbs ,vector<char *> &insn_pool){    


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
    printf("updating kernel bounds\n");
    for(auto & kb : kbs){
        if(insert_location < kb.first){
            kb.first += size_acc;
            kb.last += size_acc;
            fseek(fp,kb.kdAddr+16, SEEK_SET);
            long long unsigned new_offset = kb.first-kb.kdAddr;
            fwrite(&new_offset,1,8,fp );
        }
    }


}



uint32_t get_size(vector<MyInsn> &insns){
    uint32_t ret = 0;
    for (auto & insn : insns){
        ret += insn.size;
    }
    return ret;
}


void update_kernel_bound(FILE * fp , vector<kernel_bound> & kbs, uint32_t insert_location, uint32_t insert_size){
    for(auto & kb : kbs){
        if(insert_location < kb.first){
            kb.first += insert_size;
            kb.last += insert_size;
            fseek(fp,kb.kdAddr+16, SEEK_SET);
            long long unsigned new_offset = kb.first-kb.kdAddr;
            printf("updating kernel descriptor at %x to value %llx\n",kb.kdAddr,new_offset);
            fwrite(&new_offset,1,8,fp );
        }
    }
}

