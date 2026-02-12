#include "allocator.h"
#include "system.h"
#include <string.h>
#include "./opcode.h"

struct opcode opcode_get(struct system *system) {
    return (struct opcode) {
        .Group = system->memory[system->cpu.program_counter] >> 4,
        .X     = system->memory[system->cpu.program_counter] & 0x0F,
        .Y     = system->memory[system->cpu.program_counter + 1] >> 4,
        .N     = system->memory[system->cpu.program_counter + 1] & 0x0F,
        .NN    = system->memory[system->cpu.program_counter + 1],
        .NNN   = (( system->memory[system->cpu.program_counter] & 0x0F) << 8) | system->memory[system->cpu.program_counter + 1],
    };
}

void opcode_0x0( struct system *system, struct opcode op_code ) {
    if ( op_code.NN == 0xE0 ) {
        memset( &system->display , false , (CHIP8_SCREEN_WIDTH * CHIP8_SCREEN_HEIGHT ));
    }
    else if ( op_code.NN == 0xEE ) {
        // [NOTE] : Implement Later Subroutines
        system->cpu.sp--;
        system->cpu.program_counter = system->stack[system->cpu.sp];

    }
}

void opcode_0x1( struct system *system, struct opcode op_code ) {
        system->cpu.program_counter = op_code.NNN;
}

void opcode_0x2( struct system *system, struct opcode op_code ) {
    // [NOTE] : Implement Later Subroutine

    system->stack[system->cpu.sp] = system->cpu.program_counter;
    system->cpu.sp++;
    system->cpu.program_counter = op_code.NNN;

}

void opcode_0x3( struct system *system, struct opcode op_code ) {
    if( system->cpu.V[ op_code.X ] == op_code.NN ) {
        system->cpu.program_counter += 2;
    }
}

void opcode_0x4( struct system *system, struct opcode op_code ) {
    if( system->cpu.V[ op_code.X ] != op_code.NN ) {
        system->cpu.program_counter += 2;
    }

}

void opcode_0x5( struct system *system, struct opcode op_code ) {
    if( system->cpu.V[ op_code.X ] == system->cpu.V[ op_code.Y ] ) {
        system->cpu.program_counter += 2;
    }
}

void opcode_0x6( struct system *system, struct opcode op_code ) {
    system->cpu.V[ op_code.X ] = op_code.NN;
}

void opcode_0x7( struct system *system, struct opcode op_code ) {
    system->cpu.V[ op_code.X ] += op_code.NN;
}

void opcode_0x8( struct system *system, struct opcode op_code ) {
    switch( op_code.N ) {
        case 0:{
            system->cpu.V[ op_code.X ] = system->cpu.V[ op_code.Y ];
        }break;
        case 1:{
            system->cpu.V[ op_code.X ] |= system->cpu.V[ op_code.Y ];
        }break;
        case 2:{
            system->cpu.V[ op_code.X ] &= system->cpu.V[ op_code.Y ];
        }break;
        case 3:{
            system->cpu.V[ op_code.X ] ^= system->cpu.V[ op_code.Y ];
        }break;
        case 4:{
            uint16_t sum = system->cpu.V[op_code.X] + system->cpu.V[op_code.Y];
            system->cpu.V[0xF] = (sum > 0xFF) ? 1 : 0;
            system->cpu.V[op_code.X] = sum & 0xFF;
        }break;
        case 5:{
            system->cpu.V[0xF] = (system->cpu.V[op_code.X] >= system->cpu.V[op_code.Y]) ? 1 : 0;
            system->cpu.V[op_code.X] -= system->cpu.V[op_code.Y];
        }break;
        case 6:{
            system->cpu.V[0xF] = system->cpu.V[op_code.X] & 0x1;
            system->cpu.V[op_code.X] >>= 1;
        }break;
        case 7:  {
            system->cpu.V[0xF] = (system->cpu.V[op_code.Y] >= system->cpu.V[op_code.X]) ? 1 : 0;
            system->cpu.V[op_code.X] = system->cpu.V[op_code.Y] - system->cpu.V[op_code.X];
        }break;
        case 0xE:{
            system->cpu.V[0xF] = (system->cpu.V[op_code.X] >> 7) & 0x1;
            system->cpu.V[op_code.X] <<= 1;
        }break;

        default:{
            printf(" huh ? \n");
        }break;
    }
}

void opcode_0x9( struct system *system, struct opcode op_code ) {
    if (system->cpu.V[op_code.X] != system->cpu.V[op_code.Y]) {
        system->cpu.program_counter += 2;
    }
}

void opcode_0xA( struct system *system, struct opcode op_code ) {
    system->cpu.I = op_code.NNN;
}

void opcode_0xB( struct system *system, struct opcode op_code ) {
    system->cpu.program_counter = op_code.NNN + system->cpu.V[0];
}

void opcode_0xC( struct system *system, struct opcode op_code ) {
    system->cpu.V[op_code.X] = (rand() % 256) & op_code.NN;
}

void opcode_0xD( struct system *system, struct opcode op_code ) {
    uint8_t x_coord = system->cpu.V[op_code.X] % CHIP8_SCREEN_WIDTH;
    uint8_t y_coord = system->cpu.V[op_code.Y] % CHIP8_SCREEN_HEIGHT;
    uint8_t height = op_code.N;

    system->cpu.V[0xF] = 0;

    for (uint8_t row = 0; row < height; row++) {
        uint8_t sprite_byte = system->memory[system->cpu.I + row];
        if ((y_coord + row) >= CHIP8_SCREEN_HEIGHT) {
            break;
        }

        for (uint8_t col = 0; col < 8; col++) {
            if ((x_coord + col) >= CHIP8_SCREEN_WIDTH) {
                break;
            }
            uint8_t sprite_pixel = (sprite_byte >> (7 - col)) & 0x1;
            uint32_t display_index = (y_coord + row) * CHIP8_SCREEN_WIDTH + (x_coord + col);

            if (sprite_pixel) {
                if (system->display[display_index]) {
                    system->cpu.V[0xF] = 1;
                }
                system->display[display_index] ^= true;
            }
        }
    }
}

void opcode_0xE( struct system *system, struct opcode op_code ) {
    uint8_t key = system->cpu.V[op_code.X] & 0x0F;

    if (op_code.NN == 0x9E) {
        if (system->keyboard[key]) {
            system->cpu.program_counter += 2;
        }
    } else if (op_code.NN == 0xA1) {
        if (!system->keyboard[key]) {
            system->cpu.program_counter += 2;
        }
    }
}

void opcode_0xF( struct system *system, struct opcode op_code ) {
    switch (op_code.NN) {
        case 0x07:
            system->cpu.V[op_code.X] = system->delay_timer;
            break;

        case 0x0A: {
            bool key_pressed = false;
            for (uint8_t i = 0; i < 16; i++) {
                if (system->keyboard[i]) {
                    system->cpu.V[op_code.X] = i;
                    key_pressed = true;
                    break;
                }
            }
            if (!key_pressed) {
                system->cpu.program_counter -= 2;
            }
            break;
        }

        case 0x15:
            system->delay_timer = system->cpu.V[op_code.X];
            break;

        case 0x18:
            system->sound_timer = system->cpu.V[op_code.X];
            break;

        case 0x1E:
            system->cpu.I += system->cpu.V[op_code.X];
            break;

        case 0x29:
            system->cpu.I = (system->cpu.V[op_code.X] & 0x0F) * 5;
            break;

        case 0x33: {
            uint8_t value = system->cpu.V[op_code.X];
            system->memory[system->cpu.I]     = value / 100;
            system->memory[system->cpu.I + 1] = (value / 10) % 10;
            system->memory[system->cpu.I + 2] = value % 10;
            break;
        }

        case 0x55:
            for (uint8_t i = 0; i <= op_code.X; i++) {
                system->memory[system->cpu.I + i] = system->cpu.V[i];
            }
            break;

        case 0x65:
            for (uint8_t i = 0; i <= op_code.X; i++) {
                system->cpu.V[i] = system->memory[system->cpu.I + i];
            }
            break;
    }
}

void create_opcode_hashmap(struct system* system) {

    system->op_hashmap[0x0].exec = opcode_0x0;
    system->op_hashmap[0x1].exec = opcode_0x1;
    system->op_hashmap[0x2].exec = opcode_0x2;
    system->op_hashmap[0x3].exec = opcode_0x3;
    system->op_hashmap[0x4].exec = opcode_0x4;
    system->op_hashmap[0x5].exec = opcode_0x5;
    system->op_hashmap[0x6].exec = opcode_0x6;
    system->op_hashmap[0x7].exec = opcode_0x7;
    system->op_hashmap[0x8].exec = opcode_0x8;
    system->op_hashmap[0x9].exec = opcode_0x9;
    system->op_hashmap[0xA].exec = opcode_0xA;
    system->op_hashmap[0xB].exec = opcode_0xB;
    system->op_hashmap[0xC].exec = opcode_0xC;
    system->op_hashmap[0xD].exec = opcode_0xD;
    system->op_hashmap[0xE].exec = opcode_0xE;
    system->op_hashmap[0xF].exec = opcode_0xF;

}
