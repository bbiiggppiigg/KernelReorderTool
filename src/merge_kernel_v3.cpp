#include <elf.h>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <assert.h>
#include <vector>
#include <sys/stat.h>

using namespace std;

// <offset,size> 
//
//

void setup(vector<char * > &names ,vector<long long int> &bundle_size){
    FILE * fmeta = fopen("fmeta_data.txt","r");
    char binary[1000],name[100];
    int total_bundles, offset , size;
    fscanf(fmeta,"%s",binary);
    fscanf(fmeta,"%d",&total_bundles);
    int max_bundle_id = 1, no_cos =0, bundle_id;
    for (int i =0; i < total_bundles; i++){
        printf("i = %d\n",i);
        fscanf(fmeta,"%d %d %s",&offset,&size,name);
        char * del = strchr(name,'-');
        del[0]=0;
        bundle_id = strtol(name,NULL,10);
        char * triple = del+1;
        names.push_back(strdup(triple));
        if(bundle_id > max_bundle_id){
            bundle_size.push_back(no_cos);
            no_cos = 0;
            max_bundle_id = bundle_id;
        }
        no_cos ++;
    }
    bundle_size.push_back(no_cos);
    fclose(fmeta);
}

int align_up(int input, int align){
    return ((input+align-1)/align)* align;
}
int main(int argc, char ** argv){
    vector<char *> names;
    vector<long long int>bundle_size;
    setup(names,bundle_size);
    vector<int> meta_size;
    vector<long long int> real_co_sizes;
    vector<long long int> real_co_addrs;
    int x  =0;
    for(int i =0 ; i < bundle_size.size();  i++){
        //printf("first bundle, %d code-objects\n",bundle_size[i]);
        int my_size = 24 + 8+8*3 * bundle_size[i];
        for(int j =0 ; j < bundle_size[i]; j++){
            puts(names[x]);
            my_size + strlen(names[x]);
            x++;
        }
        meta_size.push_back(my_size);
    }
    char magic[] =  "__CLANG_OFFLOAD_BUNDLE__";
    int write_offset = 0;
    x=0;
    for(int i =0; i < bundle_size.size(); i++){
        //fwrite(magic,1,24,fp);
        //fwrite(&bundle_size[i],1,8,fp);
        write_offset += meta_size[i];
        int addr_offset = write_offset;
        for(int j =0 ; j< bundle_size[i]; j++){
            addr_offset = align_up(addr_offset,0x1000); 
            char path[1000];
            snprintf(path,1000,"code-objects/%d-%s",(i+1),names[x]);
            printf("reading file size from %s\n",path);
            FILE * f_co = fopen(path,"r");
            fseek(f_co,0L,SEEK_END);
            int real_co_size = ftell(f_co);

            printf("aligning address to %x, code_object_size = %x\n",addr_offset,real_co_size);
            real_co_addrs.push_back(addr_offset);
            fclose(f_co);
            addr_offset += real_co_size;
            real_co_sizes.push_back(real_co_size);
            x++;
        }
        write_offset = align_up(addr_offset,0x1000);
    }
    puts("Writing Output");
    FILE * fp = fopen("test_bundle.txt","w");
    x=0;
    int old_x = 0;
    fseek(fp,0L,SEEK_SET);
    int addr_offset =0 ;
    write_offset = 0;
    for(int i =0; i < bundle_size.size(); i++){
        fseek(fp,write_offset,SEEK_SET);
        int curr = ftell(fp);
        printf("currently writing at %x\n",curr);

        fwrite(magic,1,24,fp);
        fwrite(&bundle_size[i],1,8,fp);
        for(int j =0 ; j< bundle_size[i]; j++){
            long long int my_addr = real_co_addrs[x+j]-curr;
            fwrite(&my_addr,1,8,fp);
            fwrite(&real_co_sizes[x+j],1,8,fp);
            long long int triple_len = strlen(names[x+j]);
            fwrite(&triple_len,1,8,fp);
            fwrite(names[x+j],1,triple_len,fp);
            printf("triple = %s, len = %d \n",names[x+j], triple_len);
        }

        for(int j =0 ; j< bundle_size[i]; j++){
            char path[1000];
            void * buffer = malloc(real_co_sizes[x+j]);
            snprintf(path,1000,"code-objects/%d-%s",(i+1),names[x+j]);
            FILE * f_co = fopen(path,"r");
            fseek(f_co,0L,SEEK_SET);
            fread(buffer,1,real_co_sizes[x+j],f_co);
            fclose(f_co);

            fseek(fp,real_co_addrs[x+j],SEEK_SET);
            fwrite(buffer,1,real_co_sizes[x+j],fp);

            addr_offset = real_co_addrs[x+j]+ real_co_sizes[x+j];
        }
        x = x+bundle_size[i]; 
        write_offset = align_up(addr_offset,0x1000);
    }


    fclose(fp);




    /*7
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
    free(co_data);*/
        return 0;
}
