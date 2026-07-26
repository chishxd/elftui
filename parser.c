#include <elf.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

typedef union {
  Elf32_Ehdr elf32;
  Elf64_Ehdr elf64;
} Elf_Header;

typedef struct {
  const char *class_str;
  const char *endian_str;
  int version;
  const char *os_abi;
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

  return 0;
}

void parse_elf_header(const Elf_Header *header, Elf_Metadata *meta) {
  parse_ident(header, meta);
}

int main() {
  int file = open("/usr/bin/ls", O_RDONLY);

  if (file == -1) {
    perror("Failed to open file");
    return 1;
  }

  Elf_Header header;
  Elf_Metadata meta;

  if (read(file, &header, sizeof(header)) != sizeof(header)) {
    perror("Error reading first 4 bytes");
    close(file);
    return 1;
  }
  parse_elf_header(&header, &meta);

  printf("%s\n", meta.class_str);
  printf("%s\n", meta.endian_str);
  printf("%d\n", meta.version);
  printf("%s\n", meta.os_abi);

  close(file);

  return 0;
}
