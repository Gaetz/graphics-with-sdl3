//
// Created by Admin on 19/11/2024.
//

#include "Window.hpp"

#include <string>
#include <SDL3/SDL.h>
using std::string;

void Window::Init() {
    SDL_Init(SDL_INIT_VIDEO);
    const string title { "Computer Graphics with SDL3" };
    sdlWindow = SDL_CreateWindow(title.c_str(), width, height, SDL_WINDOW_RESIZABLE);
}

void Window::Close() const {
    SDL_DestroyWindow(sdlWindow);
    SDL_Quit();
}
