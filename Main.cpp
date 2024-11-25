//
// Created by Gaëtan Blaise-Cazalet on 18/11/2024.
//

#include <iostream>
#include <SDL3/SDL_main.h>

#include "Renderer.hpp"
#include "Scene01Clear.hpp"
#include "Scene02Triangle.hpp"
#include "Scene03TriangleVertexBuffer.hpp"
#include "Scene04TriangleCullModes.hpp"
#include "Scene05TriangleStencil.hpp"
#include "Time.hpp"
#include "Window.hpp"

using namespace std;

int main(int argc, char **argv) {
    Window window {};
    Renderer renderer {};
    Time time {};
    window.Init();
    renderer.Init(window);

    auto scene = std::make_unique<Scene05TriangleStencil>();
    scene->Load(renderer);

    bool isRunning { true };
    while (isRunning) {
        const float dt = time.ComputeDeltaTime();

        isRunning = scene->Update(dt);
        scene->Draw(renderer);

        time.DelayTime();
    }

    scene->Unload(renderer);

    renderer.Close();
    window.Close();
    return 0;
}