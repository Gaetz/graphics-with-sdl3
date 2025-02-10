//
// Created by Gaëtan Blaise-Cazalet on 10/02/2025.
//

#ifndef SCENE09BASICCOMPUTE_HPP
#define SCENE09BASICCOMPUTE_HPP

#include <SDL3/SDL_gpu.h>
#include "Scene.hpp"

class Scene09BasicCompute : public Scene {
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
    SDL_GPUGraphicsPipeline* graphicsPipeline {nullptr};

    SDL_GPUComputePipeline* computePipeline {nullptr};
    SDL_GPUTexture* screenTexture {nullptr};
    SDL_GPUSampler* sampler {nullptr};
    SDL_GPUBuffer* vertexBuffer {nullptr};
    SDL_GPUBuffer* indexBuffer {nullptr};
};



#endif //SCENE09BASICCOMPUTE_HPP
