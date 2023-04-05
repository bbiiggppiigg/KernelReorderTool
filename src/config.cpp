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
#include "KdUtils.h"
#include "KernelDescriptor.h"
#include <fstream>


#include "Function.h"
#include "config.h"

using namespace std;
using namespace Dyninst;
using namespace ParseAPI;
using namespace InstructionAPI;

/**
 * Make use of SymtabAPI to collect inforamtion about kernel descriptors / kernels bounds and the last instruction of the file
 */ 
void getKernelBounds(char * binaryPath, vector<kernel_bound> & kernel_bounds, uint32_t & last_instr ){
    SymtabAPI::Symtab * symTab;
    string binaryPathStr(binaryPath);
    printf("before calling openFile\n");
    SymtabAPI::Symtab::openFile(symTab, binaryPathStr);



  std::string filePath(binaryPath);
  std::ifstream inputFile(filePath);

  inputFile.seekg(0, std::ios::end);
  size_t length = inputFile.tellg();
  inputFile.seekg(0, std::ios::beg);

  char *buffer = new char[length];
  inputFile.read(buffer, length);

  Elf_X *elfHeader = Elf_X::newElf_X(buffer, length);  printf("before calling getAllModules \n");
    last_instr = 0;

        vector<SymtabAPI::Symbol *> symbols;
        symTab->getAllSymbols(symbols);

        for(const auto & symbol : symbols){
         if (symbol->getType() == SymtabAPI::Symbol::ST_FUNCTION && symbol->isInSymtab()){
            uint32_t offset = symbol->getOffset();
            uint32_t size = symbol->getSize();
            string name = symbol->getMangledName();
            bool found = false;
            string kd_name = name + ".kd";
            Address kdAddr;

            kernel_bound kb;
            for (const auto & symbol2 : symbols){
                string name2 = symbol2->getMangledName();

                std::cout << " mangeled names = " << name2 << " type = " << symbol2->getType() << std::endl;
                if(name2 == kd_name){
                    found = true;
                    kdAddr = symbol2->getOffset();
                     
                    kb.kd = new KernelDescriptor(symbol2,elfHeader);
                    break;
                }
            }
            if(!found){
                printf("failed to find corresponding kernel descriptor");
                exit(-1);
            }

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

void analyze_binary(char * binaryPath, vector<CFG_EDGE> & ret_edges , vector<std::pair<uint32_t,uint32_t>>  & save_mask_insns , uint32_t func_start , uint32_t func_end , vector<uint32_t> & endpgms ){
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
        printf("Creating a decoder, parsing region addr = %lx, len = %u\n",f->addr(), InstructionDecoder::maxInstructionLength);
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
        if(instr.format().find("S_AND_SAVEEXEC") != string::npos){
            std::vector < InstructionAPI::Operand> operands;
            instr.getOperands(operands);
            for (auto & opr : operands){

                if(opr.isWritten()){
                    auto name = opr.format(Arch_amdgpu_cdna2);
                    uint32_t start ,end ;

                    if(sscanf(name.c_str(),"S[%d:%d]",&start,&end)==2){

                        std::cout << "OPRNAMEZ" << name<<"Z" << std::endl;
                        uint32_t sgpr = start;
                        //sscanf(name.substr(pos+5).c_str(),"%d",&sgpr);
                        printf("s and save exec address = %lx , sgpr %d\n",baseAddr + offset + instr.size()  ,sgpr);
                        save_mask_insns.push_back(make_pair(baseAddr+offset + instr.size() , sgpr));
                    }

                }
            }
        }else if(instr.format().find("S_ENDPGM") != string::npos){
            endpgms.push_back(baseAddr+offset);
        }
        offset += instr.size();
    }
    delete co;
    delete sts;
    printf("return from %s\n" , __func__);
    
}


uint32_t get_sgpr(config &conf, bool pair ){

    if(pair){
        if((conf.sgpr_max & 1)   ==0){ // even, next is odd, skip one register ( say use up to 60), we should allocate 62,63
            conf.sgpr_max+=1; // 61
        }

        conf.sgpr_max+=2;
        return conf.sgpr_max-1;
    }

    conf.sgpr_max+=1;
    return conf.sgpr_max;
}

uint32_t get_vgpr(config &conf, bool pair){


    if(pair){
        conf.vgpr_max+=2;
        return conf.vgpr_max-1;
    }
    conf.vgpr_max+=1;
    return conf.vgpr_max;
}


void read_config(FILE * fp, char * configPath , vector<config> &configs , vector<kernel_bound> &kernel_bounds){
    puts("reading config");
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
        //KernelDescriptor * kd;
        for(auto kb : kernel_bounds){
            if(kb.name == kernel_name){

                c.func_start = kb.first;
                c.func_end = kb.last;

                kd_offset = kb.kdAddr;
                c.kd_addr = kd_offset;

                c.name = kernel_name;
                //kd = kb.kd;
                found = true;
            }
            printf("HAHA\n");
        }
        if(!found){
            puts("ERROR! wrong kernel name specified in cofig file");
            exit(-1);
        }
        c.sgpr_max = reader.GetInteger(section,"sgpr_max",-1); 
        c.vgpr_max = reader.GetInteger(section,"vgpr_max",-1); 

        llvm::amdhsa::kernel_descriptor_t fkd;
        fseek(fp,kd_offset,SEEK_SET);
        fread(&fkd,sizeof(fkd),1,fp);
        uint32_t code_entry_offset = fkd.kernel_code_entry_byte_offset;
        c.code_entry_offset = code_entry_offset;
        c.old_kernarg_size = reader.GetInteger(section,"kernarg_size",0);
        if(c.old_kernarg_size %8){
            c.old_kernarg_size = ((c.old_kernarg_size>>3) + 1)<<3;
        } 


        c.work_group_id_x_enabled = AMDHSA_BITS_GET(fkd.compute_pgm_rsrc2,llvm::amdhsa::COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_ID_X);
        c.work_group_id_y_enabled = AMDHSA_BITS_GET(fkd.compute_pgm_rsrc2,llvm::amdhsa::COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_ID_Y);
        c.work_group_id_z_enabled = AMDHSA_BITS_GET(fkd.compute_pgm_rsrc2,llvm::amdhsa::COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_ID_Z);
        c.work_item_id_enabled = AMDHSA_BITS_GET(fkd.compute_pgm_rsrc2,llvm::amdhsa::COMPUTE_PGM_RSRC2_ENABLE_VGPR_WORKITEM_ID);
        //assert(c.work_group_id_x_enabled == kd->getCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupIdX());
        //assert(c.work_group_id_y_enabled == kd->getCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupIdY());
        //assert(c.work_group_id_z_enabled == kd->getCOMPUTE_PGM_RSRC2_EnableSgprWorkgroupIdZ());

        uint32_t sgpr_count=0;

        sgpr_count = AMDHSA_BITS_GET(fkd.kernel_code_properties,llvm::amdhsa::KERNEL_CODE_PROPERTY_ENABLE_SGPR_PRIVATE_SEGMENT_BUFFER) ? 4 : 0;
        //assert( AMDHSA_BITS_GET(fkd.kernel_code_properties,llvm::amdhsa::KERNEL_CODE_PROPERTY_ENABLE_SGPR_PRIVATE_SEGMENT_BUFFER) == kd->getKernelCodeProperty_EnableSgprPrivateSegmentBuffer());


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
            printf("workgroup_id_x = %u, sgpr_count = %u\n",c.work_group_id_x,sgpr_count);
        }

        if(c.work_group_id_y_enabled){
            c.work_group_id_y = sgpr_count;
            sgpr_count +=1;

            printf("workgroup_id_y = %u\n",c.work_group_id_y);
        }

        if(c.work_group_id_z_enabled){
            c.work_group_id_z = sgpr_count;
            sgpr_count +=1;

            printf("workgroup_id_z = %u\n",c.work_group_id_z);
        }

        c.first_uninitalized_sgpr = sgpr_count;
        if(sgpr_count %2)
            c.first_uninitalized_sgpr+=1;
         
        if(c.first_uninitalized_sgpr+4 > c.sgpr_max){
            c.sgpr_max = c.first_uninitalized_sgpr+4;
        }

        
        c.BACKUP_WRITEBACK_ADDR = get_sgpr(c,true); // even
        c.TIMER_1 = get_sgpr(c,true); // even
        c.TIMER_2 = get_sgpr(c,true); // even
        c.BACKUP_EXEC = get_sgpr(c,true); // even
        c.WORK_GROUP_ID = get_sgpr(c); // odd

        c.TMP_SGPR0 = get_sgpr(c,true); // even
        c.TMP_SGPR1 = get_sgpr(c); // odd
        c.GLOBAL_WAVEFRONT_ID = get_sgpr(c); // even
        c.LOCAL_WAVEFRONT_ID = get_sgpr(c); // odd
        c.DEBUG_SGPR0 = get_sgpr(c);
        c.DEBUG_SGPR1 = get_sgpr(c);


       /* 
        c.BACKUP_WRITEBACK_ADDR = sgpr_max+1; // even
        c.TIMER_1 = c.BACKUP_WRITEBACK_ADDR+2; // even
        c.TIMER_2 = c.TIMER_1+2; // even
        c.BACKUP_EXEC = c.TIMER_2 + 2; // even
        c.WORK_GROUP_ID = c.BACKUP_EXEC +1; // odd

        c.TMP_SGPR0 = c.WORK_GROUP_ID +1; // even
        c.TMP_SGPR1 = c.TMP_SGPR0 +1; // odd
        c.GLOBAL_WAVEFRONT_ID = c.TMP_SGPR1+1; // even
        c.LOCAL_WAVEFRONT_ID = c.GLOBAL_WAVEFRONT_ID+1; // odd
*/
        c.DS_ADDR = get_vgpr(c);
        c.DS_DATA_0 = get_vgpr(c);
        c.DS_DATA_1 = get_vgpr(c);
        c.V_MINUS_1 = get_vgpr(c);
        c.v_global_addr = get_vgpr(c,true);
        configs.push_back(c);
    }
}

void update_symtab_symbols(FILE * f, uint32_t text_start , uint32_t text_end , uint32_t insert_loc , uint32_t insert_size);


void update_endpgms(FILE* , vector<uint32_t> & endpgms, uint32_t insert_loc , uint32_t insert_size){
   for ( auto & endpgm : endpgms){
        if(insert_loc < endpgm){
            endpgm+= insert_size;
        }
    } 
}
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
void inplace_insert(FILE * fp , const uint32_t text_start , uint32_t & text_end , vector<MyInsn> & insns, vector<MyBranchInsn> & branches, uint32_t insert_location,  vector<kernel_bound> &kbs , vector<uint32_t> & endpgms ,vector<char *> &){    


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
    update_endpgms(fp,endpgms,insert_location,size_acc);
    
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

void offsetted_inplace_insert(uint32_t offset , FILE * fp , const uint32_t text_start , uint32_t & text_end , vector<MyInsn> & insns, vector<MyBranchInsn> & branches, uint32_t insert_location,  vector<kernel_bound> &kbs , vector<uint32_t> & endpgms ,vector<char *> &){    


    uint32_t buffer_size = text_end - insert_location;
    void * file_buffer = malloc(sizeof(char *) * (buffer_size));
    printf("inserting instructions to address %x\n",insert_location);
    fseek(fp,offset+insert_location,SEEK_SET);
    fread(file_buffer, 1 , buffer_size , fp );
    fseek(fp,offset+insert_location,SEEK_SET);
    uint32_t size_acc = 0;
    for(auto & insn : insns){
        insn.write(fp);
        size_acc+= insn.size;

    }
    fwrite(file_buffer,1,buffer_size,fp);
    update_symtab_symbols(fp, text_start ,text_end, insert_location, size_acc);
    update_branches(fp,branches,insert_location,size_acc);
    update_endpgms(fp,endpgms,insert_location,size_acc);
    
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


