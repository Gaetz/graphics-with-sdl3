//
// Created by Gaëtan Blaise-Cazalet on 19/11/2024.
//

#ifndef RENDERER_HPP
#define RENDERER_HPP

#include <SDL3/SDL_gpu.h>
#include <vector>
#include <string>

using std::vector;
using std::string;

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

    SDL_Surface* LoadBMPImage(const char* basePath, const char* imageFilename, int desiredChannels);
    SDL_GPUSampler* CreateSampler(const SDL_GPUSamplerCreateInfo& createInfo) const;
    void ReleaseSurface(SDL_Surface* surface) const;
    SDL_GPUTexture* CreateTexture(const SDL_GPUTextureCreateInfo& createInfo) const;
    void SetTextureName(SDL_GPUTexture* texture, const string& name) const;
    void ReleaseTexture(SDL_GPUTexture* texture) const;


    SDL_GPUGraphicsPipeline* CreateGPUGraphicsPipeline(const SDL_GPUGraphicsPipelineCreateInfo& createInfo) const;
    void BindGraphicsPipeline(SDL_GPUGraphicsPipeline* pipeline) const;
    void BindVertexBuffers(Uint32 firstSlot, const SDL_GPUBufferBinding& bindings, Uint32 numBindings) const;
    void BindIndexBuffer(const SDL_GPUBufferBinding& bindings, SDL_GPUIndexElementSize indexElementSize) const;
    void BindFragmentSamplers(Uint32 firstSlot, const SDL_GPUTextureSamplerBinding& bindings, Uint32 numBindings) const;

    void DrawPrimitives(int numVertices, int numInstances, int firstVertex, int firstInstance) const;
    void DrawIndexedPrimitives(int numIndices, int numInstances, int firstIndex, int vertexOffset, int firstInstance) const;

    void SetViewport(const SDL_GPUViewport& viewport) const;
    void SetScissorRect(const SDL_Rect& rect) const;
    void SetStencilReference(Uint8 stencilReference) const;
    bool DoesTextureSupportFormat(SDL_GPUTextureFormat format, SDL_GPUTextureType type, SDL_GPUTextureUsageFlags usageFlags) const;


    SDL_GPUBuffer* CreateBuffer(const SDL_GPUBufferCreateInfo& createInfo) const;
    void SetBufferName(SDL_GPUBuffer* buffer, const string& name) const;
    SDL_GPUTransferBuffer* CreateTransferBuffer(const SDL_GPUTransferBufferCreateInfo& createInfo) const;
    void* MapTransferBuffer(SDL_GPUTransferBuffer* transferBuffer, bool cycle) const;
    void UnmapTransferBuffer(SDL_GPUTransferBuffer* transferBuffer) const;
    void ReleaseTransferBuffer(SDL_GPUTransferBuffer* transferBuffer) const;

    void BeginUploadToBuffer();
    void UploadToBuffer(const SDL_GPUTransferBufferLocation& source,
                           const SDL_GPUBufferRegion& destination, bool cycle) const;
    void UploadToTexture(const SDL_GPUTextureTransferInfo& source,
                           const SDL_GPUTextureRegion& destination, bool cycle) const;
    void EndUploadToBuffer(SDL_GPUTransferBuffer* transferBuffer) const;

    void ReleaseBuffer(SDL_GPUBuffer* buffer) const;
    void ReleaseGraphicsPipeline(SDL_GPUGraphicsPipeline* pipeline) const;

    void PushVertexUniformData(uint32_t slot, const void* data, Uint32 size) const;
    void PushFragmentUniformData(uint32_t slot, const void* data, Uint32 size) const;

    SDL_GPUDevice* device {nullptr};
    SDL_Window* renderWindow {nullptr};
    SDL_GPUCommandBuffer* cmdBuffer {nullptr};
    SDL_GPUTexture* swapchainTexture {nullptr};
    SDL_GPURenderPass* renderPass {nullptr};

    SDL_GPUCommandBuffer* uploadCmdBuf {nullptr};
    SDL_GPUCopyPass* copyPass {nullptr};
};



#endif //RENDERER_HPP
