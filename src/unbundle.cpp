#include <stdio.h>
#include <stdlib.h>
#include <cstring>
#include <cstdint>
#include <utility>
#include <string>
#include <vector>
class meta_data{
public:
  meta_data(uint64_t offset, uint64_t size, uint64_t id_length, char * id, int index) : _offset(offset), _size(size), _id(std::move(std::to_string(index)+std::string("-")+std::string(id,id_length)))
  {
      
  }
  uint64_t _offset;
  uint64_t _size;
  std::string _id;
};

int main(int argc, char * argv[]){
  if( argc !=2 ){
    printf("Usage : unbundle.exe <clang-offload-bundle-file>\n");
    exit(-1);
  }
  FILE * fp = fopen(argv[1],"r");
  if(!fp){
    printf("Failed to open file %s\n",argv[1]);
  }
  char tmp_buffer[1024];

  std::vector<meta_data> meta_datas;
  uint64_t max_co_size = 0;
  uint32_t bundle_index = 1;
  uint64_t bundle_start_offset = 0;
  while(1){
    uint64_t cur_bundle_offset = 0;
    printf("Current bundle offset is = %p %lld\n",bundle_start_offset,bundle_start_offset);
    fseek(fp,bundle_start_offset,SEEK_SET);
    int ret = fread(tmp_buffer,1,24,fp);
    if (ret ==0 || strncmp(tmp_buffer,"__CLANG_OFFLOAD_BUNDLE__",24)!=0){
      printf("Magic number doesn't match, got %s ret = %d\n",tmp_buffer,ret);
      break;
    }
    uint64_t num_bundles;
    fread(&num_bundles,1,8,fp);
    printf("Index = %d, Number of bundles %lld\n",bundle_index,num_bundles);

    uint64_t code_object_offset, code_object_size, id_length;
    void *bundle_id; 
    char *co_output_name;
    // Read metadata for each bundle
    for(uint64_t cur_bundle_id = 0 ; cur_bundle_id < num_bundles; cur_bundle_id++){
      fread(&code_object_offset,1,8,fp);
      fread(&code_object_size,1,8,fp);
      fread(&id_length,1,8,fp);
      bundle_id = malloc(sizeof(char) * id_length+1);
      fread(bundle_id,1,id_length,fp);
      ((char *)bundle_id)[id_length]='\0';
      printf("code object %s, offset = %lld, size = %lld\n",
        bundle_id,code_object_offset,code_object_size);
      if (code_object_size > max_co_size)
        max_co_size = code_object_size;
      //co_output_name = (char *) malloc(sizeof(char) * (id_length+10));
      //snprintf(co_output_name,id_length+10,"%s.hsaco",bundle_id);
      meta_datas.push_back(meta_data(code_object_offset,code_object_size,id_length, (char *) bundle_id, bundle_index));
      free(bundle_id); 
      if (code_object_size + code_object_offset > cur_bundle_offset)
        cur_bundle_offset = code_object_size + code_object_offset;
    }
    bundle_start_offset += ((cur_bundle_offset + 4095) / 4096) * 4096;
    bundle_index += 1;
  }
  char * co_buffer;
  co_buffer = (char *) malloc(max_co_size);
  // Extract each co into its own file
  for(meta_data md : meta_datas){
    std::string output_file_str = md._id+std::string(".hsaco"); 
    fseek(fp,md._offset,SEEK_SET);
    fread(co_buffer,1,md._size,fp);
    FILE * fout = fopen(output_file_str.c_str(),"w");
    fwrite(co_buffer,1,md._size,fout);
    fclose(fout);
  }
  return 0;
}

