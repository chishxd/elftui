#include <elf.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

typedef union {
  Elf32_Ehdr elf32;
  Elf64_Ehdr elf64;
} Elf_Header;

// This struct extracts info about binary into the form I want for TUI
typedef struct {
  const char *class_str;
  const char *endian_str;
  int version;
  const char *os_abi;
  uint8_t abi_version;
  const char *type_str;
  const char *machine_str;
} Elf_Metadata;

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
    fprintf(stderr, "Error: Invalid ELF File!");
    return 1;
  }

  switch (header->elf64.e_ident[EI_CLASS]) {
  case ELFCLASS64:
    meta->class_str = "64-bit";
    break;
  case ELFCLASS32:
    meta->class_str = "32bit";
    break;
  default:
    perror("Invalid ELF class\n");
    return 1;
  }

  switch (header->elf64.e_ident[EI_DATA]) {
  case ELFDATA2LSB:
    meta->endian_str = "Little Endian";
    break;
  case ELFDATA2MSB:
    meta->endian_str = "Big Endian";
    break;
  default:
    fprintf(stderr, "Error: Invalid data encoding\n");
    return 1;
  }

  switch (header->elf64.e_ident[EI_VERSION]) {
  case EV_CURRENT:
    meta->version = EV_CURRENT;
    break;
  default:
    fprintf(stderr, "Error: Invalid Version\n");
    return 1;
  }

  switch (header->elf64.e_ident[EI_OSABI]) {
  case ELFOSABI_SYSV:
    meta->os_abi = "UNIX System V";
    break;
  case ELFOSABI_HPUX:
    meta->os_abi = "HP-UX";
    break;
  case ELFOSABI_NETBSD:
    meta->os_abi = "NETBSD";
    break;
  case ELFOSABI_LINUX:
    meta->os_abi = "linux";
    break;
  case ELFOSABI_SOLARIS:
    meta->os_abi = "Solaris";
    break;
  case ELFOSABI_IRIX:
    meta->os_abi = "IRIX";
    break;
  case ELFOSABI_FREEBSD:
    meta->os_abi = "FreeBSD";
    break;
  case ELFOSABI_TRU64:
    meta->os_abi = "TRU64";
    break;
  case ELFOSABI_ARM:
    meta->os_abi = "Arm architecture";
    break;
  case ELFOSABI_STANDALONE:
    meta->os_abi = "Stand-alone(embedded)";
    break;
  default:
    fprintf(stderr, "OS ABI: UNIX System V\n");
    break;
  }

  meta->abi_version = header->elf64.e_ident[EI_ABIVERSION];

  return 0;
}

int parse_elf_header(const Elf_Header *header, Elf_Metadata *meta) {
  if (parse_ident(header, meta) != 0) { return 1; }

  if (header->elf64.e_ident[EI_CLASS] == ELFCLASS64) {

    // Parsing the type of binary from header's e_type data member
    // Note to me: check man elf and search e_type
    switch (header->elf64.e_type) {
    case ET_REL:
      meta->type_str = "Relocatable File";
      break;
    case ET_EXEC:
      meta->type_str = "Execultable File";
      break;
    case ET_DYN:
      meta->type_str = "Shared Object File";
      break;
    case ET_CORE:
      meta->type_str = "Core File";
      break;
    case ET_NONE:
      meta->type_str = "Unknow File";
      break;
    default:
      fprintf(stderr, "Something went wrong :(");
      return 1;
    }

    // Parsing the type of architecture required by file
    // again, open manpage and search e_machine
    switch (header->elf64.e_machine) {
    case EM_M32:
      meta->machine_str = "AT&T WE 32100";
      break;
    case EM_SPARC:
      meta->machine_str = "Sun Microsys SPARC";
      break;
    case EM_386:
      meta->machine_str = "Intel 80386";
      break;
    case EM_68K:
      meta->machine_str = "Motorola 68000";
      break;
    case EM_88K:
      meta->machine_str = "Motorola 88000";
      break;
    case EM_MIPS:
      meta->machine_str = "MIPS RS3000";
      break;
    case EM_PARISC:
      meta->machine_str = "HP/PA";
      break;
    case EM_SPARC32PLUS:
      meta->machine_str = "SPARC with enhanced instruction set";
      break;
    case EM_PPC:
      meta->machine_str = "PowerPC";
      break;
    case EM_PPC64:
      meta->machine_str = "PowerPC 64-bit";
      break;
    case EM_S390:
      meta->machine_str = "IBM S/390";
      break;
    case EM_ARM:
      meta->machine_str = "Advanced RISC Machines";
      break;
    case EM_SH:
      meta->machine_str = "Renesas SuperH";
      break;
    case EM_SPARCV9:
      meta->machine_str = "SPARC v9 64-bit";
      break;
    case EM_IA_64:
      meta->machine_str = "Intel Itanium";
      break;
    case EM_X86_64:
      meta->machine_str = "AMD x86-64";
      break;
    case EM_VAX:
      meta->machine_str = "Dec Vax";
      break;
    }
  }

  else if (header->elf64.e_ident[EI_CLASS] == ELFCLASS32) {

    // Parsing the type of binary from header's e_type data member
    // Note to me: check man elf and search e_type
    switch (header->elf32.e_type) {
    case ET_REL:
      meta->type_str = "Relocatable File";
      break;
    case ET_EXEC:
      meta->type_str = "Execultable File";
      break;
    case ET_DYN:
      meta->type_str = "Shared Object File";
      break;
    case ET_CORE:
      meta->type_str = "Core File";
      break;
    case ET_NONE:
      meta->type_str = "Unknow File";
      break;
    default:
      fprintf(stderr, "Something went wrong :(");
      return 1;
    }

    // Parsing the type of architecture required by file
    // again, open manpage and search e_machine
    switch (header->elf32.e_machine) {
    case EM_M32:
      meta->machine_str = "AT&T WE 32100";
      break;
    case EM_SPARC:
      meta->machine_str = "Sun Microsys SPARC";
      break;
    case EM_386:
      meta->machine_str = "Intel 80386";
      break;
    case EM_68K:
      meta->machine_str = "Motorola 68000";
      break;
    case EM_88K:
      meta->machine_str = "Motorola 88000";
      break;
    case EM_MIPS:
      meta->machine_str = "MIPS RS3000";
      break;
    case EM_PARISC:
      meta->machine_str = "HP/PA";
      break;
    case EM_SPARC32PLUS:
      meta->machine_str = "SPARC with enhanced instruction set";
      break;
    case EM_PPC:
      meta->machine_str = "PowerPC";
      break;
    case EM_PPC64:
      meta->machine_str = "PowerPC 64-bit";
      break;
    case EM_S390:
      meta->machine_str = "IBM S/390";
      break;
    case EM_ARM:
      meta->machine_str = "Advanced RISC Machines";
      break;
    case EM_SH:
      meta->machine_str = "Renesas SuperH";
      break;
    case EM_SPARCV9:
      meta->machine_str = "SPARC v9 64-bit";
      break;
    case EM_IA_64:
      meta->machine_str = "Intel Itanium";
      break;
    case EM_X86_64:
      meta->machine_str = "AMD x86-64";
      break;
    case EM_VAX:
      meta->machine_str = "Dec Vax";
      break;
    }
  }

  return 0;
}

int main() {
  int file = open("/usr/bin/ls", O_RDONLY);

  if (file == -1) {
    perror("Failed to open file");
    return 1;
  }

  Elf_Header header;
  Elf_Metadata meta = {0};

  if (read(file, &header, sizeof(header)) != sizeof(header)) {
    perror("Error reading first 4 bytes");
    close(file);
    return 1;
  }

  if (parse_elf_header(&header, &meta) != 0) {
    close(file);
    return 1;
  }
  printf("%s\n", meta.class_str);
  printf("%s\n", meta.endian_str);
  printf("%d\n", meta.version);
  printf("%s\n", meta.os_abi);
  printf("%d\n", meta.abi_version);
  printf("%s\n", meta.type_str);
  printf("%s\n", meta.machine_str);

  close(file);

  return 0;
}
