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
    Shdr symtab_shdr;
    Shdr dynsym_shdr;
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
        if(0==strcmp(sh_name,".symtab")){
            symtab_shdr = tmp_shdr;
        }
        if(0==strcmp(sh_name,".dynsym")){
            dynsym_shdr = tmp_shdr;
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


    //text_shdr.sh_addr = new_offset;
    uint32_t old_offset = old_text_shdr.sh_offset;
    old_text_shdr.sh_offset = new_offset;
    old_text_shdr.sh_name = text_shdr.sh_name;
    old_text_shdr.sh_size = new_size;
    fseek(f,header.e_shoff + sizeof(Shdr) * text_index ,SEEK_SET);
    fwrite(&old_text_shdr,sizeof(Shdr),1,f);

    char * symtab_section = read_section(f,&symtab_shdr);
    int num_entries = symtab_shdr.sh_size / symtab_shdr.sh_entsize;
    for( int i =0; i < num_entries ; i++){
        Elf64_Sym * symbol = ((Elf64_Sym *) symtab_section + i );
        unsigned char bind = ELF64_ST_BIND(symbol->st_info);
        unsigned char type = ELF64_ST_TYPE(symbol->st_info);
        if(type == STT_FUNC){
            printf(" Found STT_FUNC , VALUE = 0x%lx, size = %lu\n",symbol->st_value, symbol->st_size);
            symbol->st_value = symbol->st_value - old_offset + new_offset;
            fseek(f,symtab_shdr.sh_offset + i * sizeof(Elf64_Sym),SEEK_SET);
            fwrite(symbol,sizeof(Elf64_Sym),1,f);
        }
    }
    
    char * dynsym_section = read_section(f,&dynsym_shdr);
    num_entries = dynsym_shdr.sh_size / dynsym_shdr.sh_entsize;
    for( int i =0; i < num_entries ; i++){
        Elf64_Sym * symbol = ((Elf64_Sym *) dynsym_section + i );
        unsigned char bind = ELF64_ST_BIND(symbol->st_info);
        unsigned char type = ELF64_ST_TYPE(symbol->st_info);
        if(type == STT_FUNC){
            printf(" Found STT_FUNC , VALUE = 0x%lx, size = %lu\n",symbol->st_value, symbol->st_size);
            symbol->st_value = symbol->st_value - old_offset + new_offset;
            fseek(f,dynsym_shdr.sh_offset + i * sizeof(Elf64_Sym),SEEK_SET);
            fwrite(symbol,sizeof(Elf64_Sym),1,f);
        }
    }





    //dynamic_shdr.sh_addr = new_dynamic_offset;
    /*
    old_dynamic_shdr.sh_offset = new_dynamic_offset;
    old_dynamic_shdr.sh_name = dynamic_shdr.sh_name;
    fseek(f,header.e_shoff + sizeof(Shdr) * dynamic_index ,SEEK_SET);
    fwrite(&old_dynamic_shdr,sizeof(Shdr),1,f);
    */



    printf("new offset = %u, new size = %u\n", new_offset , new_size); 
    Phdr tmp_phdr;
    uint32_t load_count = 0;
    long load_1 , load_2 , load_3;
    load_1 = load_2 = load_3 = -1;
    for (unsigned int i = 0; i < header.e_phnum ;i ++){
        read_phdr(&tmp_phdr,f,&header,i);
        if(tmp_phdr.p_type == PT_LOAD){
            load_count ++ ;
            if( load_2 > load_1){
                if ( tmp_phdr.p_offset > load_2){
                    load_3 = tmp_phdr.p_offset;
                }
            }
            if( load_2 == load_1 && load_1 > -1){
                if ( tmp_phdr.p_offset > load_1){
                    load_2 = tmp_phdr.p_offset;
                }
            }

            if(load_1 == -1){
                load_1 = load_2 = load_3 = tmp_phdr.p_offset;
            }
        }
    } 
    if(load_count != 3){
        printf("ERR : load _count = %d\n",load_count);
    }
    printf("load addrs = %lx %lx %lx\n",load_1,load_2,load_3);
    printf("new text addr = %x, new dynamic addr = %x\n",new_offset, new_dynamic_offset);
    for (unsigned int i = 0; i < header.e_phnum ;i ++){
        read_phdr(&tmp_phdr,f,&header,i);
        if(tmp_phdr.p_type == PT_LOAD && tmp_phdr.p_offset == load_2){
            tmp_phdr.p_offset = new_offset;
            tmp_phdr.p_filesz = new_size;
            tmp_phdr.p_memsz = new_size;
            //tmp_phdr.p_paddr = new_offset;
            //tmp_phdr.p_vaddr = new_offset;
            fseek(f,header.e_phoff+sizeof(Phdr) * i , SEEK_SET);
                int ret = fwrite(&tmp_phdr,sizeof(Phdr),1,f);
            if(ferror(f))
                puts("ERROR");

        }
    } 
    /*for (unsigned int i = 0; i < header.e_phnum ;i ++){
        read_phdr(&tmp_phdr,f,&header,i);
        if(tmp_phdr.p_type == PT_NOTE){
            printf("found program header of type PT_NOTE, index = %u\n", i);
            printf("p offset = %u , paddr = %u , p vaddr = %u\n", tmp_phdr.p_offset , tmp_phdr.p_paddr, tmp_phdr.p_vaddr);
            tmp_phdr.p_filesz = new_size;
            tmp_phdr.p_memsz = new_size;
            tmp_phdr.p_offset = new_offset;
            tmp_phdr.p_paddr = new_offset;
            tmp_phdr.p_vaddr = new_offset;
            fseek(f,header.e_phoff+sizeof(Phdr) * i , SEEK_SET);
            int ret = fwrite(&tmp_phdr,sizeof(Phdr),1,f);
            if(ferror(f))
                puts("ERROR");
            printf("patching offset in program header at 0x%lx to %d, ret value = %d\n",header.e_phoff+sizeof(Phdr)*i,new_offset,ret);
            break;
        }else{
            printf("p offset = %u , paddr = %u , p vaddr = %u\n", tmp_phdr.p_offset , tmp_phdr.p_paddr, tmp_phdr.p_vaddr);
        }
    }*/

}



int main(int argc, char * argv[]){
    assert(argc > 1);
    FILE * fp = fopen(argv[1],"rw+");
    update_text_phdr(fp);
    fclose(fp);
}
