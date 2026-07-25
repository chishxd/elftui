#include <elf.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

typedef union {
  Elf32_Ehdr elf32;
  Elf64_Ehdr elf64;
} Elf_Header;

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
    printf("Little Endian");
    break;
  case ELFDATA2MSB:
    printf("Big Endian");
    break;
  default:
    fprintf(stderr, "Error: Invalid data encoding");
    return 1;
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
