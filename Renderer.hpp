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

    SDL_GPUShader* LoadShader(
        const char* basePath,
        const char* shaderFilename,
        Uint32 samplerCount,
        Uint32 uniformBufferCount,
        Uint32 storageBufferCount,
        Uint32 storageTextureCount
    );

    void BindGraphicsPipeline(SDL_GPUGraphicsPipeline* pipeline) const;
    void SetGPUViewport(const SDL_GPUViewport& viewport) const;
    void SetGPUScissorRect(const SDL_Rect& rect) const;
    void DrawGPUPrimitive(int, int, int, int) const;

    SDL_GPUDevice* device;
    SDL_Window* renderWindow;
    SDL_GPUCommandBuffer* cmdBuffer;
    SDL_GPUTexture* swapchainTexture;
    SDL_GPURenderPass* renderPass;
};



#endif //RENDERER_HPP
