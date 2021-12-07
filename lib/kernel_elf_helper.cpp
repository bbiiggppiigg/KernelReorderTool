
#include <elf.h>
#include <vector>
#include <iostream>
#include <cstring>
#include <cassert>
#include <msgpack.hpp>
#include <cstdlib>
#include "kernel_elf_helper.h"

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
void write_section(FILE * f, Shdr * shdr, char * data) {
    int offset = shdr->sh_offset;
    int size = shdr->sh_size;
    fseek(f,offset,SEEK_SET);
    fwrite(data,size,1,f);
}

void update_lds_usage(FILE * f, uint32_t new_usage){

    fseek(f,0,SEEK_SET);
    ElfW header;
    fread(&header,sizeof(header),1,f);
    
    Shdr shstrtable_header; 
    read_shdr(&shstrtable_header,f,&header,header.e_shstrndx);
    char * shstrtable = read_section(f,&shstrtable_header);

    Shdr tmp_hdr;
    Shdr note_hdr;
    
    int note_index = -1;
    for (unsigned int i = 1; i < header.e_shnum ; i ++){
        read_shdr(&tmp_hdr,f,&header,i);
        char * sh_name = shstrtable+tmp_hdr.sh_name;
        if(0==strcmp(sh_name,".note")){
            note_index = i ;
            note_hdr = tmp_hdr;
            printf("found .note, index =%d\n",note_index);
            break;
        }
    }
    if(note_index == -1){
        puts("CAN'T FIND NOTE SECTION");
        exit(-1);
    }

    char * shnote = read_section(f,&note_hdr);
    //fwrite(shnote,note_hdr.sh_size,1,stdout);
    char pattern[] = ".group_segment_fixed_size";
    char * location = (char * )  memmem(shnote,note_hdr.sh_size,pattern,strlen(pattern));
    if(location){
        uint8_t * update_ptr = (uint8_t * ) (location+strlen(pattern));
        *update_ptr = new_usage;
        printf("updateing lds usage in note at address %lx to %d\n",note_hdr.sh_offset + location - shnote + strlen(pattern), new_usage);
        write_section(f,&note_hdr,shnote);
    }else{
        puts("Can't find gg");    
    }
    //fwrite(shnote,1,0x468,stdout);
    //fseek(f,header.e_shoff + sizeof(Shdr) * text_index ,SEEK_SET);
    //fwrite(&text_hdr,sizeof(Shdr),1,f);
    free(shstrtable);
    
   /* 
    Phdr tmp_phdr;
    for (unsigned int i = 0; i < header.e_phnum ;i ++){
        read_phdr(&tmp_phdr,f,&header,i);
        if(tmp_phdr.p_offset == note_hdr.sh_addr){
            printf("patching text_size in program header at 0x%lx to %d\n",header.e_phoff+sizeof(Phdr)*i,new_usage);
            break;
        }
    }*/
}

void set_lds_usage(FILE* fp, uint32_t kd_offset , uint32_t new_lds_usage ){
    uint32_t old_value ;
    fseek(fp,kd_offset,SEEK_SET);
    fread(&old_value,1,sizeof(old_value),fp);
    assert(old_value <= new_lds_usage );
    fseek(fp,kd_offset,SEEK_SET);
    fwrite(&new_lds_usage,1,sizeof(new_lds_usage),fp);
}
// ONLY WORKS FOR GFX9 NOW
void set_sgpr_vgpr_usage(FILE * fp , uint32_t kd_offset , uint32_t sgpr_usage ,uint32_t vgpr_usage ){

    assert(vgpr_usage < 256);
    assert(sgpr_usage < 112);

    uint32_t vgpr_bits = vgpr_usage / 4 ;
    if(vgpr_usage %4)
        vgpr_usage += 1;
    vgpr_usage -=1;
    if(vgpr_usage < 0)
        vgpr_usage =0 ;
    uint32_t sgpr_bits = sgpr_usage / 16;
    if(sgpr_bits%16)
        sgpr_bits +=1;
    sgpr_bits -=1;
    if(sgpr_bits <0)
        sgpr_bits = 0;
    sgpr_bits *= 2;
    uint32_t old_bits = 0;
    fseek(fp,kd_offset+0x30,SEEK_SET);
    fread(&old_bits,1,sizeof(old_bits),fp);
    uint32_t old_sgpr_bits, old_vgpr_bits;
    old_sgpr_bits = (old_bits & 0x3c0) >> 6;
    old_vgpr_bits = old_bits & 0x3f;
    assert(old_sgpr_bits <= sgpr_bits);
    assert(old_vgpr_bits <= vgpr_bits);
    
    uint32_t new_mask =  ( 0xaf <<16 ) | ( sgpr_bits << 6 ) | vgpr_bits;
    uint32_t new_bits = (old_bits & 0xc00) | new_mask;

    fseek(fp,kd_offset+0x30,SEEK_SET);
    fwrite(&new_bits,1,sizeof(new_bits),fp);
    printf("old bits = %x, new bits = %x, writing to address %x\n",old_bits,new_bits,  kd_offset+0x30);

}



void extend_text(FILE * f, int new_text_size){

    fseek(f,0,SEEK_SET);
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


void get_kds(FILE * f, vector<pair<uint64_t, string>> & ret){
    fseek(f,0,SEEK_SET);
    ElfW header;
    fread(&header,sizeof(header),1,f);

    Shdr shstrtable_header; 
    read_shdr(&shstrtable_header,f,&header,header.e_shstrndx);
    char * shstrtable = read_section(f,&shstrtable_header);

    Shdr tmp_hdr;
    int symtab_index = -1;
    int strtab_index = -1;
    for (unsigned int i = 1; i < header.e_shnum ; i ++){
        read_shdr(&tmp_hdr,f,&header,i);
        char * sh_name = shstrtable+tmp_hdr.sh_name;
        if(0==strcmp(sh_name,".symtab")){
            symtab_index = i;
        }
        if(0==strcmp(sh_name,".strtab")){
            strtab_index = i;
        }

    }
    if(symtab_index == -1 || strtab_index == -1)
        assert(0 && "failed");



    Shdr strtab_header;
    read_shdr(&strtab_header,f,&header,strtab_index);
    char * strtab_section = read_section(f,&strtab_header);


    Shdr symtab_header;
    read_shdr(&symtab_header,f,&header,symtab_index);
    char * symtab_section = read_section(f,&symtab_header);
    int num_entries = symtab_header.sh_size / symtab_header.sh_entsize;
    printf("entsize = %lu\n",symtab_header.sh_entsize);
    printf("number of entries = %d\n",num_entries);
    for (int i =0; i < num_entries ;i ++){
        Elf64_Sym * symbol = ((Elf64_Sym *) symtab_section)+i;
        char * symbol_name = strtab_section + symbol->st_name;
        int name_len = strlen(symbol_name);
        if( strncmp(symbol_name+name_len-3,".kd",3)==0){
            ret.push_back( make_pair(symbol->st_value,string(symbol_name)));
        }

    } 

    free(shstrtable);
    free(strtab_section);
    free(symtab_section);
}

#define bits_mask(up,low) ( 1<<(up - low +1 )-1 ) 

uint32_t get_bits(uint32_t value, uint32_t hi , uint32_t low){
    if (hi < low)
        std::swap(hi,low);

    return (value & (( (1 << ( hi - low + 1 ) ) - 1 ) << low ) ) >> low;

}

uint32_t set_bits(uint32_t old_bits, uint32_t hi, uint32_t low, uint32_t bits){
    uint32_t mask = ~((( 1 << (hi-low + 1))-1)<< low);

    printf("setting bits %x , mask = %x\n", bits,mask);
    uint32_t ret = old_bits & mask;
    ret |= (bits << low );
    return ret;
}

uint32_t get_pgm_rsrc1 ( FILE* fp ,uint32_t kd_offset ){
    uint32_t ret;
    fseek(fp,kd_offset + 0x30 , SEEK_SET);
    fread((void *) &ret,4,1,fp);
    return ret;
}

void set_pgm_rsrc1 ( FILE* fp ,uint32_t kd_offset , uint32_t pgm_rsrc1_bits ){
    fseek(fp,kd_offset + 0x30 , SEEK_SET);
    fwrite((void *) &pgm_rsrc1_bits,4,1,fp);
}

uint32_t sgpr_count_to_bits(uint32_t count){
    uint32_t ret = count / 8;
    if(count %8) ret += 1;
    ret -=1;
    if(ret < 0)
        ret = 0;
    return ret;
}

uint32_t sgpr_bits_to_count(uint32_t bits){
    uint32_t ret = bits+1; 
    return ret * 8;
}
uint32_t vgpr_bits_to_count(uint32_t bits){
    uint32_t ret = bits+1; 
    return ret * 4;
}

void set_sgpr_usage( FILE* fp , vector<pair <uint64_t,string>> & kds ,string name, uint32_t new_sgpr_count ){
    printf("looking for name %s\n",name.c_str());
    for (auto &p : kds ){
        if(p.second == name ){
            printf("preparing to set new count \n");
            uint32_t old_bits = get_pgm_rsrc1(fp,p.first);
            uint32_t sgpr_count = sgpr_bits_to_count(get_bits(old_bits,9,6));
            if(sgpr_count > new_sgpr_count ){
                puts("Can't lower the sgpr count ");
                return;    
            }
            uint32_t new_bits = set_bits(old_bits,9,6,sgpr_count_to_bits(new_sgpr_count));
            printf("old bits = %x , new bits = %x\n",old_bits,new_bits);
            printf("old count = %u\n",sgpr_count);
            set_pgm_rsrc1(fp,p.first,new_bits);
            uint32_t written_bits = get_pgm_rsrc1(fp,p.first);
            printf("reading own write, bits = %x\n",written_bits);
            return;
        }    
    } 
    printf("GGGGGGGGGGGGGGG!\n");
}




/* Update the symbol of labels that is after the insertion point such that llvm-objdump works correctly
 */

void update_symtab_symbols(FILE * f, uint32_t text_start , uint32_t text_end , uint32_t insert_loc , uint32_t insert_size){
	ElfW header;
    fseek(f,0,SEEK_SET);
	fread(&header,sizeof(header),1,f);

	Shdr shstrtable_header;
	read_shdr(&shstrtable_header,f,&header,header.e_shstrndx);
	char * shstrtable = read_section(f,&shstrtable_header);

	Shdr tmp_hdr;

	//Shdr text_hdr;
	Shdr symtab_hdr;
	//Shdr strtab_hdr;
	//Shdr dynsym_hdr;
	//Shdr dynstr_hdr;

	//int text_index = -1;
	int symtab_index = -1;
	for (unsigned int i = 1; i < header.e_shnum ; i ++){
		read_shdr(&tmp_hdr,f,&header,i);
		char * sh_name = shstrtable+tmp_hdr.sh_name;
		if(0==strcmp(sh_name,".symtab")){
			symtab_hdr = tmp_hdr;
            printf("found symtab at index %d\n",i);
			symtab_index = i;
		}

	}
    if(symtab_index == -1){
        printf("ERROR ! Can't Find Symtab\n");
        exit(-1);
    }

	//char * strtab_content = read_section(f,&strtab_hdr);
	//char * dynstr_content = read_section(f,&dynstr_hdr);


	Elf64_Sym * symtab_content = (Elf64_Sym *) read_section(f,&symtab_hdr);
    printf("symtab hdr sh_size : %ld, .ent_size = %ld\n",symtab_hdr.sh_size,symtab_hdr.sh_entsize);
	int num_entries = symtab_hdr.sh_size / symtab_hdr.sh_entsize;
	//int target_index = -1;
	for (int i =0 ; i < num_entries ;i ++){
		Elf64_Sym * symbol = symtab_content+i;
		//char * symbol_name = strtab_content + symbol -> st_name;
        //puts(symbol_name);
		if( text_start < symbol->st_value && symbol->st_value < text_end ){
			if(symbol->st_value >= insert_loc){
				symbol->st_value += insert_size;
                //printf("writing to address %x\n",symtab_hdr.sh_offset +i * sizeof(Elf64_Sym));
				fseek(f,symtab_hdr.sh_offset + i * sizeof(Elf64_Sym),SEEK_SET);
				fwrite(symbol,sizeof(Elf64_Sym),1,f);
			}
				
		}
	}

}


/* Update the symbol of labels that is after the insertion point such that llvm-objdump works correctly
 */

void update_symtab_symbol_size(FILE * f, const char kernel_name [] , uint32_t new_size){
	ElfW header;
    fseek(f,0,SEEK_SET);
	fread(&header,sizeof(header),1,f);

	Shdr shstrtable_header;
	read_shdr(&shstrtable_header,f,&header,header.e_shstrndx);
	char * shstrtable = read_section(f,&shstrtable_header);

	Shdr tmp_hdr;

	Shdr text_hdr;
	Shdr symtab_hdr;
	Shdr strtab_hdr;
	Shdr dynsym_hdr;
	Shdr dynstr_hdr;

	//int text_index = -1;
	//int symtab_index = -1;
	for (unsigned int i = 1; i < header.e_shnum ; i ++){
		read_shdr(&tmp_hdr,f,&header,i);
		char * sh_name = shstrtable+tmp_hdr.sh_name;
		if(0==strcmp(sh_name,".text")){
	//		text_index = i ;
			text_hdr = tmp_hdr;
		}
		if(0==strcmp(sh_name,".symtab")){
			symtab_hdr = tmp_hdr;
	//		symtab_index = i;
		}
		if(0==strcmp(sh_name,".strtab")){
			strtab_hdr = tmp_hdr;
		}
		if(0==strcmp(sh_name,".dynstr")){
			dynstr_hdr = tmp_hdr;
		}

		if(0==strcmp(sh_name,".dynsym")){
			dynsym_hdr = tmp_hdr;
		}


	}

	char * strtab_content = read_section(f,&strtab_hdr);
	//char * dynstr_content = read_section(f,&dynstr_hdr);


	Elf64_Sym * symtab_content = (Elf64_Sym *) read_section(f,&symtab_hdr);

	int num_entries = symtab_hdr.sh_size / symtab_hdr.sh_entsize;
	//int target_index = -1;
	for (int i =0 ; i < num_entries ;i ++){
		Elf64_Sym * symbol = symtab_content+i;
		char * symbol_name = strtab_content + symbol -> st_name;
        if(strcmp(kernel_name,symbol_name)==0){
            puts(symbol_name);
            symbol->st_size = new_size;
		    fseek(f,symtab_hdr.sh_offset + i * sizeof(Elf64_Sym),SEEK_SET);
			fwrite(symbol,sizeof(Elf64_Sym),1,f);

        }
	}

}

void AMDGPU_KERNEL_METADATA::parse_msgpack(FILE * f){
	ElfW header;
    fseek(f,0,SEEK_SET);
	fread(&header,sizeof(header),1,f);

	Shdr shstrtable_header;
	read_shdr(&shstrtable_header,f,&header,header.e_shstrndx);
	char * shstrtable = read_section(f,&shstrtable_header);
	Shdr tmp_hdr;
	Shdr note_hdr;

	//int text_index = -1;
	int note_index = -1;
	for (unsigned int i = 1; i < header.e_shnum ; i ++){
		read_shdr(&tmp_hdr,f,&header,i);
		char * sh_name = shstrtable+tmp_hdr.sh_name;
		if(0==strcmp(sh_name,".note")){
			note_hdr = tmp_hdr;
            //printf("found .note at index %d\n",i);
			note_index = i;
		}

	}
    if(note_index == -1){
        printf("ERROR ! Can't Find Symtab\n");
        exit(-1);
    }

	//char * strtab_content = read_section(f,&strtab_hdr);
	//char * dynstr_content = read_section(f,&dynstr_hdr);


	char * note_content = (char *) read_section(f,&note_hdr);
    std::string str(note_content,note_hdr.sh_size);
    
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

    kernarg_list_map[".sgpr_count"].convert(sgpr_count);
    kernarg_list_map[".vgpr_count"].convert(vgpr_count);
    kernarg_list_map[".group_segment_fixed_size"].convert(group_segment_fixed_size);
    //vgpr_count = kernarg_list_map[".sgpr_count"];
    //group_segment_fixed_size = kernarg_list_map[".group_segment_fixed_size"];


}



AMDGPU_KERNEL_METADATA::AMDGPU_KERNEL_METADATA(FILE * fp , uint32_t kd_offset){
            uint32_t sgpr_count = 0;
            dispatch_ptr = queue_ptr = kernarg_segment_ptr = dispatch_id = flat_scratch_init = private_segment_size = work_group_id_x = work_group_id_y = work_group_id_z = -1;
            pgm_rsrc1 = new COMPUTE_PGM_RSRC1(fp,kd_offset);
            pgm_rsrc2 = new COMPUTE_PGM_RSRC2(fp,kd_offset);
            pgm_others = new Others(fp,kd_offset);
            if(pgm_others->enable_sgpr_private_segment_buffer){
                sgpr_count += 4;
            }
            if(pgm_others->enable_sgpr_dispatch_ptr){
                dispatch_ptr = sgpr_count;
                sgpr_count += 2;
            }
            if(pgm_others->enable_sgpr_queue_ptr){
                queue_ptr = sgpr_count;
                sgpr_count +=2 ;
            }
            if(pgm_others->enable_sgpr_kernarg_segment_ptr){
                kernarg_segment_ptr = sgpr_count;
                sgpr_count += 2;
            }
            if(pgm_others->enable_sgpr_dispatch_id){
                dispatch_id = sgpr_count;
                sgpr_count += 2;
            }
            if(pgm_others->enable_sgpr_flat_scratch_init){
                flat_scratch_init = sgpr_count;
                sgpr_count += 2;
            }
            if(pgm_others->enable_sgpr_private_segment_size){
                private_segment_size = sgpr_count;
                sgpr_count += 1;
            }
            if(pgm_rsrc2->enable_sgpr_workgroup_id_x){
                work_group_id_x = sgpr_count;
                sgpr_count +=1;
            }
            if(pgm_rsrc2->enable_sgpr_workgroup_id_y){
                work_group_id_y = sgpr_count;
                sgpr_count +=1;
            }
            if(pgm_rsrc2->enable_sgpr_workgroup_id_x){
                work_group_id_z = sgpr_count;
                sgpr_count +=1;
            }
            parse_msgpack(fp);
            PRINT(dispatch_ptr);
            PRINT(kernarg_segment_ptr);
            PRINT(queue_ptr);
            PRINT(work_group_id_x);
            PRINT(work_group_id_y);
            PRINT(work_group_id_z);
            PRINT(sgpr_count);
            PRINT(vgpr_count);
            PRINT(group_segment_fixed_size);

           
        }

