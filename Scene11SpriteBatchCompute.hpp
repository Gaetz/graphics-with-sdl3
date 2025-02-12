//
// Created by Gaëtan Blaise-Cazalet on 12/02/2025.
//

#ifndef SCENE11SPRITEBATCHCOMPUTE_HPP
#define SCENE11SPRITEBATCHCOMPUTE_HPP

#include <SDL3/SDL_gpu.h>
#include "Scene.hpp"
#include "Mat4.hpp"

struct PositionTextureColorVertex
{
    float x, y, z, w;
    float u, v, padding_a, padding_b;
    float r, g, b, a;
};

struct ComputeSpriteInstance
{
    float x, y, z;
    float rotation;
    float w, h, padding_a, padding_b;
    float r, g, b, a;
};

const Uint32 SPRITE_COUNT = 8192;

class Scene11SpriteBatchCompute : public Scene {
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
    SDL_GPUTexture* texture {nullptr};
    SDL_GPUSampler* sampler {nullptr};

    Mat4 viewProj;
    SDL_GPUBuffer* spriteComputeBuffer {nullptr};
    SDL_GPUBuffer* vertexBuffer {nullptr};
    SDL_GPUBuffer* indexBuffer {nullptr};
    SDL_GPUTransferBuffer* spriteComputeTransferBuffer {nullptr};
};



#endif //SCENE11SPRITEBATCHCOMPUTE_HPP
