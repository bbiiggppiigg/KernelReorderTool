#include <elf.h>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <assert.h>
#include <vector>
using namespace std;

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


/* This function will read in an amd fatbin, parse the headers
 * and insert the offset and size of the kernel into a return vector 
 */

void setup(FILE * f, vector<pair<long long int, long long int >> &ret ){
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
        if(0==strcmp(sh_name,".kernel") || 0==strcmp(sh_name,".hip_fatbin")){
            //printf("found !\n");
            fatbin_index = i;
            break;
        }
    }
    if(fatbin_index == -1)
        assert(0 && "failed");
    free(strtable);
    long long int kernel_end = tmp_hdr.sh_offset + tmp_hdr.sh_size;

    long long int current_blob_offset = tmp_hdr.sh_offset;

    FILE * fmeta = fopen("fmeta_data2.txt","w");
    while(current_blob_offset < kernel_end){

        printf("current_blob_offset = %lld kernel_end = %lld\n",current_blob_offset,kernel_end);
        current_blob_offset =  (( current_blob_offset + 7)/8) *8;

        //printf("current blob offset = %d\n",current_blob_offset);
        if (current_blob_offset > kernel_end){
            //printf("kernel end reached\n");
            break;
        }
        fseek(f,current_blob_offset,SEEK_SET);

        int num_bundles ;
        char magic_str[25];
        fread(magic_str,24,1,f);
        fread(&num_bundles,8,1,f);
        char triple[100];
        fwrite(magic_str,24,1,fmeta);
        fwrite(&num_bundles,8,1,fmeta);

        printf("number of bundles: %d\n",num_bundles);

        long long int offset  = 0 ,size = 0 ,triple_size = 0;
        for (unsigned int i =0; i < num_bundles ; i++){
            printf("before reading offset, fp at %x\n", ftell(f));
            fread(&offset,8,1,f);
            fread(&size,8,1,f);
            fread(&triple_size,8,1,f);
            fread(triple,triple_size,1,f);

            fwrite(&offset,8,1,fmeta);
            fwrite(&size,8,1,fmeta);
            fwrite(&triple_size,8,1,fmeta);
            fwrite(&triple,triple_size,1,fmeta);

            printf("triple = %.*s\n",triple_size, triple);
            printf("fp at %x\n",ftell(f));
            printf("\t bundle %d: co_offset = %x fb_offset=%x, size = %x, triple_size = %x, triple = %.*s\n",i,offset,current_blob_offset+offset,size,triple_size,triple_size,triple);
            if(strncmp(triple,"hcc",3)==0 || strncmp(triple,"hip",3)==0){
                ret.push_back(make_pair(current_blob_offset + offset, size));
            }
        }
        current_blob_offset = current_blob_offset + offset + size;
        current_blob_offset = (current_blob_offset+ 4095)/4096 * 4096;
        printf("current_blob_offset = %d\n",current_blob_offset);
    }
    fclose(fmeta);
}

int main(int argc, char ** argv){

    vector<pair<long long int , long long int >> kernels; 
    char * filename;
    if (argc < 2 ){
        assert(0 && "usage : ./split_kernel <filename> ");
    }else{
        filename = argv[1];
    }
    FILE * fmeta = fopen("fmeta_data.txt","w");
    FILE * f = fopen(filename,"rb");
    fprintf(fmeta,"%s\n",filename);
    setup(f,kernels);
    fprintf(fmeta,"%lu\n",kernels.size());
    printf("number of kernels = %lu\n",kernels.size());
    for (int i = 0; i < kernels.size() ; i++){
        fprintf(fmeta,"%lld %lld\n",kernels[i].first,kernels[i].second);
        printf("kernel %d : offset = %x , size = %x\n",i,kernels[i].first, kernels[i].second);
        char * buffer = (char *) malloc(kernels[i].second+1); 
        fseek(f,kernels[i].first,SEEK_SET);
        fread(buffer,kernels[i].second,1,f);

        char tmp_filename[1000];
        sprintf(tmp_filename,"tmp_%02d.hsaco",i);
        FILE * tmpfile = fopen(tmp_filename,"wb");
        fwrite(buffer,kernels[i].second,1,tmpfile);
        fclose(tmpfile);

        free(buffer);
    }
    fclose(fmeta);
    /*
       initscr();
       printw("Hello Wrold !!");
       refresh();
       getch();
       endwin();*/
    return 0;

}
