#include "./system.h"
#include "./allocator.h"
#include "./helper.h"

#include <corecrt_memory.h>
#include <stdio.h>

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

  return system;
}

opcode op_code_get(struct system *system) {
    opcode op_code = system->memory[ system->cpu.program_counter ] << 8 | system->memory[ system->cpu.program_counter + 1 ];
    return op_code;
}

void system_execute_opcode(struct system *system) {
  opcode opcode = op_code_get(system);
}

void system_check_collision(struct system *system) {}

void system_display_draw(struct system *system) {}
