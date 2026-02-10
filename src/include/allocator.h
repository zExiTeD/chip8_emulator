#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include <stdint.h>
#include <stdlib.h>

struct allocator {
  uint8_t*  (*alloc)(struct allocator*, uint32_t);
  void      (*de_allocate)(struct allocator*);
  uint32_t  length;
  uint32_t  used_len;
  uint8_t*  allocated_mem;
};

uint8_t* arena_allocate(struct allocator *allocator, uint32_t size) {
  if (allocator->used_len + size > allocator->length) {
    exit(1);
  }
  uint8_t* ptr = allocator->allocated_mem + allocator->used_len;
  allocator->used_len += size;
  return ptr;
}

void arena_free(struct allocator* allocator) {
  free(allocator);
}

struct allocator*
allocator_create_arena_allocator(int size_of_arena) {
  struct allocator* allocator = (struct allocator*)malloc(sizeof(struct allocator) + size_of_arena);

  allocator->alloc = arena_allocate;
  allocator->de_allocate = arena_free;
  allocator->length = size_of_arena;
  allocator->used_len = 0;
  allocator->allocated_mem = (uint8_t*)(allocator + 1);

  return allocator;
}

#endif
