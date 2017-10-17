#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "FileWatcher.h"
#include "Renderer.h"

#include <iostream>
#include <thread>

//==============================================================================
int main(int argc, char *argv[])
{
  // Initialize SDL
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_Window *window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
  SDL_GLContext context = SDL_GL_CreateContext(window);

  // Initialize GLEW
  glewExperimental = GL_TRUE;
  glewInit();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);

  Renderer renderer;
  renderer.initialize();

  unsigned long currentTime = SDL_GetTicks();
  unsigned long lastTime = currentTime;
  float deltaTime = 0;

  FileWatcher watcher("shader/obj/");
  std::thread producerThread(&FileWatcher::run, &watcher);

  bool quit = false;
  while (!quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = true;
      }
      if (event.type == SDL_KEYDOWN) {
        switch (event.key.keysym.sym) {
        case SDLK_l:
          renderer.objProgram().recompile();
          break;
        }
      }
      renderer.camera().orbitController(event);
    }

    Buffer& b = watcher.buffer();
    while (b.hasItem()) {
      renderer.objProgram().recompile();
      b.removeAll();
    }

    currentTime = SDL_GetTicks();
    deltaTime = ((float)currentTime - (float)lastTime) / 1000.0f;
    lastTime = SDL_GetTicks();

    //renderer.add(obj);

    renderer.render();

    SDL_GL_SwapWindow(window);
  }

  SDL_GL_DeleteContext(context);
  SDL_Quit();

  watcher.doQuit();
  producerThread.join();

  return 0;
}
