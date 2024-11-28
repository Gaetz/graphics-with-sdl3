//
// Created by Gaëtan Blaise-Cazalet on 20/11/2024.
//

#ifndef SCENE06TRIANGLEINSTANCES_HPP
#define SCENE06TRIANGLEINSTANCES_HPP

#include <SDL3/SDL_gpu.h>
#include "Scene.hpp"
#include <array>
#include <string>

using std::array;
using std::string;

class Scene06TriangleInstances : public Scene {
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

    SDL_GPUGraphicsPipeline* pipeline;
    SDL_GPUBuffer* vertexBuffer;
    SDL_GPUBuffer* indexBuffer;
    bool useVertexOffset = false;
    bool useIndexOffset = false;
    bool useIndexBuffer = true;
};



#endif //SCENE06TRIANGLEINSTANCES_HPP
