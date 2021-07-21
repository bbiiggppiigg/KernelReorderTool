#include "kernel_elf_helper.h"
void extend_text(FILE * f, int new_text_size);
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




