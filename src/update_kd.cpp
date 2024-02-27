
#include <elf.h>
#include <vector>
#include <iostream>
#include <cstring>
#include <cassert>
#include <cstdlib>
#include "../lib/AMDHSAKernelDescriptor.h"
#include "kernel_elf_helper.h"
#define ElfW Elf64_Ehdr
#define Shdr Elf64_Shdr
#define Phdr Elf64_Phdr
using namespace std;



/* Update the symbol of labels that is after the insertion point such that llvm-objdump works correctly
*/

void update_kd_symbols(FILE * f){
    ElfW header;
    fseek(f,0,SEEK_SET);
    fread(&header,sizeof(header),1,f);

    Shdr shstrtable_header;
    read_shdr(&shstrtable_header,f,&header,header.e_shstrndx);
    char * shstrtable = read_section(f,&shstrtable_header);

    Shdr tmp_hdr;

    Shdr text_hdr;
    Shdr symtab_hdr;
    Shdr strtab_hdr;
    Shdr dynsym_hdr;
    Shdr dynstr_hdr;

    //int text_index = -1;
    //int symtab_index = -1;
    for (unsigned int i = 1; i < header.e_shnum ; i ++){
        read_shdr(&tmp_hdr,f,&header,i);
        char * sh_name = shstrtable+tmp_hdr.sh_name;
        if(0==strcmp(sh_name,".text")){
            //		text_index = i ;
            text_hdr = tmp_hdr;
        }
        if(0==strcmp(sh_name,".symtab")){
            symtab_hdr = tmp_hdr;
            //		symtab_index = i;
        }
        if(0==strcmp(sh_name,".strtab")){
            strtab_hdr = tmp_hdr;
        }
        if(0==strcmp(sh_name,".dynstr")){
            dynstr_hdr = tmp_hdr;
        }

        if(0==strcmp(sh_name,".dynsym")){
            dynsym_hdr = tmp_hdr;
        }


    }

    char * strtab_content = read_section(f,&strtab_hdr);
    char * dynstr_content = read_section(f,&dynstr_hdr);


    Elf64_Sym * symtab_content = (Elf64_Sym *) read_section(f,&symtab_hdr);
    Elf64_Sym * dynsym_content = (Elf64_Sym *) read_section(f,&dynsym_hdr);


    int num_entries = symtab_hdr.sh_size / symtab_hdr.sh_entsize;
    //int target_index = -1;
    for (int i =0 ; i < num_entries ;i ++){
        Elf64_Sym * symbol = symtab_content+i;
        char * symbol_name = strtab_content + symbol -> st_name;
        if(strstr(symbol_name,".kd")!=0){
            uint32_t kd_offset = symbol->st_value;
            printf("KD AT OFFSET : 0x%x\n", kd_offset);
            llvm::amdhsa::kernel_descriptor_t fkd;
            fseek(f,kd_offset,SEEK_SET);
            fread(&fkd,sizeof(fkd),1,f);

            AMDHSA_BITS_SET(fkd.compute_pgm_rsrc2,llvm::amdhsa::COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_ID_X,1);
            AMDHSA_BITS_SET(fkd.compute_pgm_rsrc2,llvm::amdhsa::COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_ID_Y,1);
            AMDHSA_BITS_SET(fkd.compute_pgm_rsrc2,llvm::amdhsa::COMPUTE_PGM_RSRC2_ENABLE_SGPR_WORKGROUP_ID_Z,1);

            fseek(f,kd_offset,SEEK_SET);
            fwrite(&fkd,sizeof(fkd),1,f);
        }
    }
}

int main(int argc, char * argv[]){
    assert(argc > 1);
    FILE * fp = fopen(argv[1],"rw+");
    update_kd_symbols(fp);
    fclose(fp);
    return 0;    
}
