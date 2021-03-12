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



void extend_text(FILE * f){
    ElfW header;
    fread(&header,sizeof(header),1,f);

    Shdr shstrtable_header; 
    read_shdr(&shstrtable_header,f,&header,header.e_shstrndx);
    char * shstrtable = read_section(f,&shstrtable_header);

    Shdr tmp_hdr;

    Shdr text_hdr;
    Shdr symtab_hdr;
    Shdr strtab_hdr;

    int text_index = -1;
    int symtab_index = -1;
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

    }



    int text_size = text_hdr.sh_size;
    int text_offset = text_hdr.sh_offset;
    int text_new_size = (text_size & 0x1000);
    if (text_size | 0xfff)
        text_new_size += 0x1000;

    text_hdr.sh_size = text_new_size;
    printf("setting sh_size to %d\n",text_new_size);
    fseek(f,header.e_shoff + sizeof(Shdr) * text_index ,SEEK_SET);
    fwrite(&text_hdr,sizeof(Shdr),1,f);

    char * strtab_content = read_section(f,&strtab_hdr);
    Elf64_Sym * symtab_content = (Elf64_Sym *) read_section(f,&symtab_hdr);

    int num_entries = symtab_hdr.sh_size / symtab_hdr.sh_entsize;
    int target_index = -1;
    for (int i =0 ; i < num_entries ;i ++){
        Elf64_Sym * symbol = symtab_content+i;
        char * symbol_name = strtab_content + symbol -> st_name;
        if(strcmp("_Z9mmmKernelP15HIP_vector_typeIfLj4EES1_S1_jj",symbol_name)==0){
           target_index =i ; 
           break;
        }
    }
    Elf64_Sym * target_symbol = symtab_content + target_index;
    target_symbol -> st_size = 0x1000;
    fseek(f,symtab_hdr.sh_offset + target_index * sizeof(Elf64_Sym),SEEK_SET);
    fwrite(target_symbol,sizeof(Elf64_Sym),1,f);


}

int main(int argc, char **argv){



    if(argc != 2){
        printf("Usage: %s <binary path>\n", argv[0]);
        return -1;
    }
    char *binaryPath = argv[1];

    FILE * fp = fopen(binaryPath,"rb+");
    extend_text(fp);
    fclose(fp);
}




