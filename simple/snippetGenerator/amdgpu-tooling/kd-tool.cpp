#include "Elf_X.h"
#include "Symtab.h"

#include "KdUtils.h"
#include "KernelDescriptor.h"

#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

#include "msgpack.hpp"

using namespace Dyninst;
using namespace SymtabAPI;

int main(int argc, char **argv) {
  assert(argc == 2);

  std::string filePath(argv[1]);
  std::ifstream inputFile(filePath);

  inputFile.seekg(0, std::ios::end);
  size_t length = inputFile.tellg();
  inputFile.seekg(0, std::ios::beg);

  char *buffer = new char[length];
  inputFile.read(buffer, length);

  Elf_X *elfHeader = Elf_X::newElf_X(buffer, length);

  // iterate over all section headers
  const unsigned long numSections = elfHeader->e_shnum();
  for (unsigned i = 0; i < numSections; ++i) {
    Elf_X_Shdr &sectionHeader = elfHeader->get_shdr(i);
    if (sectionHeader.sh_type() == SHT_NOTE) {
      parseNoteMetadata(sectionHeader);
    }
  }

  // for loading symtab
  std::string name;
  bool error;
  Symtab symtab(reinterpret_cast<unsigned char *>(buffer), length, name,
                /*defensive_binary=*/false, error);
  if (error) {
    std::cerr << "error loading " << filePath << '\n';
    exit(1);
  }

  std::vector<Symbol *> symbols;
  symtab.getAllSymbols(symbols);

  for (const Symbol *symbol : symbols) {
    if (isKernelDescriptor(symbol)) {
      KernelDescriptor kd(symbol, elfHeader);
      kd.dumpDetailed(std::cout);
      std::cout << '\n' << *symbol << '\n' << '\n';
    }
  }
}
