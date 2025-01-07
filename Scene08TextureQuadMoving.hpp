//
// Created by Gaëtan Blaise-Cazalet on 19/12/2024.
//

#ifndef SCENE08TEXTUREQUADMOVING_HPP
#define SCENE08TEXTUREQUADMOVING_HPP

#include <SDL3/SDL_gpu.h>
#include "Scene.hpp"
#include <array>
#include <string>

using std::array;
using std::string;

typedef struct FragMultiplyUniform
{
    float r, g, b, a;
} fragMultiplyUniform;

class Scene08TextureQuadMoving : public Scene {
public:
    void Load(Renderer& renderer) override;
    bool Update(float dt) override;
    void Draw(Renderer& renderer) override;
    void Unload(Renderer& renderer) override;

private:
    InputState inputState;
    const char* basePath {nullptr};
    SDL_GPUShader* vertexShader {nullptr};
    SDL_GPUShader* fragmentShader {nullptr};

    SDL_GPUGraphicsPipeline* pipeline {nullptr};
    SDL_GPUBuffer* vertexBuffer {nullptr};
    SDL_GPUBuffer* indexBuffer {nullptr};
    SDL_GPUTexture* texture {nullptr};
    SDL_GPUSampler* sampler {nullptr};
    float time {0};
};

#endif //SCENE08TEXTUREQUADMOVING_HPP
