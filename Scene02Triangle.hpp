//
// Created by Gaëtan Blaise-Cazalet on 19/11/2024.
//

#ifndef SCENE02TRIANGLE_HPP
#define SCENE02TRIANGLE_HPP

#include <SDL3/SDL_gpu.h>
#include "Scene.hpp"

class Scene02Triangle : public Scene {
public:
    void Load(Renderer& renderer) override;
    bool Update(float dt) override;
    void Draw(Renderer& renderer) override;
    void Unload() override;

private:
    InputState inputState;
    const char* basePath;
    SDL_GPUShader* vertexShader;
    SDL_GPUShader* fragmentShader;
    SDL_GPUGraphicsPipeline* FillPipeline;
    SDL_GPUGraphicsPipeline* LinePipeline;
    SDL_GPUViewport SmallViewport = { 160, 120, 320, 240, 0.1f, 1.0f };
    SDL_Rect ScissorRect = { 320, 240, 320, 240 };

    bool UseWireframeMode = false;
    bool UseSmallViewport = false;
    bool UseScissorRect = false;
};



#endif //SCENE02TRIANGLE_HPP
