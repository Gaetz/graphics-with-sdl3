//
// Created by Gaëtan Blaise-Cazalet on 19/11/2024.
//

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL3/SDL_gpu.h>
#include <vector>

using std::vector;

class Window;

class Renderer {
public:
    void Init(Window &window);
    void Begin(SDL_GPUDepthStencilTargetInfo* depthStencilTargetInfo = nullptr);
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
    void ReleaseShader(SDL_GPUShader* shader) const;


    SDL_GPUGraphicsPipeline* CreateGPUGraphicsPipeline(const SDL_GPUGraphicsPipelineCreateInfo& createInfo) const;
    void BindGraphicsPipeline(SDL_GPUGraphicsPipeline* pipeline) const;
    void BindVertexBuffers(Uint32 firstSlot, const SDL_GPUBufferBinding& bindings, Uint32 numBindings) const;
    void DrawGPUPrimitive(int numVertices, int numInstances, int firstVertex, int firstInstance) const;

    void SetGPUViewport(const SDL_GPUViewport& viewport) const;
    void SetGPUScissorRect(const SDL_Rect& rect) const;
    void SetGPUStencilReference(Uint8 stencilReference) const;
    bool DoesTextureSupportFormat(SDL_GPUTextureFormat format, SDL_GPUTextureType type, SDL_GPUTextureUsageFlags usageFlags) const;


    SDL_GPUBuffer* CreateGPUBuffer(const SDL_GPUBufferCreateInfo& createInfo) const;
    SDL_GPUTransferBuffer* CreateGPUTransferBuffer(const SDL_GPUTransferBufferCreateInfo& createInfo) const;
    void* MapTransferBuffer(SDL_GPUTransferBuffer* transferBuffer, bool cycle) const;
    void UnmapTransferBuffer(SDL_GPUTransferBuffer* transferBuffer) const;

    SDL_GPUTexture* CreateTexture(const SDL_GPUTextureCreateInfo& createInfo) const;

    void BeginUploadToGPUBuffer();
    void UploadToGPUBuffer(const SDL_GPUTransferBufferLocation& source,
                           const SDL_GPUBufferRegion& destination, bool cycle) const;
    void EndUploadToGPUBuffer(SDL_GPUTransferBuffer* transferBuffer) const;

    void ReleaseBuffer(SDL_GPUBuffer* buffer) const;
    void ReleaseGraphicsPipeline(SDL_GPUGraphicsPipeline* pipeline) const;

    SDL_GPUDevice* device {nullptr};
    SDL_Window* renderWindow {nullptr};
    SDL_GPUCommandBuffer* cmdBuffer {nullptr};
    SDL_GPUTexture* swapchainTexture {nullptr};
    SDL_GPURenderPass* renderPass {nullptr};

    SDL_GPUCommandBuffer* uploadCmdBuf {nullptr};
    SDL_GPUCopyPass* copyPass {nullptr};
};



#endif //RENDERER_HPP
