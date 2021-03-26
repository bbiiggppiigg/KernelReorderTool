#include <elf.h>
#include <vector>
#include <iostream>
#include <cstring>
#include <cassert>

#define WAIT_CONST 14

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



void extend_text(FILE * f, int new_text_size){
    ElfW header;
    fread(&header,sizeof(header),1,f);

    

    Shdr shstrtable_header; 
    read_shdr(&shstrtable_header,f,&header,header.e_shstrndx);
    char * shstrtable = read_section(f,&shstrtable_header);

    Shdr tmp_hdr;
    Shdr text_hdr;
    
    int text_index = -1;
    for (unsigned int i = 1; i < header.e_shnum ; i ++){
        read_shdr(&tmp_hdr,f,&header,i);
        char * sh_name = shstrtable+tmp_hdr.sh_name;
        if(0==strcmp(sh_name,".text")){
            text_index = i ;
            text_hdr = tmp_hdr;
        }
    }

    text_hdr.sh_size = new_text_size;
    printf("patching text_size in section header at 0x%lx to %d\n",header.e_shoff+sizeof(Shdr)*text_index,new_text_size);
    fseek(f,header.e_shoff + sizeof(Shdr) * text_index ,SEEK_SET);
    fwrite(&text_hdr,sizeof(Shdr),1,f);
    free(shstrtable);
    
    
    Phdr tmp_phdr;
    for (unsigned int i = 0; i < header.e_phnum ;i ++){
        read_phdr(&tmp_phdr,f,&header,i);
        if(tmp_phdr.p_offset == text_hdr.sh_addr){
            tmp_phdr.p_filesz = new_text_size;
            tmp_phdr.p_memsz = new_text_size;
            fseek(f,header.e_phoff+sizeof(Phdr) * i , SEEK_SET);
            fwrite(&tmp_phdr,sizeof(Phdr),1,f);

            printf("patching text_size in program header at 0x%lx to %d\n",header.e_phoff+sizeof(Phdr)*i,new_text_size);
            break;
        }
    }


}

int main(int argc, char **argv){



    if(argc != 3){
        printf("Usage: %s <binary path> <new_text_section_size>\n", argv[0]);
        return -1;
    }
    char *binaryPath = argv[1];

    FILE * fp = fopen(binaryPath,"rb+");
    extend_text(fp,atoi(argv[2]));

    fclose(fp);
}




