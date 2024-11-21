//
// Created by Gaëtan Blaise-Cazalet on 21/11/2024.
//

#ifndef SCENE05TRIANGLESTENCIL_HPP
#define SCENE05TRIANGLESTENCIL_HPP

#include <SDL3/SDL_gpu.h>
#include "Scene.hpp"

class Scene05TriangleStencil : public Scene {
public:
    void Load(Renderer& renderer) override;
    bool Update(float dt) override;
    void Draw(Renderer& renderer) override;
    void Unload(Renderer& renderer) override;

private:
    InputState inputState;
    const char* basePath;
    SDL_GPUShader* vertexShader;
    SDL_GPUShader* fragmentShader;
    SDL_GPUGraphicsPipeline* maskeePipeline;
    SDL_GPUGraphicsPipeline* maskerPipeline;
    static SDL_GPUBuffer* vertexBuffer;
    static SDL_GPUTexture* depthStencilTexture;
};



#endif //SCENE05TRIANGLESTENCIL_HPP
