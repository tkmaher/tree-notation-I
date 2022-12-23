//
//  main.cpp
//  tree-widget
//
//  Created by user on 12/18/22.
//

#include "program.h"

int main(int argc, char *argv[])
{
    

    // returns zero on success else non-zero
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0 || TTF_Init() != 0) {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    SDL_Window* win = SDL_CreateWindow("tree widget", // creates a window
                                       SDL_WINDOWPOS_CENTERED,
                                    SDL_WINDOWPOS_CENTERED,
                                    1000, 800, 0);

    // triggers the program that controls
    // your graphics hardware and sets flags
    Uint32 render_flags = SDL_RENDERER_ACCELERATED;
 
    // creates a renderer to render our images
    SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
 
    const Uint8* keystates = SDL_GetKeyboardState(NULL);

    SDL_Cursor* cursor;
    cursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_CROSSHAIR);
    SDL_SetCursor(cursor);
        
    Program p(rend, keystates);
    p.mainLoop();

    // destroy renderer
    SDL_DestroyRenderer(rend);
    
    // destroy window
    SDL_DestroyWindow(win);
    
    // close SDL
    SDL_Quit();
 
    return 0;
}
