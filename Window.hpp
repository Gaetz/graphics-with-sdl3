//
// Created by Gaëtan Blaise-Cazalet on 19/11/2024.
//

#ifndef WINDOW_HPP
#define WINDOW_HPP
#include <SDL3/SDL_video.h>

class Window {
public:
    SDL_Window* sdlWindow;
    void Init();
    void Close() const;

    int width { 640 };
    int height { 480 };
};



#endif //WINDOW_HPP
