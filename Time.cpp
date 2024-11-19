//
// Created by Gaëtan Blaise-Cazalet on 19/11/2024.
//

#include "Time.hpp"
#include <SDL3/SDL.h>

float Time::ComputeDeltaTime() {
    frameStart = SDL_GetTicks();
    unsigned int dt = frameStart - lastFrame;
    lastFrame = frameStart;
    return static_cast<float>(dt) / 1000.0f;
}

void Time::DelayTime() {
    frameTime = SDL_GetTicks() - frameStart;
    if (frameTime < frameDelay) {
        SDL_Delay(frameDelay - frameTime);
    }
}