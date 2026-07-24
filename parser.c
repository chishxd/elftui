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

  uint8_t buffer[16];

  if (read(file, buffer, 16) != 16) {
    perror("Error reading first 4 bytes");
    close(file);
    return 1;
  }

  if (buffer[0] == 0x7F && buffer[1] == 'E' && buffer[2] == 'L' &&
      buffer[3] == 'F') {
    printf("Valid ELF file\n");

    switch (buffer[EI_CLASS]) {
      case ELFCLASS32:
        printf("32 bit object\n");
        break;
      case ELFCLASS64:
        printf("64 bit object\n");
        break;
      case ELFCLASSNONE:
        printf("Invaid object\n");
        break;
      default:
        break;
    }
  } else {
    printf("Invalid ELF file!\n");
  }

  close(file);

  return 0;
}
