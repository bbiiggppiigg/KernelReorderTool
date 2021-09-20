#include <msgpack.hpp>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

void update_note_modify_lds_size(const char * filename, const char * new_filename , int32_t new_size)
{

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
    kernarg_list[0].convert(kernarg_list_map);


    /**
     * Now we update the desired field of the map to use the new LDS value
     * And we pack them back into msgpackobject
     *
     */ 
    kernarg_list_map[".group_segment_fixed_size"] = msgpack::object(new_size);
    kernarg_list[0] = msgpack::object(kernarg_list_map,z);
    kvmap["amdhsa.kernels"] = msgpack::object(kernarg_list,z);

    /**
     * Pack the object back to the string format
     */
    msgpack::sbuffer outbuffer;
    msgpack::pack(outbuffer,kvmap);
    std::string out_string = std::basic_string<char>(outbuffer.data(),outbuffer.size());

    
    uint32_t out_offset;
    uint32_t desc_sz = outbuffer.size();
    out_offset = name_sz;

    // Write the headers
    std::ofstream myfile;
    myfile.open(new_filename,std::ios::binary);
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
    if(argc < 4 ){
        puts("Usage : update_note_modify_lds_size <old_note_name> <new_note_name> <new_lds_size>");
        return -1;
    }
    update_note_modify_lds_size(argv[1],argv[2],atoi(argv[3]));
    return 0;    
}
