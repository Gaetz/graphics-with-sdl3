//
// Created by Gaëtan Blaise-Cazalet on 19/11/2024.
//

#ifndef SCENE_HPP
#define SCENE_HPP

#include <SDL3/SDL_events.h>

class Renderer;

class Scene {
public:
    virtual ~Scene() {}
    void virtual Load() = 0;
    bool virtual Update(float dt) = 0;
    void virtual Draw(Renderer& renderer) = 0;
    void virtual Unload() = 0;

protected:
    static bool ManageInput() {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            if (event.type == SDL_EVENT_QUIT) { return false; } else if (event.type == SDL_EVENT_KEY_DOWN)
            {
                if (event.key.key == SDLK_ESCAPE) { return false; }
            }
        }
        return true;
    }
};

#endif //SCENE_HPP
