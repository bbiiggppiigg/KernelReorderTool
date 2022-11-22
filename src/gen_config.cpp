#include "config.h"
void readFile(){
    SymtabAPI::Symtab * symTab;
    string binaryPathStr(binaryPath);
    printf("before calling openFile\n");
    SymtabAPI::Symtab::openFile(symTab, binaryPathStr);
    vector<SymtabAPI::Module *> modules;



    std::string filePath(binaryPath);
    std::ifstream inputFile(filePath);

    inputFile.seekg(0, std::ios::end);
    size_t length = inputFile.tellg();
    inputFile.seekg(0, std::ios::beg);

    char *buffer = new char[length];
    inputFile.read(buffer, length);

    Elf_X *elfHeader = Elf_X::newElf_X(buffer, length);  printf("before calling getAllModules \n");
    symTab->getAllModules(modules);
    last_instr = 0;
    for (const auto & module : modules){
        vector<SymtabAPI::Symbol *> symbols;
        module->getAllSymbols(symbols);
        for(const auto & symbol : symbols){
            if (symbol->getType() == SymtabAPI::Symbol::ST_FUNCTION && symbol->isInSymtab()){
                uint32_t offset = symbol->getOffset();
                uint32_t size = symbol->getSize();
                string name = symbol->getMangledName();
                bool found = false;
                string kd_name = name + ".kd";
                Address kdAddr;

                kernel_bound kb;
                for (const auto & symbol2 : symbols){
                    string name2 = symbol2->getMangledName();
                    if(name2 == kd_name){
                        found = true;
                        kdAddr = symbol2->getOffset();

                        kb.kd = new KernelDescriptor(symbol2,elfHeader);
                        break;
                    }
                }
                if(!found){
                    printf("failed to find corresponding kernel descriptor");
                    exit(-1);
                }

                kb.first = offset;
                kb.last = offset + size;
                kb.name = name;
                kb.kdAddr = kdAddr;
                kernel_bounds.push_back(kb);
                //std::cout <<" symbol name = " << name << " offset = " << offset << " size = " << size << std::endl;

                if(kb.last > last_instr){
                    last_instr = kb.last;
                }
            }
        }
    }
    printf("before closing symtab\n");
    SymtabAPI::Symtab::closeSymtab(symTab);
    printf("after closing symtab\n");

}

int main(){
    return 0;
}
