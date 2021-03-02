#include <elf.h>
#include <vector>
#include <iostream>
#include "CodeObject.h"
#include "InstructionDecoder.h"
using namespace std;
using namespace Dyninst;
using namespace ParseAPI;

using namespace InstructionAPI;

#define WAIT_CONST 10

#define ElfW Elf64_Ehdr
#define Shdr Elf64_Shdr
void read_shdr(Shdr * shdr,FILE * f,  ElfW* hdr, int offset){
    fseek(f,hdr->e_shoff + sizeof(Shdr) * offset ,SEEK_SET);
    fread(shdr,sizeof(Shdr),1,f);
}

char * read_section(FILE * f, Shdr * shdr) {
    int offset = shdr->sh_offset;
    int size = shdr->sh_size;
    fseek(f,offset,SEEK_SET);
    char * ret = (char * ) malloc(size + 1 ) ;
    fread(ret,size,1,f);
    return ret;
}


char *  get_text(FILE * f, long long int &offset , long long int &size){
    ElfW header;
    fread(&header,sizeof(header),1,f);

    Shdr strtable_header; 
    read_shdr(&strtable_header,f,&header,header.e_shstrndx);
    char * strtable = read_section(f,&strtable_header);

    Shdr tmp_hdr;
    int fatbin_index = -1;
    for (unsigned int i = 1; i < header.e_shnum ; i ++){
        read_shdr(&tmp_hdr,f,&header,i);
        char * sh_name = strtable+tmp_hdr.sh_name;
        if(0==strcmp(sh_name,".text")){
            printf("found text!\n");
            fatbin_index = i;
            break;
        }
    }
    if(fatbin_index == -1)
        assert(0 && "failed");
    free(strtable);
    Shdr text_header;
    read_shdr(&text_header,f,&header,fatbin_index);
    char * text_section = read_section(f,&text_header);
    offset = text_header.sh_offset;
    size = text_header.sh_size;
    printf("offset = %llx, size = %lld\n",offset,size);
    return text_section;
}


const char * s_nop = "\x00\x00\x80\xbf";
class MyInsn {
    public:
        const void * ptr;
        unsigned int size;
        string _pretty;
        MyInsn( const void * _ptr , unsigned int _size, string pretty) : ptr(_ptr) , size(_size), _pretty(pretty) {

        };

};

class MyWaitCntInsn : public MyInsn {
    public:
    uint32_t insn; 
    MyWaitCntInsn (char * my_ptr , int vmcnt, int lgkmcnt , int expcnt = 0 ) : MyInsn(my_ptr,4, "s_waitcnt"){
        this->insn = 0xbf8c0000;
        uint16_t simm16 = 0 ;

        uint16_t vm_low = vmcnt & 0xf;
        uint16_t vm_high = (vmcnt & 0x30) >> 4;
        uint16_t exp_cnt = expcnt & 0x7; 
        uint16_t lgkm_cnt = lgkmcnt & 0xf;
        simm16 = (vm_high << 14) | (lgkm_cnt << 8) | (exp_cnt << 4) | (vm_low);
        insn |= simm16;
        std::ostringstream oss; 
        memcpy(my_ptr,&insn,32);

        oss << "s_waitcnt  " << "vmcnt(" << vmcnt << ")" << " lgkmcnt(" << lgkmcnt << ")" << " expcnt(" << expcnt  << ")";
        this->_pretty = oss.str();
        printf("after constructing waitcnt insn, value = %x\n",insn);
        
    };
};

class MyFunc {
    public:
        vector<MyInsn> myInsns;
        unsigned int offset; 
        Address _start ;
        Address _end;
        InstructionDecoder & _dec;
      
        void increase_waitcnt(char * my_ptr , int target, int vmcnt , int lgkmcnt ){
            uint32_t insn = *((uint32_t *) myInsns[target].ptr);
            //printf("target is : %x, result = %x\n",insn, insn & 0xffff0000);
            if ((insn & 0xffff0000) != 0xbf8c0000){
                puts("incorrect target for increasing waitcnt");
                return;
            }else{
                //puts("correct target for increasing waitcnt");
            }
            myInsns[target] = MyWaitCntInsn(my_ptr, vmcnt,lgkmcnt,0);
        } 

        void replace_with_nop(int target){
            MyInsn s_nop_insn = MyInsn(s_nop,4,"s_nop");
            int target_size = myInsns[target].size;
            int num_to_replace = (target_size  / 4) - 1;
            myInsns[target] = s_nop_insn;
            while(num_to_replace >0){
                auto it = myInsns.begin()+target+1;
                myInsns.insert( it , s_nop_insn);
                num_to_replace -=1;
            }
            
        } 

        MyFunc(char * buffer  , Function * f, long long int offset ,InstructionDecoder & decoder):_dec(decoder) {
            _start = f->addr();
            auto fbl = f->blocks().end();
            fbl--;
            Block *b = *fbl;
            _end = b->last();
            _end += _dec.decode( (unsigned char *)buffer + _end - offset).size();
            Address crtAddr = _start;
            unsigned char * decode_target;
            while(crtAddr < _end ){
                decode_target = (unsigned char *) buffer + crtAddr - offset;
                Instruction instr = _dec.decode(decode_target);
                append(decode_target , instr.size() , instr.format() );
                crtAddr += instr.size();
            }
        };

        void append(MyInsn insn){
            myInsns.push_back(insn);
        }

        void append(unsigned char * ptr, unsigned int size, string pretty){
            myInsns.push_back(MyInsn(ptr,size,pretty));
        }

        void format(){
            printf("\n\noutputing for function starting at address : %08lx\n",_start);
            Address curAddr = _start;
            for (size_t i =0; i < myInsns.size() ; i++){
                printf("%lu:%08lx: %s \n",i,curAddr, myInsns[i]._pretty.c_str() );
                curAddr += myInsns[i].size;
            }

        } 


        void move_before( int target , int dst ){
            if( target < dst ){
                printf("move before require target: %d > dst : %d\n",target,dst);
                return ;
            }
            for (int i = target ; dst < i ; i--){
                std::swap(myInsns[i],myInsns[i-1]);
            }

        }
        void move_after( int target , int dst ){
            if( target > dst ){
                printf("move before require target: %d < dst : %d\n",target,dst);
                return ;
            }
            for (int i = target ; dst > i ; i++){
                std::swap(myInsns[i],myInsns[i+1]);
            }

        }

        char * serialize(){
            char * ret = (char * ) malloc(_end-_start);
            printf("size of ret = %lu\n",_end-_start);
            int offset = 0;
            for (auto &insn : myInsns){
                printf("copying instruction %s\n", insn._pretty.c_str()); 
                printf("ptr value = %x , size = %d\n", *((uint32_t *) insn.ptr), insn.size);

                memcpy(ret+offset, (char *) insn.ptr, insn.size );
                offset += insn.size;
            }
            return ret; 
        }
};


int main(int argc, char **argv){



    if(argc != 2){
        printf("Usage: %s <binary path>\n", argv[0]);
        return -1;
    }
    char *binaryPath = argv[1];

    FILE * fp = fopen(binaryPath,"rb");
    long long int offset; // offset to the text section
    long long int size; // size of the section
    char * buffer = get_text(fp,offset,size);

    fclose(fp);
    SymtabCodeSource *sts;
    CodeObject *co;
    Instruction instr; //mod
    SymtabAPI::Symtab *symTab;
    std::string binaryPathStr(binaryPath);
    bool isParsable = SymtabAPI::Symtab::openFile(symTab, binaryPathStr);
    if(isParsable == false){
        const char *error = "error: file can not be parsed";
        cout << error;
        return - 1;
    }
    sts = new SymtabCodeSource(binaryPath);
    co = new CodeObject(sts);
    //parse the binary given as a command line arg
    co->parse();

    //get list of all functions in the binary
    const CodeObject::funclist &all = co->funcs();
    if(all.size() == 0){
        const char *error = "error: no functions in file";
        cout << error;
        return - 1;
    }
    auto fit = all.begin();

    Function *f = *fit;
    InstructionDecoder decoder(f->isrc()->getPtrToInstruction(f->addr()),
            InstructionDecoder::maxInstructionLength,
            f->region()->getArch());

    vector<MyFunc> myfuncs;

    for(;fit != all.end(); ++fit){
        Function *f = *fit;
        myfuncs.push_back(MyFunc(buffer,f,offset,decoder)); 
    }


    char cmd[100];
    vector<char * > insn_pool;
   
    printf("waiting for input :\n");
    puts("commands: list , edit , nop, wait, quit ");
    printf("waiting for input :\n");
    while(~scanf("%s",cmd)){


        if(strcmp(cmd,"quit")==0){
            break;
        }else if(strcmp(cmd,"list")==0){
            printf("Number of funcs :%lu \n",myfuncs.size());
            printf("Choose your target :\n");
            int target ;
            scanf("%d",&target);
            myfuncs[target-1].format();

        }else if (strcmp(cmd,"edit")==0){
            printf("Number of funcs :%lu \n",myfuncs.size());
            puts("Choose your target (starting from 1) :");
            int target , mov_target , mov_dst;
            scanf("%d",&target);
            target -=1 ;
            puts("before or after ?"); 
            scanf("%s",cmd);
            if(strcmp(cmd,"before")==0){
                scanf("%d%d",&mov_target,&mov_dst);
                myfuncs[target].move_before(mov_target,mov_dst);
            }
            else if(strcmp(cmd,"after")==0){
                scanf("%d%d",&mov_target,&mov_dst);
                myfuncs[target].move_after(mov_target,mov_dst);
            }
            else{
                puts("unsupported");
            }
            myfuncs[target].format();

        }else if (strcmp(cmd,"help")==0){
            puts("Supported commands are : list, edit , nop , wait and quit ");
        
        }else if(strcmp(cmd,"nop")==0){
            printf("Number of funcs :%lu \n",myfuncs.size());
            puts("Choose your target function (starting from 1):");
            int func_index, insn_index;
            scanf("%d",&func_index);
            func_index -= 1;
            puts("Choose your target instruction");
            scanf("%d",&insn_index);
            myfuncs[func_index].replace_with_nop(insn_index);
         }else if(strcmp(cmd,"wait")==0){
            printf("Number of funcs :%lu \n",myfuncs.size());
            puts("Choose your target function (starting from 1):");
            int func_index, insn_index;
            scanf("%d",&func_index);
            func_index -= 1;
            puts("Choose your target instruction");
            scanf("%d",&insn_index);
            char * tmp_insn = (char * ) malloc(32);
            myfuncs[func_index].increase_waitcnt(tmp_insn,insn_index,WAIT_CONST,WAIT_CONST);
            insn_pool.push_back(tmp_insn);

        }else{
            printf("unsupported command %s:\n",cmd);
            puts("Supported commands are : list, edit , nop , wait and quit ");
        }
        printf("waiting for input :\n");
    }


    fp = fopen(binaryPath,"rb+");

    for (auto & myfunc  : myfuncs ){
        char * new_kernel = myfunc.serialize();
        fseek(fp,myfunc._start,SEEK_SET);
        unsigned write_size = myfunc._end - myfunc._start;
        fwrite(new_kernel,sizeof(char),write_size,fp);
        free(new_kernel);
        printf("writing to offset : %lx, size = %d(%x)\n",myfunc._start,write_size,write_size);
    }

    fclose(fp);
    for (auto & tmp_insn : insn_pool){
        free(tmp_insn);
    }

}




