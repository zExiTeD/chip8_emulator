#include <stdbool.h>
#include <SDL.h>

#include "./include/allocator.h"
#include "./include/helper.h"
#include "./include/system.h"
#include "./include/system.c"

#define DEFAULT_ARENA_SIZE (4096 * 2)

struct emulator_state {
  bool              running;
  int               window_width;
  int               window_height;
  const char*       title;
  SDL_Window*       window;
  SDL_Renderer*     renderer;
  SDL_Event         event;

  struct system     system;
  struct allocator  *allocator;
};

struct emulator_state emulator_initialize_state(int size_of_arena);
void print_help( void );

void state_disp_update( struct emulator_state* state ) ;
void state_kyb_update( struct emulator_state* state );

int
main(int argc, char *argv[]) {

  struct emulator_state state = emulator_initialize_state(DEFAULT_ARENA_SIZE);
  file_t file = open_and_read_entire_file("test_programs\\tetris.ch8", state.allocator);

  state.system = system_initialize( state.allocator, file);

  Uint32 last_cycle_time = SDL_GetTicks();
  Uint32 last_timer_time = SDL_GetTicks();
  float cycle_delay = 1000.0f / CLOCK_SPEED;
  float timer_delay = 1000.0f / TIMER_FREQUENCY;


  while (state.running) {
    while (SDL_PollEvent(&state.event)) {
      if (state.event.type == SDL_QUIT) {
        state.running = false;
      }
    }
    state_kyb_update(&state);

    Uint32 current_time = SDL_GetTicks();
    if (current_time - last_cycle_time >= cycle_delay) {
        system_execute_opcode(&state.system);
        last_cycle_time = current_time;
    }


    SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 255);
    SDL_RenderClear(state.renderer);

    state_disp_update(&state);
    SDL_RenderPresent(state.renderer);

    //if( state.system.cpu.program_counter > 4095){
    //    break;
    //}
  }

  SDL_DestroyWindow(state.window);
  SDL_Quit();

  return 0;
}

struct emulator_state
emulator_initialize_state(int size_of_arena) {
  int window_h = 800;
  int window_w = 600;
  const char* title = "chip-8 emulator";

  SDL_Init(SDL_INIT_VIDEO);
  SDL_Window* window = SDL_CreateWindow(
      title,
      SDL_WINDOWPOS_CENTERED,
      SDL_WINDOWPOS_CENTERED,
      window_w,
      window_h,
      SDL_WINDOW_SHOWN
      );
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  struct allocator *allocator = allocator_create_arena_allocator(size_of_arena);

  return (struct emulator_state) {
      .window_width = window_w,
      .window_height = window_h,
      .title = title,
      .running = true,
      .window = window,
      .renderer = renderer,
      .allocator = allocator,
  };
}

void print_help( void ) {
  printf("Usage -> ./program <filename> \n");
}

void state_disp_update( struct emulator_state* state ) {
    SDL_Renderer* renderer = state->renderer;

    int scale_factor = 10;

    for (int y = 0; y < CHIP8_SCREEN_HEIGHT; y++) {
      for (int x = 0; x < CHIP8_SCREEN_WIDTH; x++) {
        int index = y * CHIP8_SCREEN_WIDTH + x;

        if (state->system.display[index]) {
          SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        } else {
          SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        }

        SDL_Rect rect = {
          x * scale_factor,
          y * scale_factor,
          scale_factor,
          scale_factor
        };
        SDL_RenderFillRect(renderer, &rect);
      }
    }
}

void state_kyb_update( struct emulator_state* state ){
    const Uint8* keys = SDL_GetKeyboardState(NULL);

    state->system.keyboard[0x1] = keys[SDL_SCANCODE_1];  // 1
    state->system.keyboard[0x2] = keys[SDL_SCANCODE_2];  // 2
    state->system.keyboard[0x3] = keys[SDL_SCANCODE_3];  // 3
    state->system.keyboard[0xC] = keys[SDL_SCANCODE_4];  // C

    state->system.keyboard[0x4] = keys[SDL_SCANCODE_Q];  // 4
    state->system.keyboard[0x5] = keys[SDL_SCANCODE_W];  // 5
    state->system.keyboard[0x6] = keys[SDL_SCANCODE_E];  // 6
    state->system.keyboard[0xD] = keys[SDL_SCANCODE_R];  // D

    state->system.keyboard[0x7] = keys[SDL_SCANCODE_A];  // 7
    state->system.keyboard[0x8] = keys[SDL_SCANCODE_S];  // 8
    state->system.keyboard[0x9] = keys[SDL_SCANCODE_D];  // 9
    state->system.keyboard[0xE] = keys[SDL_SCANCODE_F];  // E

    state->system.keyboard[0xA] = keys[SDL_SCANCODE_Z];  // A
    state->system.keyboard[0x0] = keys[SDL_SCANCODE_X];  // 0
    state->system.keyboard[0xB] = keys[SDL_SCANCODE_C];  // B
    state->system.keyboard[0xF] = keys[SDL_SCANCODE_V];  // F


}
