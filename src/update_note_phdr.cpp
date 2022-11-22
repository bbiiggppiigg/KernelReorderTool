
#include <elf.h>
#include <vector>
#include <iostream>
#include <cstring>
#include <cassert>
#include <cstdlib>

#define ElfW Elf64_Ehdr
#define Shdr Elf64_Shdr
#define Phdr Elf64_Phdr
using namespace std;

void read_shdr(Shdr * shdr,FILE * f,  ElfW* hdr, int offset){
    fseek(f,hdr->e_shoff + sizeof(Shdr) * offset ,SEEK_SET);
    fread(shdr,sizeof(Shdr),1,f);
}

void read_phdr(Phdr * phdr,FILE * f,  ElfW* hdr, int offset){
    fseek(f,hdr->e_phoff + sizeof(Phdr) * offset ,SEEK_SET);
    fread(phdr,sizeof(Phdr),1,f);
}

char * read_section(FILE * f, Shdr * shdr) {
    int offset = shdr->sh_offset;
    int size = shdr->sh_size;
    fseek(f,offset,SEEK_SET);
    char * ret = (char * ) malloc(size) ;
    fread(ret,size,1,f);
    return ret;
}
void write_section(FILE * f, Shdr * shdr, char * data) {
    int offset = shdr->sh_offset;
    int size = shdr->sh_size;
    fseek(f,offset,SEEK_SET);
    fwrite(data,size,1,f);
}


void update_note_phdr(FILE * f){

    fseek(f,0,SEEK_SET);
    ElfW header;
    fread(&header,sizeof(header),1,f);
    
    Shdr shstrtable_header; 
    read_shdr(&shstrtable_header,f,&header,header.e_shstrndx);
    char * shstrtable = read_section(f,&shstrtable_header);

    Shdr tmp_shdr;
    Shdr note_shdr;
    uint32_t new_offset;
    uint32_t new_size;
    int note_index = -1;
    for (unsigned int i = 1; i < header.e_shnum ; i ++){
        read_shdr(&tmp_shdr,f,&header,i);
        char * sh_name = shstrtable+tmp_shdr.sh_name;
        if(0==strcmp(sh_name,".note")){
            note_index = i ;
            note_shdr = tmp_shdr;
            //printf("found .note section at index = %u\n",i);
            //printf(".note new_offset = %u, new_size = %u\n", note_shdr.sh_offset, note_shdr.sh_size);
            new_offset = note_shdr.sh_offset;
            new_size = note_shdr.sh_size;
        }
    }
    if(note_index == -1)
        return;

    /*text_hdr.sh_size += inc_text_size;
    fseek(f,header.e_shoff + sizeof(Shdr) * text_index ,SEEK_SET);
    fwrite(&text_hdr,sizeof(Shdr),1,f);
    free(shstrtable);*/
    printf("new offset = %u, new size = %u\n", new_offset , new_size); 
    Phdr tmp_phdr;
    for (unsigned int i = 0; i < header.e_phnum ;i ++){
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
    }

}



int main(int argc, char * argv[]){
    FILE * fp = fopen(argv[1],"rw+");
    update_note_phdr(fp);
    fclose(fp);
}
