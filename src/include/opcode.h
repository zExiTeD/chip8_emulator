#ifndef OPCODE_H
#define OPCODE_H

#include <stdint.h>

struct system;

struct opcode {
    uint8_t  Group;
    uint8_t  X;
    uint8_t  Y;
    uint8_t  N;
    uint8_t  NN;
    uint16_t NNN;
};

struct opcode_entry {
   void (*exec)( struct system *system, struct opcode op_code );
};

struct opcode opcode_get(struct system *system);
void create_opcode_hashmap(struct system* system);

#endif
