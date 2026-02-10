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

int
main(int argc, char *argv[]) {

  struct emulator_state state = emulator_initialize_state(DEFAULT_ARENA_SIZE);
  file_t file = open_and_read_entire_file("rom.ch8", state.allocator);

  struct system system = system_initialize( state.allocator, file);

  while (state.running) {
    while (SDL_PollEvent(&state.event)) {
      if (state.event.type == SDL_QUIT) {
        state.running = false;
      }
    }

    SDL_SetRenderDrawColor(state.renderer, 0, 0, 0, 255);
    SDL_RenderClear(state.renderer);

    SDL_RenderPresent(state.renderer);
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
