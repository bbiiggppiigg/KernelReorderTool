#include <elf.h>
#include <vector>
#include <iostream>
#include <cstring>
#include <cassert>

#define WAIT_CONST 14

#define ElfW Elf64_Ehdr
#define Shdr Elf64_Shdr
using namespace std;
void read_shdr(Shdr * shdr,FILE * f,  ElfW* hdr, int offset){
    fseek(f,hdr->e_shoff + sizeof(Shdr) * offset ,SEEK_SET);
    fread(shdr,sizeof(Shdr),1,f);
}

char * read_section(FILE * f, Shdr * shdr) {
    int offset = shdr->sh_offset;
    int size = shdr->sh_size;
    fseek(f,offset,SEEK_SET);
    char * ret = (char * ) malloc(size) ;
    fread(ret,size,1,f);
    return ret;
}



void extend_symbol(FILE * f, char * target_symbol_name, int new_symbol_size){
    ElfW header;
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

    int text_index = -1;
    int symtab_index = -1;
    int dynsym_index = -1;
    for (unsigned int i = 1; i < header.e_shnum ; i ++){
        read_shdr(&tmp_hdr,f,&header,i);
        char * sh_name = shstrtable+tmp_hdr.sh_name;
        if(0==strcmp(sh_name,".text")){
            text_index = i ;
            text_hdr = tmp_hdr;
        }
        if(0==strcmp(sh_name,".symtab")){
            symtab_hdr = tmp_hdr;
            symtab_index = i;
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

    int num_entries = symtab_hdr.sh_size / symtab_hdr.sh_entsize;
    int target_index = -1;
    for (int i =0 ; i < num_entries ;i ++){
        Elf64_Sym * symbol = symtab_content+i;
        char * symbol_name = strtab_content + symbol -> st_name;
        if(strcmp(target_symbol_name,symbol_name)==0){
           target_index =i ; 
           break;
        }
    }
    Elf64_Sym * target_symbol = symtab_content + target_index;
    target_symbol -> st_size = new_symbol_size;
    printf("patching symtab value offset at address 0x%lx\n",symtab_hdr.sh_offset + target_index * sizeof(Elf64_Sym) + offsetof(Elf64_Sym,st_size));
    fseek(f,symtab_hdr.sh_offset + target_index * sizeof(Elf64_Sym),SEEK_SET);
    fwrite(target_symbol,sizeof(Elf64_Sym),1,f);



    Elf64_Sym * dynsym_content = (Elf64_Sym *) read_section(f,&dynsym_hdr);

    num_entries = dynsym_hdr.sh_size / dynsym_hdr.sh_entsize;
    target_index = -1;
    for (int i =0 ; i < num_entries ;i ++){
        Elf64_Sym * symbol = dynsym_content+i;
        char * symbol_name = dynstr_content + symbol -> st_name;
        printf("symbol name = %s\n",symbol_name);
        if(strcmp(target_symbol_name,symbol_name)==0){
           target_index =i ; 
           break;
        }
    }

    target_symbol = dynsym_content + target_index;
    target_symbol -> st_size = new_symbol_size;
    printf("patching symtab value offset at address 0x%lx\n",dynsym_hdr.sh_offset + target_index * sizeof(Elf64_Sym) + offsetof(Elf64_Sym,st_size));
    fseek(f,dynsym_hdr.sh_offset + target_index * sizeof(Elf64_Sym),SEEK_SET);
    fwrite(target_symbol,sizeof(Elf64_Sym),1,f);

    free(strtab_content);
    free(dynstr_content);
    free(shstrtable);

}

int main(int argc, char **argv){



    if(argc != 4){
        printf("Usage: %s <binary path> <kernel_symbnol_name> <new_symbol_size>\n", argv[0]);
        return -1;
    }
    char *binaryPath = argv[1];

    FILE * fp = fopen(binaryPath,"rb+");
    extend_symbol(fp,argv[2],atoi(argv[3]));

    fclose(fp);
}




