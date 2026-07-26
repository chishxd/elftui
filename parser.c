#include <elf.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

typedef union {
  Elf32_Ehdr elf32;
  Elf64_Ehdr elf64;
} Elf_Header;

// Trying out writing some cool docs for first time :fear:

/*
 * @brief Parse 16 byte array from Elf32_Ehdr
 * @details This function is a melted down version of main data extraction
 * function below The separation was done becausde the array itself has 16 bytes
 * filled with info.
 */
int parse_ident(const Elf_Header *header) {
  if (header->elf64.e_ident[EI_MAG0] != ELFMAG0 ||
      header->elf64.e_ident[EI_MAG1] != ELFMAG1 ||
      header->elf64.e_ident[EI_MAG2] != ELFMAG2 ||
      header->elf64.e_ident[EI_MAG3] != ELFMAG3) {
    fprintf(stderr, "Error: Invalid ELF File!");
    return 1;
  }

  switch (header->elf64.e_ident[EI_CLASS]) {
  case ELFCLASS64:
    printf("64-bit ELF detected\n");
    break;
  case ELFCLASS32:
    printf("32-bit ELF detected\n");
    break;
  default:
    perror("Invalid ELF class");
    return 1;
  }

  switch (header->elf64.e_ident[EI_DATA]) {
  case ELFDATA2LSB:
    printf("Little Endian\n");
    break;
  case ELFDATA2MSB:
    printf("Big Endian\n");
    break;
  default:
    fprintf(stderr, "Error: Invalid data encoding\n");
    return 1;
  }

  switch (header->elf64.e_ident[EI_VERSION]) {
  case EV_CURRENT:
    printf("Current Version: %d\n", EV_CURRENT);
    break;
  default:
    fprintf(stderr, "Error: Invalid Version");
    return 1;
  }

  switch (header->elf64.e_ident[EI_OSABI]) {
  case ELFOSABI_SYSV:
    printf("OS ABI: UNIX System V\n");
    break;
  case ELFOSABI_HPUX:
    printf("OS ABI: HP-UX\n");
    break;
  case ELFOSABI_NETBSD:
    printf("OS ABI: NETBSD\n");
    break;
  case ELFOSABI_LINUX:
    printf("OS ABI: LINUX\n");
    break;
  case ELFOSABI_SOLARIS:
    printf("OS ABI: SOLARIS\n");
    break;
  case ELFOSABI_IRIX:
    printf("OS ABI: IRIX\n");
    break;
  case ELFOSABI_FREEBSD:
    printf("OS ABI: FREEBSD\n");
    break;
  case ELFOSABI_TRU64:
    printf("OS ABI: TRU64\n");
    break;
  case ELFOSABI_ARM:
    printf("OS ABI: ARM architecture\n");
    break;
  case ELFOSABI_STANDALONE:
    printf("OS ABI: Stand-alone(embedded)\n");
    break;
  default:
    fprintf(stderr, "OS ABI: UNIX System V\n");
    break;
  }

  return 0;
}

void parse_elf_header(const Elf_Header *header) { parse_ident(header); }

int main() {
  int file = open("/usr/bin/ls", O_RDONLY);

  if (file == -1) {
    perror("Failed to open file");
    return 1;
  }

  Elf_Header header;

  if (read(file, &header, sizeof(header)) != sizeof(header)) {
    perror("Error reading first 4 bytes");
    close(file);
    return 1;
  }
  parse_elf_header(&header);

  close(file);

  return 0;
}
