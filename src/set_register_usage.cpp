#include <elf.h>
#include <vector>
#include <iostream>
#include <cstring>
#include <cassert>

#define WAIT_CONST 14

#define ElfW Elf64_Ehdr
#define Shdr Elf64_Shdr
using namespace std;
void read_shdr(Shdr * shdr,FILE * f,  ElfW* hdr, int offset){
    fseek(f,hdr->e_shoff + sizeof(Shdr) * offset ,SEEK_SET);
    fread(shdr,sizeof(Shdr),1,f);
}

char * read_section(FILE * f, Shdr * shdr) {
    int offset = shdr->sh_offset;
    int size = shdr->sh_size;
    fseek(f,offset,SEEK_SET);
    char * ret = (char * ) malloc(size) ;
    fread(ret,size,1,f);
    return ret;
}


void get_kds(FILE * f, vector<pair<uint64_t, string>> & ret){
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
            set_pgm_rsrc1(fp,p.first,new_bits);
            uint32_t written_bits = get_pgm_rsrc1(fp,p.first);
            printf("reading own write, bits = %x\n",written_bits);
        }    
    } 
}

class COMPUTE_PGM_RSRC1 {

    public:
    uint8_t granulated_workitem_vgpr_count;
    uint8_t granulated_wavefront_sgpr_count;
    uint8_t priority;
    uint8_t flat_round_mode_32;
    uint8_t float_round_mode_16_64;
    uint8_t float_denorm_mode_32;
    uint8_t float_denorm_mode_16_64;
    uint8_t priv;
    uint8_t enable_dx10_clamp;
    uint8_t debug_mode;
    uint8_t enable_ieee_mode;
    uint8_t bulky;
    uint8_t cdbg_user;
    uint8_t f16_ovfl;
    uint8_t wgp_mode;
    uint8_t mem_ordered;
    uint8_t fwd_progress;


    COMPUTE_PGM_RSRC1 ( uint32_t reg_value ) {
        granulated_workitem_vgpr_count = get_bits(reg_value,5,0);
        granulated_wavefront_sgpr_count = get_bits(reg_value,9,6);
        printf("vgpr_count = %u, sgpr_count = %u\n",
            vgpr_bits_to_count(
                granulated_workitem_vgpr_count
            ),
            sgpr_bits_to_count(granulated_wavefront_sgpr_count));

    }


};


int main(int argc, char **argv){



    if(argc != 2){
        printf("Usage: %s <binary path>\n", argv[0]);
        return -1;
    }
    char *binaryPath = argv[1];

    FILE * fp = fopen(binaryPath,"rb+");
    vector< pair<uint64_t,string>> kds;
    
    get_kds(fp,kds);
    uint32_t ret =get_pgm_rsrc1(fp,kds[0].first);
    auto tmp = COMPUTE_PGM_RSRC1(ret);

    set_sgpr_usage( fp , kds , "_Z9mmmKernelP15HIP_vector_typeIfLj4EES1_S1_jj.kd" , 26);
    //ret =get_pgm_rsrc1(fp,kds[0].first);
    //tmp = COMPUTE_PGM_RSRC1(ret);
    fclose(fp);
}



    /*for ( auto & pa : kds) {
        cout << std::hex << pa.first  << " " << pa.second << endl;
    }

    char cmd[1000];
    while(~scanf("%s",cmd)){
        if(strcmp(cmd,"list") ==0 ){
            for ( auto & pa : kds) {
                cout << std::hex << pa.first  << " " << pa.second << endl;
            }
        }else if(strcmp(cmd,"setvgpr_usage")==0){
            printf("There are a total of %u kds\n",kds.size());
            printf("Please pick the kd you want to edit\n");
            int index, max_vgpr;
            scanf("%d",&index);
            printf("Enter the max index of your vector register\n");
            scanf("%d",&max_vgpr);
        }else if(strcmp(cmd,"setsgpr_usage")==0){
            printf("There are a total of %u kds\n",kds.size());
            printf("Please pick the kd you want to edit\n");
            int index, max_sgpr;
            scanf("%d",&index);
            printf("Enter the max index of your scalar register\n");
            scanf("%d",&max_sgpr);
        }

    }*/

