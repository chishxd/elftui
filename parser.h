#ifndef PARSER_H
#define PARSER_H

#include <elf.h>
#include <stdint.h>

typedef union {
  Elf32_Ehdr elf32;
  Elf64_Ehdr elf64;
} Elf_Header;

// This struct extracts info about binary into the form I want for TUI
typedef struct {
  const char *class_str;
  const char *endian_str;
  int version; // This one is from the array i.e e_ident[EI_VERSION]
  const char *os_abi;
  uint8_t abi_version;

  //---- e_ident stuff ends here, now remaining are struct data members ----//

  const char *type_str;
  const char *machine_str;
  uint32_t file_version; // This one is from the Struct i.e e_version
  uint64_t entry_point;
  uint64_t ph_offset;
  uint64_t sh_offset;
  uint32_t flags;
  uint16_t eh_size;
  uint16_t ph_entry_size;
  uint16_t ph_num;
  uint16_t sh_entry_size;
  uint16_t sh_num;
  uint16_t sh_str_ndx;

} Elf_Metadata;

int parse_elf_header(const Elf_Header *header, Elf_Metadata *meta);
#endif // !PARSER_H
