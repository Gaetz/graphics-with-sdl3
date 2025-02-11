//
// Created by Gaëtan Blaise-Cazalet on 10/02/2025.
//

#include "Scene10UniformsCompute.hpp"
#include "Renderer.hpp"
#include <SDL3/SDL.h>

#include "PositionTextureVertex.hpp"

GradientUniforms Scene10UniformsCompute::gradientUniformValues {};

void Scene10UniformsCompute::Load(Renderer& renderer) {
    basePath = SDL_GetBasePath();

    // Create the pipelines
    // -- Compute pipeline
    SDL_GPUComputePipelineCreateInfo computePipelineCreateInfo = {
            .num_readwrite_storage_textures = 1,
            .num_uniform_buffers = 1,
            .threadcount_x = 8,
            .threadcount_y = 8,
            .threadcount_z = 1,
    };
    computePipeline = renderer.CreateComputePipelineFromShader(basePath, "GradientTexture.comp",
                                                               &computePipelineCreateInfo);

    // Screen texture
    SDL_GetWindowSizeInPixels(renderer.renderWindow, &w, &h);

    gradientTexture = renderer.CreateTexture(SDL_GPUTextureCreateInfo {
            .type = SDL_GPU_TEXTURETYPE_2D,
            .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
            .usage = SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_WRITE | SDL_GPU_TEXTUREUSAGE_SAMPLER,
            .width = static_cast<Uint32>(w),
            .height = static_cast<Uint32>(h),
            .layer_count_or_depth = 1,
            .num_levels = 1,
    });
    gradientUniformValues.time = 0;
}

bool Scene10UniformsCompute::Update(float dt) {
    const bool isRunning = ManageInput(inputState);
    gradientUniformValues.time += 0.01f;

    return isRunning;
}

void Scene10UniformsCompute::Draw(Renderer& renderer) {

    renderer.AcquireCmdBufferAndSwapchainTexture(w, h);

    if (renderer.IsSwapchainTextureValid()) {
        SDL_GPUStorageTextureReadWriteBinding storageTexture {
                .texture = gradientTexture,
                .cycle = true
        };
        renderer.BeginCompute(&storageTexture, 1, nullptr, 0);
        renderer.BindComputePipeline(computePipeline);
        renderer.PushComputeUniformData(0, &gradientUniformValues, sizeof(GradientUniforms));
        renderer.DispatchCompute(w / 8, h / 8, 1);
        renderer.EndCompute();

        renderer.BlitSwapchainTexture(w, h, gradientTexture, w, h, SDL_GPU_FILTER_LINEAR);
    }

    renderer.SubmitCommandBuffer();
}

void Scene10UniformsCompute::Unload(Renderer& renderer) {
    renderer.ReleaseTexture(gradientTexture);
    renderer.ReleaseComputePipeline(computePipeline);
}