//
// Created by Gaëtan Blaise-Cazalet on 19/11/2024.
//

#ifndef SCENE_HPP
#define SCENE_HPP

#include <SDL3/SDL_events.h>
#include "InputState.hpp"

class Renderer;

class Scene {
public:
    virtual ~Scene() {}

    virtual void Load(Renderer& renderer) = 0;

    virtual bool Update(float dt) = 0;

    virtual void Draw(Renderer& renderer) = 0;

    virtual void Unload(Renderer& renderer) = 0;

protected:
    static bool ManageInput(InputState &inputState) {
        inputState.previousLeft = inputState.left;
        inputState.previousRight = inputState.right;
        inputState.previousUp = inputState.up;
        inputState.previousDown = inputState.down;

        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT) { return false; }
            else if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_ESCAPE) { return false; }
                if (event.key.key == SDLK_LEFT) { inputState.left = true; }
                if (event.key.key == SDLK_RIGHT) { inputState.right = true; }
                if (event.key.key == SDLK_UP) { inputState.up = true; }
                if (event.key.key == SDLK_DOWN) { inputState.down = true; }
            }
            else if (event.type == SDL_EVENT_KEY_UP)
            {
                if (event.key.key == SDLK_LEFT) { inputState.left = false; }
                if (event.key.key == SDLK_RIGHT) { inputState.right = false; }
                if (event.key.key == SDLK_UP) { inputState.up = false; }
                if (event.key.key == SDLK_DOWN) { inputState.down = false; }
            }
        }
        return true;
    }
};

#endif //SCENE_HPP
