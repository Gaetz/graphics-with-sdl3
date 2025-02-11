//
// Created by Gaëtan Blaise-Cazalet on 11/02/2025.
//

#ifndef SCENE10UNIFORMSCOMPUTE_HPP
#define SCENE10UNIFORMSCOMPUTE_HPP

#include <SDL3/SDL_gpu.h>
#include "Scene.hpp"

struct GradientUniforms
{
    float time;
};

class Scene10UniformsCompute : public Scene {
public:
    void Load(Renderer& renderer) override;
    bool Update(float dt) override;
    void Draw(Renderer& renderer) override;
    void Unload(Renderer& renderer) override;

    static GradientUniforms gradientUniformValues;

private:
    InputState inputState;
    const char* basePath {nullptr};

    SDL_GPUComputePipeline* computePipeline {nullptr};
    SDL_GPUTexture* gradientTexture {nullptr};
    int w, h;
};



#endif //SCENE10UNIFORMSCOMPUTE_HPP
