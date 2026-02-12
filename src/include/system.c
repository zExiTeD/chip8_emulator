#include "./system.h"
#include "./allocator.h"
#include "./helper.h"

#include "./opcode.c"
#include "opcode.h"

#include <corecrt_memory.h>
#include <stdio.h>

#include <SDL.h>

struct system system_initialize(struct allocator *allocator, file_t file) {
  struct system system = {};
  system.memory = allocator->alloc(allocator, 4095);

  if (system.memory == NULL) {
    printf("[ ERROR ] : Failed to allocate Memory \n");
    exit(1);
  }

  memcpy( &system.memory[512] , file.file_data , file.file_size );

  system.cpu.program_counter = 0x200; // 0x200 == 512

  memcpy(system.memory, FontSet, sizeof(FontSet));

  create_opcode_hashmap( &system );

  return system;
}

void system_execute_opcode(struct system *system) {
    struct opcode op_code = opcode_get(system);
    uint16_t old_pc = system->cpu.program_counter;

    system->op_hashmap[op_code.Group].exec(system, op_code);

    if (system->cpu.program_counter == old_pc) {
        system->cpu.program_counter += 2;
    }
}
