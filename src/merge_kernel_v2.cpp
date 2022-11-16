#include <elf.h>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <sys/stat.h>

using namespace std;

// <offset,size> 
int main(int argc, char ** argv){

    vector<uint64_t> kernel_offsets; 
    vector<uint64_t> kernel_sizes; 


    char filename[1000];
    char orig_name[1000];
    char cmd [1000];
    char zeros[4096];
    struct stat sb;
    memset(zeros,0,sizeof(zeros));

    
    FILE * fmeta = fopen("fmeta_data2.txt","r");
    

    FILE * bundle = fopen("bundle.txt","w");
    fseek(fmeta,0L, SEEK_END);
    uint32_t meta2_size = ftell(fmeta);
    fseek(fmeta,0L,SEEK_SET);

    uint64_t num_bundles;

    char magic[25];

    fread(&magic,24,1,fmeta);
    fread(&num_bundles,8,1,fmeta);

    uint64_t co_offset;
    uint64_t co_size;
    uint64_t triple_size;
    char triple[1000];
    FILE * fco;
    uint64_t new_size;
    uint64_t file_offset = 0;
    int64_t write_offset = -1;


    uint32_t hdr_write_ptr  = -1;
    uint32_t data_write_ptr = -1;
    
    uint32_t fatbin_s_addr = 0 ;
    uint64_t meta_size = 24 + 8;
    uint64_t max_size = 0;
    for ( int i =0 ; i < num_bundles ; i++){
        fread(&co_offset,8,1,fmeta);
        fread(&co_size,8,1,fmeta);
        fread(&triple_size,8,1,fmeta);
        fread(&triple,triple_size,1,fmeta);
        meta_size += (8 + 8 + 8 + triple_size);
        if(i ==0 )
            continue;

        
        uint32_t real_co_size;
        sprintf(filename,"tmp_%02d.hsaco",(i-1));
        fco = fopen(filename,"r");
        fseek(fco,0L,SEEK_END);
        real_co_size = ftell(fco);
        fclose(fco);
        kernel_sizes.push_back(real_co_size);
        if(real_co_size > max_size)
            max_size = real_co_size;
    }

    printf("meta size = %x \n",meta_size);
    uint64_t real_co_offset = meta_size;
    for ( int i =0 ; i < num_bundles-1 ; i++){
        real_co_offset = ((  real_co_offset + 4095 )  / 4096) * 4096;
        kernel_offsets.push_back(real_co_offset);
        printf("kernel offset = %x\n", real_co_offset);
        real_co_offset += kernel_sizes[i];
    }

    fwrite(&magic,24,1,bundle);
    fwrite(&num_bundles,8,1,bundle);

    fseek(fmeta,32,SEEK_SET);
    for ( int i =0 ; i < num_bundles ; i++){

        fread(&co_offset,8,1,fmeta);
        fread(&co_size,8,1,fmeta);
        fread(&triple_size,8,1,fmeta);
        fread(&triple,triple_size,1,fmeta);
        new_size = co_size;

        if(i ==0 ){
            fwrite(&co_offset,8,1,bundle);
            fwrite(&co_size,8,1,bundle);
            fwrite(&triple_size,8,1,bundle);
            fwrite(&triple,triple_size,1,bundle);
        }else{
            uint64_t boff;
            boff = ftell(bundle);
            printf("at %x : read offset = %x, write offset = %x\n", boff ,kernel_offsets[i-1], co_offset);
            fwrite(&kernel_offsets[i-1],8,1,bundle);

            boff = ftell(bundle);
            printf("at %x : read size = %x, write size = %x\n", boff, kernel_sizes[i-1], co_size);
            fwrite(&kernel_sizes[i-1],8,1,bundle);
            fwrite(&triple_size,8,1,bundle);
            fwrite(&triple,triple_size,1,bundle);

        }
    }


    uint64_t cur_addr = ftell(bundle);
    void * co_data =  malloc(sizeof(char) * max_size);
    for ( int i =0 ; i < num_bundles-1 ; i++){
        sprintf(filename,"tmp_%02d.hsaco",(i));
        fco = fopen(filename,"r");
        fread(co_data,1, kernel_sizes[i], fco);
        fclose(fco);
        fseek(bundle,kernel_offsets[i],SEEK_SET);
        fwrite(co_data,1,kernel_sizes[i],bundle);
    }
    fclose(bundle);
    free(co_data);
    /*fscanf(fmeta,"%s",orig_name);
    int num_kernels;
    fscanf(fmeta,"%d",&num_kernels);
    for ( int i =0; i < num_kernels; i++){
        long long int offset;
        long long int size ;
        fscanf(fmeta,"%lld %lld\n",&offset,&size);
        kernels.push_back(make_pair(offset,size));
    }
    fclose(fmeta);




    if( argc == 2){
        strncpy(filename,argv[1],1000);    
    }else{
        sprintf(filename,"%s.new",orig_name);
    }
    sprintf(cmd,"cp %s %s",orig_name,filename);



    system(cmd);


    char tmp_filename[1000];

    char * buffer;

    FILE * f = fopen(filename,"rb+");
    printf("number of kernels = %lu\n",kernels.size());
    for (int i = 0; i < kernels.size() ; i++){
        printf("kernel %d : offset = %lld , size = %lld\n",i,kernels[i].first, kernels[i].second);
        buffer = (char *) malloc(kernels[i].second); 
        memset(buffer,0,kernels[i].second);
        sprintf(tmp_filename,"tmp_%02d.hsaco",i);
        FILE * tmpfile = fopen(tmp_filename,"rb");
        fread(buffer,kernels[i].second,1,tmpfile);
        fclose(tmpfile);
        fseek(f,kernels[i].first,SEEK_SET);
        fwrite(buffer,kernels[i].second,1,f);
        free(buffer);
    }
    fclose(f);
    return 0;
*/
    return 0;
}
