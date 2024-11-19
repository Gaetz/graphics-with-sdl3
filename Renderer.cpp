//
// Created by Gaëtan Blaise-Cazalet on 19/11/2024.
//

#include "Renderer.hpp"
#include "Window.hpp"
#include <SDL3/SDL_log.h>

void Renderer::Init(Window &window) {
    renderWindow = window.sdlWindow;
    device = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXIL | SDL_GPU_SHADERFORMAT_MSL,
        true,
        nullptr);
    SDL_ClaimWindowForGPUDevice(device, renderWindow);
}

void Renderer::Begin() {
    cmdBuffer = SDL_AcquireGPUCommandBuffer(device);
    if (cmdBuffer == nullptr)
    {
        SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
    }

    SDL_GPUTexture* swapchainTexture;
    if (!SDL_AcquireGPUSwapchainTexture(cmdBuffer, renderWindow, &swapchainTexture, nullptr, nullptr)) {
        SDL_Log("AcquireGPUSwapchainTexture failed: %s", SDL_GetError());
    }

    if (swapchainTexture != nullptr)
    {
        SDL_GPUColorTargetInfo colorTargetInfo = { 0 };
        colorTargetInfo.texture = swapchainTexture;
        colorTargetInfo.clear_color = SDL_FColor { 0.3f, 0.4f, 0.5f, 1.0f };
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

        renderPass = SDL_BeginGPURenderPass(cmdBuffer, &colorTargetInfo, 1, nullptr);
    }
}

void Renderer::End() const {
    SDL_EndGPURenderPass(renderPass);
    SDL_SubmitGPUCommandBuffer(cmdBuffer);
}

void Renderer::Close() const {
    SDL_ReleaseWindowFromGPUDevice(device, renderWindow);
    SDL_DestroyGPUDevice(device);
}
