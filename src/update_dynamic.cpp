
#include <elf.h>
#include <vector>
#include <iostream>
#include <cstring>
#include <cassert>
#include <cstdlib>

#include "kernel_elf_helper.h"
#define ElfW Elf64_Ehdr
#define Shdr Elf64_Shdr
#define Phdr Elf64_Phdr
using namespace std;


void update_text_phdr(FILE * f){

    fseek(f,0,SEEK_SET);
    ElfW header;
    fread(&header,sizeof(header),1,f);
    
    Shdr shstrtable_header; 
    read_shdr(&shstrtable_header,f,&header,header.e_shstrndx);
    char * shstrtable = read_section(f,&shstrtable_header);

    Shdr tmp_shdr;
    Shdr text_shdr;
    Shdr dynamic_shdr;
    Shdr old_text_shdr;
    Shdr old_dynamic_shdr;
    uint32_t new_offset;
    uint32_t new_size;
    int text_index = -1, dynamic_index = -1;
    uint32_t new_dynamic_offset = -1;
    //uint32_t old_text_addr = -1, old_dynamic_addr = -1;
    for (unsigned int i = 1; i < header.e_shnum ; i ++){
        read_shdr(&tmp_shdr,f,&header,i);
        char * sh_name = shstrtable+tmp_shdr.sh_name;
        if(0==strcmp(sh_name,".text")){
            text_index = i ;
            text_shdr = tmp_shdr;
            //printf("found .note section at index = %u\n",i);
            //printf(".note new_offset = %u, new_size = %u\n", note_shdr.sh_offset, note_shdr.sh_size);
            new_offset = text_shdr.sh_offset;
            new_size = text_shdr.sh_size;
        }
        if(0==strcmp(sh_name,".dynamic")){
            dynamic_index = i ;
            dynamic_shdr = tmp_shdr;
            //printf("found .note section at index = %u\n",i);
            //printf(".note new_offset = %u, new_size = %u\n", note_shdr.sh_offset, note_shdr.sh_size);
            new_dynamic_offset = dynamic_shdr.sh_offset;
        }
        if(0==strcmp(sh_name,".old.text")){
            old_text_shdr = tmp_shdr;
        }
        if(0==strcmp(sh_name,".old.dynamic")){
            old_dynamic_shdr = tmp_shdr;
        }


    }
    if(text_index == -1){
        printf("cannnot find text section\n");
        return;
    }

     if(dynamic_index == -1){
        printf("cannnot find dynamic section\n");
        return;
    }
    uint32_t aligned_up = ((new_offset + new_size + 4095)/ 4096) * 4096;
    printf("text_shdr sh_offset = %x, size = %x, aligned up = %x\n", new_offset, new_size ,aligned_up);

    //text_shdr.sh_addr = new_offset;

    /*
    old_text_shdr.sh_offset = new_offset;
    old_text_shdr.sh_name = text_shdr.sh_name;
    old_text_shdr.sh_size = new_size;
    fseek(f,header.e_shoff + sizeof(Shdr) * text_index ,SEEK_SET);
    fwrite(&old_text_shdr,sizeof(Shdr),1,f);
    */


    dynamic_shdr.sh_addr = aligned_up;
    fseek(f,header.e_shoff + sizeof(Shdr) * dynamic_index ,SEEK_SET);
    fwrite(&dynamic_shdr,sizeof(Shdr),1,f);
    

    Phdr tmp_phdr;

    for (unsigned int i = 0; i < header.e_phnum ;i ++){
        read_phdr(&tmp_phdr,f,&header,i);
        if(tmp_phdr.p_offset == new_dynamic_offset){
            tmp_phdr.p_paddr = aligned_up;
            tmp_phdr.p_vaddr = aligned_up;
            fseek(f,header.e_phoff+sizeof(Phdr) * i , SEEK_SET);
                int ret = fwrite(&tmp_phdr,sizeof(Phdr),1,f);
            if(ferror(f))
                puts("ERROR");

        }
    } 

}



int main(int argc, char * argv[]){
    FILE * fp = fopen(argv[1],"rw+");
    update_text_phdr(fp);
    fclose(fp);
}
