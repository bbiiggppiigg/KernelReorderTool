#include "Region.h"

#include "msgpack.hpp"

#include "KdUtils.h"

#include <iostream>
using namespace Dyninst;
using namespace SymtabAPI;

bool endsWith(const std::string &suffix, const std::string &str) {
  if (suffix.length() > str.length())
    return false;

  return str.substr(str.length() - suffix.length()) == suffix;
}

bool isKernelDescriptor(const Dyninst::SymtabAPI::Symbol *symbol) {
  // symbol
  // - is in symtab, not dynsym
  // - type is object, size is 64
  // - is in rodata section
  // - name ends with ".kd"

  if (symbol->isInDynSymtab())
    return false;

  bool c1 = symbol->getType() == Symbol::ST_OBJECT && symbol->getSize() == 64;

  const Region *region = symbol->getRegion();
  bool c2 = region->isData() && region->getRegionPermissions() == Region::RP_R;

  if (c1 && c2 == false)
    return false;

  std::string suffix = ".kd";
  return endsWith(suffix, symbol->getMangledName());
}

void dumpMsgpackObjType(msgpack::object &object) {
  switch (object.type) {
  case msgpack::type::NIL:
    std::cout << "nil" << '\n';
    break;

  case msgpack::type::BOOLEAN:
    std::cout << "boolean" << '\n';
    break;

  case msgpack::type::POSITIVE_INTEGER:
    std::cout << "uint" << '\n';
    break;

  case msgpack::type::NEGATIVE_INTEGER:
    std::cout << "int" << '\n';
    break;

  case msgpack::type::FLOAT32:
    std::cout << "float 32" << '\n';
    break;

  case msgpack::type::FLOAT64:
    std::cout << "float 64" << '\n';
    break;

  case msgpack::type::STR:
    std::cout << "str" << '\n';
    break;

  case msgpack::type::BIN:
    std::cout << "bin" << '\n';
    break;

  case msgpack::type::EXT:
    std::cout << "ext" << '\n';
    break;

  case msgpack::type::ARRAY:
    std::cout << "arrray" << '\n';
    break;

  case msgpack::type::MAP:
    std::cout << "map" << '\n';
    break;
  }
}

// Initial parsing, based on hsuan-heng's code
void parseNotes(const char *sectionContents, size_t length) {
  /*
   * Step 1, read entire .note section into buffer
   */
  std::stringstream ss;
  ss.write(sectionContents, length);

  std::map<std::string, msgpack::object> kvmap;
  std::vector<msgpack::object> kernarg_list;
  std::map<std::string, msgpack::object> kernarg_list_map;
  std::string str = ss.str();
  msgpack::zone z;

  uint32_t offset = 0;

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
  std::string name_szstr = str.substr(0, 4);
  uint32_t name_sz = *((uint32_t *)name_szstr.c_str());
  std::string note_type = str.substr(8, 4);
  std::string name = str.substr(12, name_sz);
  offset = 12 + name_sz;

  while (offset % 4)
    offset++;

  /*
   * Now we are ready to process the msgpack data
   * We unpack from offset calculated above
   * Unpack until we get to .group_segment_fixed_size
   *
   */
  std::cout << str.size() - offset << '\n';
  oh = msgpack::unpack(str.data() + offset, str.size() - offset);
  msgpack::object map_root = oh.get();
  dumpMsgpackObjType(map_root);
  map_root.convert(kvmap);
  kvmap["amdhsa.kernels"].convert(kernarg_list);
  kernarg_list[0].convert(kernarg_list_map);

  std::cout << " .sgpr_count = " << kernarg_list_map[".sgpr_count"]
            << std::endl;
  std::cout << " .vgpr_count = " << kernarg_list_map[".vgpr_count"]
            << std::endl;
}

void parseNoteMetadata(Elf_X_Shdr &sectionHeader) {
  assert(sectionHeader.sh_type() == SHT_NOTE &&
         "The section must be a noite section");

  Elf_X_Nhdr note = sectionHeader.get_note();

  assert(note.n_namesz() == 7);
  assert(strncmp(note.get_name(), "AMDGPU\0", 7) == 0 &&
         "Name of note must be AMDGPU\0");

  Elf_X_Data data = sectionHeader.get_data();
  const char *rawData = (const char *)data.d_buf();
  size_t length = data.d_size();

  parseNotes(rawData, length);
  // FIXME:
  // Make the bytes parsable in msgpack by looking at the note contents. Right
  // not we rely on Hsuan-Heng's manual parse of the entire note section.
}
