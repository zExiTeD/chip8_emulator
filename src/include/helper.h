#ifndef HELPER_H
#define HELPER_H

#include "./allocator.h"
#include <stdint.h>
#include <stdio.h>

typedef struct {
  FILE *file_descriptor;
  uint32_t file_size;
  uint8_t *file_data;
} file_t;

static file_t open_and_read_entire_file(const char *file_path,
                                        struct allocator *allocator) {
  file_t file;
  fopen_s(&file.file_descriptor, file_path, "rb");

  fseek(file.file_descriptor, 0, SEEK_END);
  file.file_size = (uint32_t)ftell(file.file_descriptor);
  fseek(file.file_descriptor, 0, SEEK_SET);

  if (file.file_size > (4096 - 512)) {
    printf("[ ERROR ] : Rom Size is greater than Chip8 Memory \n");
    fclose(file.file_descriptor);
    exit(1);
  }

  file.file_data = allocator->alloc(allocator, file.file_size);
  size_t read_bytes =
      fread(file.file_data, 1, file.file_size, file.file_descriptor);

  if (!(read_bytes == file.file_size)) {
    printf("[ ERROR ] : Failed to read to memory \n");
    fclose(file.file_descriptor);
    exit(1);
  }

  fclose(file.file_descriptor);
  return file;
}

#endif
