//
// Created by Gaëtan Blaise-Cazalet on 19/11/2024.
//

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL3/SDL_gpu.h>

class Window;

class Renderer {
public:
    void Init(Window &window);
    void Begin();
    void End() const;
    void Close() const;

    SDL_GPUDevice* device;
    SDL_Window* renderWindow;
    SDL_GPUCommandBuffer* cmdBuffer;
    SDL_GPUTexture* swapchainTexture;
    SDL_GPURenderPass* renderPass;
};



#endif //RENDERER_HPP
