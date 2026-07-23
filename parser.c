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

  uint8_t buffer[4];

  if (read(file, buffer, 4) != 4) {
    perror("Error reading first 4 bytes");
    close(file);
    return 1;
  }

  if (buffer[0] == 0x7F && buffer[1] == 'E' && buffer[2] == 'L' &&
      buffer[3] == 'F') {
    printf("Valid ELF file\n");
  } else {
    printf("Invalid ELF file!\n");
  }

  close(file);

  return 0;
}
