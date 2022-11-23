#include <msgpack.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

void list_args(const char * filename)
{


    size_t len = strlen(filename);
    char newfile_name[len+5];
    sprintf(newfile_name,"%s.new\0",filename);
    /*
     * Step 1, read .note file into buffer
     */
    std::ifstream t(filename);
    std::stringstream buffer;
    buffer << t.rdbuf();
    buffer.seekg(0);
    std::string str(buffer.str());

    std::map<std::string, msgpack::object > kvmap;
    std::vector<msgpack::object> kernarg_list;
    std::map<std::string, msgpack::object> kernarg_list_map;
    std::vector<msgpack::object> arg_list_map;
    msgpack::zone z;

    uint32_t offset =0 ;

    /**
     * Step 2, parse some non-msgpack header data
     * First 4 bytes : Size of the Name str (should be AMDGPU\0)
     * Second 4 bytes: Size of the note in msgpack format
     * Third 4 bytes : Type of the note (should be 32)
     * Followed by the Name str
     * Padding until 4 byte aligned
     * Followed by msgpack data
     */ 
    msgpack::object_handle oh; 
    std::string name_szstr=str.substr(0,4);
    uint32_t name_sz = *( (uint32_t*)  name_szstr.c_str());
    std::string note_type = str.substr(8,4);
    std::string name = str.substr(12,name_sz);
    offset = 12 + name_sz;
    while(offset%4) offset++;

    /*
     * Now we are ready to process the msgpack data
     * We unpack from offset calculated above
     * Unpack until we get to .group_segment_fixed_size
     *
     */
    oh = msgpack::unpack(str.data()+offset, str.size()-offset);
    msgpack::object map_root = oh.get();
    map_root.convert(kvmap);
    kvmap["amdhsa.kernels"].convert(kernarg_list);
    for ( size_t k = 0 ; k < kernarg_list.size() ;k ++){
        kernarg_list[k].convert(kernarg_list_map);
        //std::cout << kernarg_list_map[".args"] << std::endl;
        //std::cout<< "  before convert " << std::endl;
        kernarg_list_map[".args"].convert(arg_list_map);

        size_t i =0 ;
        uint32_t new_offset_loc = 0 ;
        std::string kname;
        kernarg_list_map[".name"].convert(kname); 
        std::map<std::string, msgpack::object> arg_list_ele_map;
        for (  ; i < arg_list_map.size() ;i++){
            //std::cout << arg_list_map[i] << std::endl;    

            arg_list_map[i].convert(arg_list_ele_map);
            uint32_t offset = 0, size = 0;
            arg_list_ele_map[".offset"].convert(offset);
            arg_list_ele_map[".size"].convert(size);
            //std::cout << " offset =  " << offset << " size = " << size << std::endl;
            if(offset + size > new_offset_loc)
                new_offset_loc = offset + size;
        }
        std::cout << kname << " " <<  new_offset_loc << " " << arg_list_map.size() << std::endl;
    }
}

int main(int argc, char * argv[]){
    if(argc < 2){
        printf("usage list_args <name of .note file>\n");
        return -1;
    }
    list_args(argv[1]);
    return 0;    
}
