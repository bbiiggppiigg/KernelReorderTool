#include <msgpack.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
//#include "INIReader.h"

void expand_args(const char * filename)
{

    /*INIReader reader("config.ini");
    auto &sections = reader.Sections();
    for (auto & section : sections){
    
    }*/
    size_t len = strlen(filename);
    char newfile_name[len+20];
    sprintf(newfile_name,"%s.expanded.new",filename);
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

    for ( uint32_t k_list_i = 0 ; k_list_i < kernarg_list.size(); k_list_i++){
        kernarg_list[k_list_i].convert(kernarg_list_map);
        uint32_t old_kernarg_size = 0;
        //size_t i =0 ;
        old_kernarg_size = kernarg_list_map[".kernarg_segment_size"].convert(old_kernarg_size);
        kernarg_list_map[".args"].convert(arg_list_map);
        //int old_kernarg_num  = arg_list_map.size();
        std::cout<< old_kernarg_size << " " << arg_list_map.size() << std::endl;
        uint32_t new_kernarg_size = old_kernarg_size;
        if(old_kernarg_size % 8)
            new_kernarg_size = ((old_kernarg_size >>3 ) +1)<<3;
        std::map<std::string, msgpack::object> newarg;
        newarg[".address_space"]= msgpack::object( std::string("global"),z);
        newarg[".offset"]= msgpack::object( new_kernarg_size,z);
        newarg[".size"]= msgpack::object( 8,z);
        newarg[".value_kind"]= msgpack::object( std::string("global_buffer"),z);
        arg_list_map.push_back(msgpack::object(newarg,z)); 
        new_kernarg_size = new_kernarg_size+ 8;
        std::map<std::string, msgpack::object> newarg2;
        for (int xx =0 ; xx < 5 ; xx++){
            newarg2[".offset"]= msgpack::object( new_kernarg_size,z);
            newarg2[".size"]= msgpack::object( 4,z);
            newarg2[".value_kind"]= msgpack::object( std::string("by_value"),z);
            arg_list_map.push_back(msgpack::object(newarg2,z)); 
            new_kernarg_size = new_kernarg_size+ 4;
        }
        kernarg_list_map[".args"] = msgpack::object(arg_list_map,z);
        kernarg_list_map[".kernarg_segment_size"] = msgpack::object(new_kernarg_size,z);
        kernarg_list[k_list_i] = msgpack::object(kernarg_list_map,z);

    }
    /*kernarg_list[0].convert(kernarg_list_map);
    std::cout << kernarg_list_map[".args"] << std::endl;
    std::cout<< "  before convert " << std::endl;
    kernarg_list_map[".args"].convert(arg_list_map);

    uint32_t old_kernarg_size = 0;
    old_kernarg_size = kernarg_list_map[".kernarg_segment_size"].convert(old_kernarg_size);
    std::cout<< old_kernarg_size << " " << arg_list_map.size() << std::endl;
    int old_kernarg_num  = arg_list_map.size();
    size_t i =0 ;
    
    uint32_t new_kernarg_size = old_kernarg_size;
    if(old_kernarg_size % 8)
        new_kernarg_size = ((old_kernarg_size >>3 ) +1)<<3;

    std::map<std::string, msgpack::object> newarg;
    newarg[".address_space"]= msgpack::object( std::string("global"),z);
    newarg[".offset"]= msgpack::object( new_kernarg_size,z);
    newarg[".size"]= msgpack::object( 8,z);
    newarg[".value_kind"]= msgpack::object( std::string("global_buffer"),z);
    arg_list_map.push_back(msgpack::object(newarg,z)); 
    new_kernarg_size = new_kernarg_size+ 8;

    std::map<std::string, msgpack::object> newarg2;
    for (int xx =0 ; xx < 5 ; xx++){
        newarg2[".offset"]= msgpack::object( new_kernarg_size,z);
        newarg2[".size"]= msgpack::object( 4,z);
        newarg2[".value_kind"]= msgpack::object( std::string("by_value"),z);
        arg_list_map.push_back(msgpack::object(newarg2,z)); 
        new_kernarg_size = new_kernarg_size+ 4;
    }


    kernarg_list_map[".args"] = msgpack::object(arg_list_map,z);
    kernarg_list_map[".kernarg_segment_size"] = msgpack::object(new_kernarg_size,z);
    kernarg_list[0] = msgpack::object(kernarg_list_map,z);
    */
    kvmap["amdhsa.kernels"] = msgpack::object(kernarg_list,z);
    msgpack::sbuffer outbuffer;
    msgpack::pack(outbuffer,kvmap);
    std::string out_string = std::basic_string<char>(outbuffer.data(),outbuffer.size());

    
    uint32_t out_offset;
    uint32_t desc_sz = outbuffer.size();
    out_offset = name_sz;

    // Write the headers
    std::ofstream myfile;
    myfile.open(newfile_name,std::ios::binary);
    myfile.write(reinterpret_cast<char *>(&name_sz),sizeof(name_sz));
    myfile.write(reinterpret_cast<char *>(&desc_sz),sizeof(desc_sz));
    myfile.write(note_type.c_str(),note_type.size());
    myfile.write(name.c_str(),name.size());
    // Padding
    while(out_offset %4){
        myfile.put('\0');    
        out_offset +=1;
    }
    // Write the msgpack data
    myfile << out_string ;
    out_offset += out_string.size();
    
    // Padding 
    while(out_offset %4){
        myfile.put('\0');    
        out_offset +=1;
    }

   
    myfile.close();
    
}

int main(int argc, char * argv[]){
    if(argc < 2){
        printf("usage expand_args <name of .note file>\n");
        return -1;
    }
    expand_args(argv[1]);
    return 0;    
}
