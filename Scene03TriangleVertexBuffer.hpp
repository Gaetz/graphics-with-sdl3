//
// Created by Gaëtan Blaise-Cazalet on 19/11/2024.
//

#ifndef SCENE03TRIANGLEVERTEXBUFFER_HPP
#define SCENE03TRIANGLEVERTEXBUFFER_HPP

#include <SDL3/SDL_gpu.h>
#include "Scene.hpp"

class Scene03TriangleVertexBuffer : public Scene {
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
};



#endif //SCENE03TRIANGLEVERTEXBUFFER_HPP
