#include <elf.h>
#include <vector>
#include <iostream>
#include "CodeObject.h"
#include "InstructionDecoder.h"
#include "CFG.h"
#include "InsnFactory.h"


using namespace std;
using namespace Dyninst;
using namespace ParseAPI;
using namespace InstructionAPI;


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

typedef struct symtab_symbol{
	uint32_t index;
	uint32_t st_value;
	uint32_t st_size;
}symtab_symbol;



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

	Shdr text_hdr;
	Shdr symtab_hdr;
	Shdr strtab_hdr;
	Shdr dynsym_hdr;
	Shdr dynstr_hdr;

	int text_index = -1;
	int symtab_index = -1;
	int dynsym_index = -1;
	for (unsigned int i = 1; i < header.e_shnum ; i ++){
		read_shdr(&tmp_hdr,f,&header,i);
		char * sh_name = shstrtable+tmp_hdr.sh_name;
		if(0==strcmp(sh_name,".text")){
			text_index = i ;
			text_hdr = tmp_hdr;
		}
		if(0==strcmp(sh_name,".symtab")){
			symtab_hdr = tmp_hdr;
			symtab_index = i;
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
	char * dynstr_content = read_section(f,&dynstr_hdr);


	Elf64_Sym * symtab_content = (Elf64_Sym *) read_section(f,&symtab_hdr);

	int num_entries = symtab_hdr.sh_size / symtab_hdr.sh_entsize;
	int target_index = -1;
	for (int i =0 ; i < num_entries ;i ++){
		Elf64_Sym * symbol = symtab_content+i;
		char * symbol_name = strtab_content + symbol -> st_name;
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





void update_branches(FILE* fp , vector<MyBranchInsn> &branches, uint32_t insert_loc , uint32_t insert_size){
    for ( auto & branch : branches ){
        branch.update_for_insertion(insert_loc,insert_size);
    }

    for ( auto & branch : branches ){
        branch.write_to_file(fp);
    }
}

// Inplace Insertion of Code Snippet
// This should be easier 
// Keep track of a global structure of edges, we only need to modify the edges that go across the specified insert location
//

void inplace_insert(FILE * fp , const uint32_t text_start , uint32_t & text_end , vector<MyInsn> & insns, vector<MyBranchInsn> & branches, uint32_t insert_location, vector<char *> &insn_pool){    

	void * file_buffer = malloc(sizeof(char *) * (text_end - insert_location));

	fseek(fp,insert_location,SEEK_SET);
	fread(file_buffer, 1 , text_end - insert_location , fp );
	fseek(fp,insert_location,SEEK_SET);
	uint32_t size_acc = 0;
	for(auto & insn : insns){
		insn.write(fp);
		size_acc+= insn.size;

	}
	fwrite(file_buffer,1,text_end,fp);
	printf("before update symtab sytmbols\n");
	update_symtab_symbols(fp, text_start ,text_end, insert_location, size_acc);
    update_branches(fp,branches,insert_location,size_acc);
	text_end += size_acc;


}

typedef struct {
	Address branch_address;
	Address target_address;
    char cmd[64];
    uint32_t length;
} CFG_EDGE;

void analyze_cfg(char * binaryPath, uint32_t & ret_text_start , uint32_t & ret_text_end, vector<CFG_EDGE> & ret_edges ){


	map<Address,bool> seen;
	vector<Function *> funcs;
	SymtabCodeSource * sts;
	CodeObject * co;
	sts = new SymtabCodeSource(binaryPath);
	co = new CodeObject( sts);
	co -> parse();
	uint32_t text_start= -1;
	uint32_t text_end = 0;
	const CodeObject::funclist & all = co->funcs();
	auto fit = all.begin();
	for (int i =0 ; fit != all.end(); fit++ , i++){
		Function * f = * fit;
		InstructionDecoder decoder(f->isrc()->getPtrToInstruction(f->addr()),InstructionDecoder::maxInstructionLength,f->region()->getArch());

		auto bit = f->blocks().begin();
		for (; bit != f->blocks().end(); ++ bit){
			Block * b = * bit;
			if(b->start() < text_start)
				text_start = b->start();
			if(b->end() > text_end)
				text_end = b->end();
			if(seen.find(b->start())!=seen.end())
				continue;
			seen[b->start()] = true;
			for (auto & edges : b->targets()){

				Address branch_addr = edges->src()->lastInsnAddr();
                void * insn_ptr = f->isrc()->getPtrToInstruction(branch_addr);
				Instruction instr = decoder.decode((unsigned char *) insn_ptr);

				if(edges->type() == COND_TAKEN){
					printf("DEALING WITH DRIRECT BRANCH");
					cout << instr.format() << endl;
					cout << "branching to " << std::hex<<edges->trg()->start() << endl;
					CFG_EDGE cfg_edge;
					cfg_edge.branch_address = branch_addr;
					cfg_edge.target_address = edges->trg()->start();
                    memset(cfg_edge.cmd,0,sizeof(cfg_edge.cmd));
                    memcpy( cfg_edge.cmd, (const char *) insn_ptr, instr.size()  );
                    //printf("%x\n", * (uint32_t *) cfg_edge.cmd);
                    
                    cout << std::hex << cfg_edge.cmd << endl;
					cfg_edge.length = instr.size();
					ret_edges.push_back(cfg_edge);
				}
			}

		}

	}
	ret_text_start = text_start;
	ret_text_end = text_end;
}


int main(int argc, char **argv){


	vector < char *> insn_pool;
	if(argc != 5){
		printf("Usage: %s <binary path> <start_included> <end_excluded> <tramp_location> \n", argv[0]);
		return -1;
	}

	char *binaryPath = argv[1];
	FILE* fp = fopen(binaryPath,"rb+");
	uint32_t text_start, text_end;
	vector<CFG_EDGE> edges;
	analyze_cfg(binaryPath, text_start , text_end, edges); 
    vector<MyBranchInsn> branches;
    for (auto & edge : edges ) {
        branches.push_back(
            InsnFactory::convert_to_branch_insn(edge.branch_address, edge.target_address, edge.cmd, edge.length, insn_pool)
        );
    }
	printf("text_start at %x , text_end at %x \n",text_start, text_end );
	vector<MyInsn> to_insert;
	
	to_insert.push_back(InsnFactory::create_s_mov_b32(10,0,insn_pool));
	inplace_insert(fp,text_start,text_end,to_insert,branches,0x1088,insn_pool);
	inplace_insert(fp,text_start,text_end,to_insert,branches,0x1088,insn_pool);
	inplace_insert(fp,text_start,text_end,to_insert,branches,0x1088,insn_pool);



	for (auto &p : insn_pool){
		free(p);    
	}



	fclose(fp);
	return 0;

}





