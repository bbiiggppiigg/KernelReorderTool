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

typedef struct meta{
    long long int offset;
    long long int size;
    char * name;
}meta;

/* This function will read in an amd fatbin, parse the headers
 * and insert the offset and size of the kernel into a return vector 
 */

void setup(FILE * f, vector<pair<long long int, long long int >> &ret, vector<meta>& metas ){
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
    uint32_t bundle_id=1;
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
        char name[1000];
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
            
            triple[triple_size]='\0';
            snprintf(name,1000,"%d-%s\0",bundle_id,triple);
            printf("triple = %.*s\n",triple_size, triple);
            printf("fp at %x\n",ftell(f));
            printf("\t bundle %d: co_offset = %x fb_offset=%x, size = %x, triple_size = %x, triple = %.*s\n",i,offset,current_blob_offset+offset,size,triple_size,triple_size,triple);
            //if(strncmp(triple,"hcc",3)==0 || strncmp(triple,"hip",3)==0){
                ret.push_back(make_pair(current_blob_offset + offset, size));
                metas.push_back({current_blob_offset+offset,size,strdup(name)});
            //}
        }
        current_blob_offset = current_blob_offset + offset + size;
        current_blob_offset = (current_blob_offset+ 4095)/4096 * 4096;
        printf("current_blob_offset = %d\n",current_blob_offset);
        bundle_id++;
    }
    fclose(fmeta);
}

int main(int argc, char ** argv){

    vector<pair<long long int , long long int >> kernels; 
    vector<meta>metas; 
    char * filename;
    if (argc < 2 ){
        assert(0 && "usage : ./split_kernel <filename> ");
    }else{
        filename = argv[1];
    }
    FILE * fmeta = fopen("fmeta_data.txt","w");
    FILE * f = fopen(filename,"rb");
    fprintf(fmeta,"%s\n",filename);
    setup(f,kernels,metas);
    fprintf(fmeta,"%lu\n",kernels.size());
    char path[1000];
    for (auto & meta : metas){
        fprintf(fmeta,"%lld %lld %s\n",meta.offset,meta.size,meta.name);
        printf("code object %s : offset = %x , size = %x\n",meta.name,meta.offset,meta.size);
        char * buffer = (char *) malloc(meta.size+1); 
        fseek(f,meta.offset,SEEK_SET);
        fread(buffer,meta.size,1,f);
        snprintf(path,1000,"code-objects/%s\0",meta.name);
        FILE * tmpfile = fopen(path,"wb");
        fwrite(buffer,meta.size,1,tmpfile);
        fclose(tmpfile);
        free(buffer);
        printf("freeing buffer\n");
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
