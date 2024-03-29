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
#include <algorithm>
#include "unaligned_memory_access.h"
#include "Function.h"
#include <stdint.h>
#include <stdarg.h>

using namespace std;
using namespace Dyninst;
using namespace ParseAPI;
using namespace InstructionAPI;

FILE * log_file = 0;
int kr_printf_initialized = 0;
int kr_debug_printf = 0;
void kr_printf(const char * format , ... ){
    if(!kr_printf_initialized){
        kr_printf_initialized = 1;
        if(getenv("KR_DEBUG")){
            kr_debug_printf = 1; 
            if( log_file == NULL){
                log_file = fopen("reorder_tool_debug.txt","w");
            }
        }
    }
    if(!kr_debug_printf)
        return;

    va_list va;
    va_start(va,format);
    int ret = vfprintf(log_file, format, va);
    assert(ret >= 0);
    fflush(log_file);
    va_end(va);

}

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

    uint32_t PER_WAVEFRONT_OFFSET;
    uint32_t SDATA;

    uint32_t V_SPILL;;
    // VGPRS
    
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
    bool vgpr_spill;
} config;


typedef struct {
    uint32_t branch_address;
    uint32_t target_address;
    char cmd[64];
    uint32_t length;
} CFG_EDGE;


typedef struct myKernelDescriptor{
    uint32_t start_addr;
    uint32_t end_addr;
    uint32_t kd_offset; // offset in file where the kd is stored, shouldn't change
    char * buffer;
    vector<uint32_t> symbol_addrs;
    vector<uint32_t> symbol_update_value;
    vector<MyBranchInsn> branch_insns;
    vector<std::pair<uint32_t,uint32_t>> save_mask_insns;
    vector<uint32_t> endpgms;
    string name;
    myKernelDescriptor(){};
    bool operator < (const myKernelDescriptor & other) const {
        return start_addr < other.start_addr;
    }
}myKernelDescriptor;

void dumpBuffers(vector<myKernelDescriptor> & mkds);

void update_mkd_move( myKernelDescriptor & mkd, uint32_t new_start_addr ){

    uint32_t offset = new_start_addr - mkd.start_addr;
    for (auto & pr : mkd.save_mask_insns){
        pr.first += offset;
    }
    for ( auto & branch : mkd.branch_insns){
        uint32_t buffer_offset = branch._branch_addr - mkd.start_addr;
        branch.update_for_move(offset);
        memcpy(mkd.buffer+buffer_offset , branch.ptr, branch.size);
    }
    for ( auto & endpgm : mkd.endpgms){
        endpgm+= offset;
    } 

}

void update_save_mask_insns( myKernelDescriptor & mkd, uint32_t insert_location, uint32_t size){
    for (auto & pr : mkd.save_mask_insns){
        kr_printf("address = %x, mask reg = %u\n",pr.first, pr.second);
        if(insert_location <= pr.first){
            pr.first += size;
        }
    }
}
void update_mkd_branches( myKernelDescriptor & mkd, uint32_t insert_location, uint32_t size){
    int j = 0; 
    for ( auto & branch : mkd.branch_insns){
        kr_printf("bid = %d\n",j++);
        kr_printf("patching branch at address = %x, size = %u bytes\n", branch._branch_addr , branch.size);
        kr_printf("old branch code = %x\n",*(uint32_t *) branch.ptr);
        branch.update_for_insertion(insert_location,size);
        kr_printf("new branch code = %x ad address = %x\n",*(uint32_t *) branch.ptr, branch._branch_addr);
    }
    for ( auto & branch : mkd.branch_insns ){
        uint32_t buffer_offset = branch._branch_addr - mkd.start_addr;

        kr_printf("buffer_offset = %x  =  new address =  %x, %u bytes\n", buffer_offset, branch._branch_addr, branch.size);
        memcpy(mkd.buffer+buffer_offset , branch.ptr, branch.size);
    }

}

void update_mkd_endpgms(myKernelDescriptor & mkd, uint32_t insert_location ,uint32_t size){
    for ( auto & endpgm : mkd.endpgms){
        if(insert_location <= endpgm){
            endpgm+= size;
        }
    } 
}
void update_mkds_kds(vector<myKernelDescriptor> &mkds){
    for ( size_t i =1 ; i < mkds.size(); i ++){
        uint32_t prev_end = mkds[i-1].end_addr;
        if(prev_end > mkds[i].start_addr){
            uint32_t aligned_up_addr = ((prev_end + 1) / 0x100) * 0x100;
            uint32_t offset = aligned_up_addr - mkds[i].start_addr;
            mkds[i].start_addr += offset;
            mkds[i].end_addr += offset;
        }
    }
}

void analyze_binary(char * binaryPath, vector<myKernelDescriptor> &mkds){
    SymtabAPI::Symtab * symTab;
    string binaryPathStr(binaryPath);
    SymtabAPI::Symtab::openFile(symTab, binaryPathStr);
    vector<SymtabAPI::Symbol *> symbols;
    symTab->getAllSymbols(symbols);


    for(const auto & symbol : symbols){
        if (symbol->getType() == SymtabAPI::Symbol::ST_FUNCTION && symbol->isInSymtab()){
            myKernelDescriptor mkd;
            string name = symbol->getMangledName();
            string kd_name = name + ".kd";
            mkd.start_addr = symbol->getOffset();
            mkd.end_addr = mkd.start_addr + symbol->getSize();
            mkd.name = name;
            bool found = false;
            for (const auto & symbol2 : symbols){
                string name2 = symbol2->getMangledName();
                //std::cout << " mangeled names = " << name2 << " type = " << symbol2->getType() << std::endl;
                if(name2 == kd_name){
                    mkd.kd_offset =symbol2->getOffset();
                    found = true;
                }
            }
            if(!found){
                std::cerr << " can't find kernel descriptor for kernel " << name << std::endl;
                exit(-1);
            }
            mkd.buffer = (char *) malloc(mkd.end_addr - mkd.start_addr); 

            FILE * fp = fopen(binaryPath,"r");
            kr_printf("Reading kernel from address %x, size = %d\n",mkd.start_addr, mkd.end_addr - mkd.start_addr);
            fseek(fp, mkd.start_addr,SEEK_SET);
            fread(mkd.buffer,1,mkd.end_addr -mkd.start_addr, fp);
            fclose(fp);
            kr_printf(" Adding kernel to mkds\n");
            mkds.push_back(mkd);
        }else{
            //std::cerr << symbol->getType() << " symbol name " << symbol->getMangledName()  << std::endl;
        }
    }
    for(const auto & symbol : symbols){
        if(symbol->getType() == SymtabAPI::Symbol::ST_NOTYPE && symbol->isInSymtab()){
            uint32_t addr = symbol->getOffset();
            for(auto & mkd : mkds){
                if( mkd.start_addr < addr && addr < mkd.end_addr){
                    mkd.symbol_addrs.push_back(addr);
                }
            }
        }
    }

    SymtabAPI::Symtab::closeSymtab(symTab);
    vector<char *> insn_pool;
    map<Address,bool> seen;
    vector<Function *> funcs;
    SymtabCodeSource * sts;
    CodeObject * co;
    kr_printf("in %s \n",__func__);
    sts = new SymtabCodeSource(binaryPath);
    kr_printf("after sts\n");
    co = new CodeObject( sts);
    kr_printf("after co\n");
    co -> parse();
    kr_printf("after parse\n");
    auto arch = sts->getArch();

    const CodeObject::funclist & all = co->funcs();
    auto fit = all.begin();
    for (int i =0 ; fit != all.end(); fit++ , i++){
        Function * f = * fit;
        kr_printf("Creating a decoder, parsing region addr = %lx, len = %u\n",f->addr(), InstructionDecoder::maxInstructionLength);
        InstructionDecoder decoder("",InstructionDecoder::maxInstructionLength,f->region()->getArch());
        auto bit = f->blocks().begin();
        for (; bit != f->blocks().end(); ++ bit){
            Block * b = * bit;
            if(seen.find(b->start())!=seen.end()) continue;
            seen[b->start()] = true;
            for (auto & edges : b->targets()){
                Address branch_addr = edges->src()->lastInsnAddr();
                void * insn_ptr = f->isrc()->getPtrToInstruction(branch_addr);
                Instruction instr = decoder.decode((unsigned char *) insn_ptr);
                kr_printf("Address = %x : %s \n", branch_addr, instr.format().c_str());
                if(edges->type() == COND_TAKEN || edges->type() == DIRECT || edges->type() == CALL ){
                    //cout << instr.format() << endl;
                    CFG_EDGE edge;
                    edge.branch_address = branch_addr;
                    edge.target_address = edges->trg()->start();
                    memset(edge.cmd,0,sizeof(edge.cmd));
                    memcpy(edge.cmd, (const char *) insn_ptr, instr.size()  );
                    edge.length = instr.size();
                    for (auto & mkd : mkds){
                        if( mkd.start_addr <= branch_addr && branch_addr < mkd.end_addr){
                            mkd.branch_insns.push_back(InsnFactory::convert_to_branch_insn(edge.branch_address, edge.target_address, edge.cmd, edge.length, insn_pool));
                            break;
                        }
                    }
                }

            }
        }
    }

    std::sort(mkds.begin(),mkds.end());

    for (auto & mkd : mkds){
        kr_printf("HAHA\n");
        InstructionDecoder decoder(mkd.buffer,mkd.end_addr - mkd.start_addr,arch);
        Address buffer_offset = 0;
        Address addr = mkd.start_addr;
        while( addr < mkd.end_addr ){
            Address addr = mkd.start_addr+buffer_offset;
            Instruction instr = decoder.decode();
            //std::cout << "asddress = " << std::hex << addr<< " end addr = " << mkd.end_addr << "bool" << (addr < mkd.end_addr)<< std::endl;
            //std::cout << instr.format() << std::endl;;
            if(instr.format().find("S_ENDPGM") != string::npos){
                mkd.endpgms.push_back(addr);
            }
            if(instr.format().find("S_AND_SAVEEXEC") != string::npos){
                //kr_printf("FOUND SAVEEXEC AT ADDRESS = %x\n",addr);
                std::vector < InstructionAPI::Operand> operands;
                instr.getOperands(operands);
                for (auto & opr : operands){
                    if(opr.isWritten()){
                        auto name = opr.format(Arch_amdgpu_gfx940);
                        uint32_t start,end;
                        if(sscanf(name.c_str(),"S%d",&start)==1){
                            //std::cout << "OPRNAMEZ" << name<<"Z" << std::endl;
                            uint32_t sgpr = start;
                            mkd.save_mask_insns.push_back(make_pair(addr + instr.size() , sgpr));
                            break;
                        }else if (sscanf(name.c_str(),"S[%d:%d]",&start,&end)==2){
                            uint32_t sgpr = start;
                            mkd.save_mask_insns.push_back(make_pair(addr + instr.size() , sgpr));
                            break;
                        }
                        //std::cout << "OPERAND WRITTEN, NAME = " << opr.format(Arch_amdgpu_gfx940) << std::endl;    
                    }else{
                        //std::cout << "OPERAND NOT WRITTEN, NAME = " << opr.format(Arch_amdgpu_gfx940) << std::endl;    
                    }
                }
            }else{
            }
            buffer_offset += instr.size();
            if(addr >=mkd.end_addr){
                puts("FFFFFFFFFFFFFFFFFFFF");
                break;
            }
        }
    }
    delete co;
    delete sts;
    kr_printf("return from %s\n" , __func__);
}


/**
 * Inplace Insertion of a sequence of code to a fixed address
 * Each code object is described as buffer, with starting address text_offset
 * Insert offset within the buffer = Insert Location - MKD.start  
 * We need to copy back the
 *
 */ 
void inplace_insert(vector<myKernelDescriptor> &mkds, uint32_t mkd_id, vector<MyInsn> & toInsert, uint32_t insert_location ){
    uint32_t size_acc = 0;
    for(auto & insn : toInsert){
        size_acc+= insn.size;
    }


    myKernelDescriptor & mkd = mkds[mkd_id];
    char * tmp_buffer;

    uint32_t insert_offset = insert_location - mkd.start_addr;
    uint32_t new_buffer_size = mkd.end_addr - mkd.start_addr + size_acc;
    uint32_t copy_size = mkd.end_addr - insert_location;
    kr_printf("start_addr = %x, insert_location = %x, end_addr = %x\n",mkd.start_addr, insert_location, mkd.end_addr);
    kr_printf("insert_offset = %u, new_buffer_size = %u , copy_size = %u\n",insert_offset, new_buffer_size, copy_size);
    kr_printf("Inserting to address %p, size = %u\n", insert_location, size_acc);
    //kr_printf("increasing buffer size to %u\n", new_buffer_size);
    tmp_buffer = (char * ) malloc(new_buffer_size);
    kr_printf("tmp buffer start at address %08llx, ends at %08llx\n", tmp_buffer, tmp_buffer+ new_buffer_size);
    kr_printf("insert offset = %u\n",insert_offset);
    kr_printf("copying to address %08llx, ends at %08llx\n",tmp_buffer+insert_offset+size_acc,tmp_buffer+insert_offset+size_acc+copy_size);
    memmove(tmp_buffer+insert_offset+size_acc, mkd.buffer+insert_offset, copy_size);
    memmove(tmp_buffer,mkd.buffer,insert_offset);
    free(mkd.buffer);
    mkd.buffer = tmp_buffer;
    mkd.end_addr = mkd.end_addr + size_acc;

    uint32_t copy_offset = 0;
    for(auto & insn : toInsert){

        memmove(mkd.buffer+insert_offset+copy_offset,insn.ptr, insn.size);
        copy_offset += insn.size;
    }

    update_mkd_branches(mkd,insert_location,size_acc);
    update_mkd_endpgms(mkd,insert_location,size_acc);
    update_save_mask_insns(mkd,insert_location,size_acc);
    dumpBuffers(mkds);
    kr_printf("out func %s \n",__func__);
}


uint32_t getSgpr(config &conf, bool pair = false);
uint32_t getVgpr(config &conf, bool pair = false);
uint32_t getSgpr(config &conf, bool pair ){

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

uint32_t getVgpr(config &conf, bool pair){


    if(pair){
        conf.vgpr_max+=2;
        return conf.vgpr_max-1;
    }
    conf.vgpr_max+=1;
    return conf.vgpr_max;
}


void read_config(FILE * fp, char * configPath , vector<config> &configs , vector<myKernelDescriptor> & mkds){
    kr_printf("reading config\n");
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

        for(auto & mkd : mkds){
            if(mkd.name == kernel_name){
                c.func_start = mkd.start_addr;
                c.func_end = mkd.end_addr;
                c.kd_addr = mkd.kd_offset;
                kd_offset = mkd.kd_offset;
                c.name = kernel_name;
                found = true;
            }else{
                //std::cerr << " kernel name = " << kernel_name << " mkd name = " << mkd.name << std::endl;
            }
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
            kr_printf("dispatch ptr = %u\n",c.dispatch_ptr);
        }

        if(AMDHSA_BITS_GET(fkd.kernel_code_properties,llvm::amdhsa::KERNEL_CODE_PROPERTY_ENABLE_SGPR_QUEUE_PTR)){
            c.queue_ptr = sgpr_count;
            sgpr_count +=2 ;

            kr_printf("queue ptr = %u\n",c.queue_ptr);
        }

        if(AMDHSA_BITS_GET(fkd.kernel_code_properties,llvm::amdhsa::KERNEL_CODE_PROPERTY_ENABLE_SGPR_KERNARG_SEGMENT_PTR )){
            c.kernarg_segment_ptr = sgpr_count;
            sgpr_count += 2;

            kr_printf("kerarg seg ptr = %u\n",c.kernarg_segment_ptr);
        }else{
            kr_printf("no kernarg seg ptr \n");    
        }



        if(AMDHSA_BITS_GET(fkd.kernel_code_properties,llvm::amdhsa::KERNEL_CODE_PROPERTY_ENABLE_SGPR_DISPATCH_ID )){
            c.dispatch_id = sgpr_count;
            sgpr_count += 2;

            kr_printf("dispatch id = %u\n",c.dispatch_id);
        }


        if(AMDHSA_BITS_GET(fkd.kernel_code_properties,llvm::amdhsa::KERNEL_CODE_PROPERTY_ENABLE_SGPR_FLAT_SCRATCH_INIT )){
            //c.flat_scratch_init = sgpr_count;
            //
            sgpr_count += 2;
        }

        if(AMDHSA_BITS_GET(fkd.kernel_code_properties,llvm::amdhsa::KERNEL_CODE_PROPERTY_ENABLE_SGPR_PRIVATE_SEGMENT_SIZE )){
            c.private_segment_size = sgpr_count;
            sgpr_count += 1;

            kr_printf("private segment size   = %u\n",c.private_segment_size);
        }

        if(c.work_group_id_x_enabled){
            c.work_group_id_x = sgpr_count;
            sgpr_count +=1;
            kr_printf("workgroup_id_x = %u, sgpr_count = %u\n",c.work_group_id_x,sgpr_count);
        }

        if(c.work_group_id_y_enabled){
            c.work_group_id_y = sgpr_count;
            sgpr_count +=1;

            kr_printf("workgroup_id_y = %u\n",c.work_group_id_y);
        }

        if(c.work_group_id_z_enabled){
            c.work_group_id_z = sgpr_count;
            sgpr_count +=1;

            kr_printf("workgroup_id_z = %u\n",c.work_group_id_z);
        }
        c.vgpr_spill = false;
        if( c.sgpr_max >= 101 ) {
            kr_printf("scalar register >= 101, try to use vgpr \n");
            if( c.vgpr_max >= 255){
                kr_printf("vector registers also maxed out, need to do stack\n");
                assert(0 && " maxed usage of both scalar and vector register");
            }
            c.vgpr_spill = true;
            c.V_SPILL = getVgpr(c);
        }
        c.first_uninitalized_sgpr = sgpr_count;
        if(sgpr_count %2)
            c.first_uninitalized_sgpr+=1;

        if(c.first_uninitalized_sgpr+10 > c.sgpr_max){
            c.sgpr_max = c.first_uninitalized_sgpr+10;
        }

        c.PER_WAVEFRONT_OFFSET = getSgpr(c,true);
        c.SDATA = getSgpr(c,true);
        configs.push_back(c);
    }
}


uint32_t getSize(vector<MyInsn> &insns){
    uint32_t ret = 0;
    for (auto & insn : insns){
        ret += insn.size;
    }
    return ret;
}
void dumpBuffers(vector<myKernelDescriptor> & mkds){
    kr_printf("in %s \n",__func__);
    for(const auto & mkd : mkds){
        long long int * mbuffer = ( long long int * ) mkd.buffer;
        kr_printf("start address = %x , end address = %x, buffer = %08llx, kd_offset = %x\n",mkd.start_addr , mkd.end_addr,mbuffer,mkd.kd_offset);
        kr_printf("writing buffer  output , first 16 bytes = %08llx %08llx\n",mbuffer[0],mbuffer[1]);
    }
    kr_printf("out %s \n",__func__);


    kr_printf("in %s \n",__func__);
    int i = 0;
    for(const auto & mkd : mkds){
        kr_printf("Kernel id = %d\n",i);
        int j =0 ;
        for ( auto & branch : mkd.branch_insns){
            kr_printf("\tbid = %d : ptr = %llx, branch_addr = %llx\n",j++,branch.ptr, branch._branch_addr);
        }
        i++;
    }
    kr_printf("out %s \n",__func__);

}


void propogate_mkd_update(vector<myKernelDescriptor> & mkds, uint32_t mkd_id){
    kr_printf("in %s\n",__func__);
    for( uint32_t nid = mkd_id +1 ; nid < mkds.size(); nid ++){
        uint32_t prev_end = mkds[nid-1].end_addr;
        kr_printf("nid = %u , prev_end = %x , next_start = %x\n",nid,prev_end,mkds[nid].start_addr);
        if(prev_end > mkds[nid].start_addr){
            uint32_t aligned_up_addr = ((prev_end + 0xff) / 0x100) * 0x100;
            uint32_t offset = aligned_up_addr - mkds[nid].start_addr;

            // Here the start_addr is still pointing to the old offset
            update_mkd_move(mkds[nid],aligned_up_addr);
            mkds[nid].start_addr += offset;
            mkds[nid].end_addr += offset;


            kr_printf("aligned_up_addr = %x, offset = %u\n",aligned_up_addr,offset);
            kr_printf("new start address for nid = %u [%x,%x)\n",nid,mkds[nid].start_addr,mkds[nid].end_addr);

        }
    }


}





void finalize(char * filename, vector<myKernelDescriptor> &mkds, uint32_t text_offset){

    kr_printf("in func %s \n",__func__);
    dumpBuffers(mkds);

    char new_filename[1000];
    snprintf(new_filename,strlen(filename)+10,"%s.text.new",filename);

    //update_mkds_kds(mkds);

    FILE * f_co;
    f_co = fopen(filename,"r+");
    if(f_co){
        puts("Code Object Opened");
    }
    for(const auto & mkd : mkds){
        fseek(f_co,mkd.kd_offset+16,SEEK_SET);
        long long unsigned new_offset = mkd.start_addr - mkd.kd_offset;
        fwrite(&new_offset,1,8,f_co);
        kr_printf("updating offset in kernel descriptor, address = %p\n",mkd.kd_offset+16); 
    }
    fclose(f_co);

    FILE * f_text = fopen(new_filename,"w");
    for(const auto & mkd : mkds){
        kr_printf("fseeking to file offset = %x\n",mkd.start_addr - text_offset);
        fseek(f_text,mkd.start_addr-text_offset,SEEK_SET);

        long long int * mbuffer = ( long long int * ) mkd.buffer;
        kr_printf("writing buffer  output , first 16 bytes = %08llx %08llx\n",mbuffer[0],mbuffer[1]);
        fwrite(mkd.buffer,mkd.end_addr-mkd.start_addr,1,f_text);
    }
    fclose(f_text);
    puts("Code Object closed successfully");
}


