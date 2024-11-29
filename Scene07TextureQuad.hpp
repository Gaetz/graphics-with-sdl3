//
// Created by Gaëtan Blaise-Cazalet on 29/11/2024.
//

#ifndef SCENE07TEXTUREQUAD_HPP
#define SCENE07TEXTUREQUAD_HPP

#include <SDL3/SDL_gpu.h>
#include "Scene.hpp"
#include <array>
#include <string>

using std::array;
using std::string;

class Scene07TextureQuad : public Scene {
public:
    void Load(Renderer& renderer) override;
    bool Update(float dt) override;
    void Draw(Renderer& renderer) override;
    void Unload(Renderer& renderer) override;

private:
    array<string, 6> samplerNames {
        "PointClamp",
        "PointWrap",
        "LinearClamp",
        "LinearWrap",
        "AnisotropicClamp",
        "AnisotropicWrap",
    };

    InputState inputState;
    const char* basePath {nullptr};
    SDL_GPUShader* vertexShader {nullptr};
    SDL_GPUShader* fragmentShader {nullptr};

    SDL_GPUGraphicsPipeline* pipeline {nullptr};
    SDL_GPUBuffer* vertexBuffer {nullptr};
    SDL_GPUBuffer* indexBuffer {nullptr};
    SDL_GPUTexture* texture {nullptr};
    array<SDL_GPUSampler*, 6> samplers {};
};



#endif //SCENE07TEXTUREQUAD_HPP
