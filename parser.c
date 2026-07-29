#include "parser.h"
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

/*
 * @brief Parse 16 byte array from Elf32_Ehdr
 * @details This function is a melted down version of main data extraction
 * function below The separation was done becausde the array itself has 16 bytes
 * filled with info.
 */
int parse_ident(const Elf_Header *header, Elf_Metadata *meta) {
  if (header->elf64.e_ident[EI_MAG0] != ELFMAG0 ||
      header->elf64.e_ident[EI_MAG1] != ELFMAG1 ||
      header->elf64.e_ident[EI_MAG2] != ELFMAG2 ||
      header->elf64.e_ident[EI_MAG3] != ELFMAG3) {
    fprintf(stderr, "Error: Invalid ELF File!\n");
    return 1;
  }

  switch (header->elf64.e_ident[EI_CLASS]) {
  case ELFCLASS64: meta->class_str = "64-bit"; break;
  case ELFCLASS32: meta->class_str = "32bit"; break;
  default: perror("Invalid ELF class\n"); return 1;
  }

  switch (header->elf64.e_ident[EI_DATA]) {
  case ELFDATA2LSB: meta->endian_str = "Little Endian"; break;
  case ELFDATA2MSB: meta->endian_str = "Big Endian"; break;
  default: fprintf(stderr, "Error: Invalid data encoding\n"); return 1;
  }

  switch (header->elf64.e_ident[EI_VERSION]) {
  case EV_CURRENT: meta->version = EV_CURRENT; break;
  default: fprintf(stderr, "Error: Invalid Version\n"); return 1;
  }

  switch (header->elf64.e_ident[EI_OSABI]) {
  case ELFOSABI_SYSV: meta->os_abi = "UNIX System V"; break;
  case ELFOSABI_HPUX: meta->os_abi = "HP-UX"; break;
  case ELFOSABI_NETBSD: meta->os_abi = "NETBSD"; break;
  case ELFOSABI_LINUX: meta->os_abi = "linux"; break;
  case ELFOSABI_SOLARIS: meta->os_abi = "Solaris"; break;
  case ELFOSABI_IRIX: meta->os_abi = "IRIX"; break;
  case ELFOSABI_FREEBSD: meta->os_abi = "FreeBSD"; break;
  case ELFOSABI_TRU64: meta->os_abi = "TRU64"; break;
  case ELFOSABI_ARM: meta->os_abi = "Arm architecture"; break;
  case ELFOSABI_STANDALONE: meta->os_abi = "Stand-alone(embedded)"; break;
  default: meta->os_abi = "Unknown"; break;
  }

  meta->abi_version = header->elf64.e_ident[EI_ABIVERSION];

  return 0;
}

static const char *get_type_name(uint16_t type) {
  switch (type) {
  case ET_REL: return "Relocatable File";
  case ET_EXEC: return "Execultable File";
  case ET_DYN: return "Shared Object File";
  case ET_CORE: return "Core File";
  case ET_NONE: return "Unknow File";
  default: return "Unknown File";
  }
}

static const char *get_machine_type(uint16_t type) {
  switch (type) {
  case EM_M32: return "AT&T WE 32100";
  case EM_SPARC: return "Sun Microsys SPARC";
  case EM_386: return "Intel 80386";
  case EM_68K: return "Motorola 68000";
  case EM_88K: return "Motorola 88000";
  case EM_MIPS: return "MIPS RS3000";
  case EM_PARISC: return "HP/PA";
  case EM_SPARC32PLUS: return "SPARC with enhanced instruction set";
  case EM_PPC: return "PowerPC";
  case EM_PPC64: return "PowerPC 64-bit";
  case EM_S390: return "IBM S/390";
  case EM_ARM: return "Advanced RISC Machines";
  case EM_SH: return "Renesas SuperH";
  case EM_SPARCV9: return "SPARC v9 64-bit";
  case EM_IA_64: return "Intel Itanium";
  case EM_X86_64: return "AMD x86-64";
  case EM_VAX: return "Dec Vax";
  default: return "Unknown architecture";
  }
}

int parse_elf_header(const Elf_Header *header, Elf_Metadata *meta) {
  if (parse_ident(header, meta) != 0) { return 1; }
  uint16_t raw_type;
  uint16_t raw_machine;

  if (header->elf64.e_ident[EI_CLASS] == ELFCLASS64) {
    raw_type = header->elf64.e_type;
    raw_machine = header->elf64.e_machine;

    meta->file_version = header->elf64.e_version;
    meta->entry_point = header->elf64.e_entry;
    meta->ph_offset = header->elf64.e_phoff;
    meta->sh_offset = header->elf64.e_shoff;
    meta->flags = header->elf64.e_flags;
    meta->eh_size = header->elf64.e_ehsize;
    meta->ph_entry_size = header->elf64.e_phentsize;
    meta->ph_num = header->elf64.e_phnum;
    meta->sh_entry_size = header->elf64.e_shentsize;
    meta->sh_num = header->elf64.e_shnum;
    meta->sh_str_ndx = header->elf64.e_shstrndx;

  } else {
    raw_type = header->elf32.e_type;
    raw_machine = header->elf32.e_machine;

    meta->file_version = header->elf32.e_version;
    meta->entry_point = header->elf32.e_entry;
    meta->ph_offset = header->elf32.e_phoff;
    meta->sh_offset = header->elf32.e_shoff;
    meta->flags = header->elf32.e_flags;
    meta->eh_size = header->elf32.e_ehsize;
    meta->ph_entry_size = header->elf32.e_phentsize;
    meta->ph_num = header->elf32.e_phnum;
    meta->sh_entry_size = header->elf32.e_shentsize;
    meta->sh_num = header->elf32.e_shnum;
    meta->sh_str_ndx = header->elf32.e_shstrndx;
  }

  meta->type_str = get_type_name(raw_type);
  meta->machine_str = get_machine_type(raw_machine);
  return 0;
}
