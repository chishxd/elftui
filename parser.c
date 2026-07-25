#include <elf.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>

int main() {
  int file = open("/usr/bin/ls", O_RDONLY);

  if (file == -1) {
    perror("Failed to open file");
    return 1;
  }

  union {
    Elf32_Ehdr elf32;
    Elf64_Ehdr elf64;
  } header;

  if (read(file, &header, sizeof(header)) != sizeof(header)) {
    perror("Error reading first 4 bytes");
    close(file);
    return 1;
  }

  if (header.elf64.e_ident[EI_MAG0] == ELFMAG0 &&
      header.elf64.e_ident[EI_MAG1] == ELFMAG1 &&
      header.elf64.e_ident[EI_MAG2] == ELFMAG2 &&
      header.elf64.e_ident[EI_MAG3] == ELFMAG3) {
    switch (header.elf64.e_ident[EI_CLASS]) {
    case ELFCLASS64:
      printf("64-bit ELF detected\n");
      printf("Entry Point: 0x%lx\n", header.elf64.e_entry);
      break;
    case ELFCLASS32:
      printf("32-bit ELF detected\n");
      printf("Entry Point: 0x%lx\n", header.elf64.e_entry);
    }
  }

  close(file);

  return 0;
}
