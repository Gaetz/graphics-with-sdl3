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

SDL_GPUShader* Renderer::LoadShader(
	const char* basePath,
	const char* shaderFilename,
	Uint32 samplerCount,
	Uint32 uniformBufferCount,
	Uint32 storageBufferCount,
	Uint32 storageTextureCount
) {
	// Auto-detect the shader stage from the file name for convenience
	SDL_GPUShaderStage stage;
	if (SDL_strstr(shaderFilename, ".vert"))
	{
		stage = SDL_GPU_SHADERSTAGE_VERTEX;
	}
	else if (SDL_strstr(shaderFilename, ".frag"))
	{
		stage = SDL_GPU_SHADERSTAGE_FRAGMENT;
	}
	else
	{
		SDL_Log("Invalid shader stage!");
		return nullptr;
	}

	char fullPath[256];
	SDL_GPUShaderFormat backendFormats = SDL_GetGPUShaderFormats(device);
	SDL_GPUShaderFormat format = SDL_GPU_SHADERFORMAT_INVALID;
	const char *entrypoint;

	if (backendFormats & SDL_GPU_SHADERFORMAT_SPIRV) {
		SDL_snprintf(fullPath, sizeof(fullPath), "%sContent/Shaders/Compiled/SPIRV/%s.spv", basePath, shaderFilename);
		format = SDL_GPU_SHADERFORMAT_SPIRV;
		entrypoint = "main";
	} else if (backendFormats & SDL_GPU_SHADERFORMAT_MSL) {
		SDL_snprintf(fullPath, sizeof(fullPath), "%sContent/Shaders/Compiled/MSL/%s.msl", basePath, shaderFilename);
		format = SDL_GPU_SHADERFORMAT_MSL;
		entrypoint = "main0";
	} else if (backendFormats & SDL_GPU_SHADERFORMAT_DXIL) {
		SDL_snprintf(fullPath, sizeof(fullPath), "%sContent/Shaders/Compiled/DXIL/%s.dxil", basePath, shaderFilename);
		format = SDL_GPU_SHADERFORMAT_DXIL;
		entrypoint = "main";
	} else {
		SDL_Log("%s", "Unrecognized backend shader format!");
		return nullptr;
	}

	size_t codeSize;
	void* code = SDL_LoadFile(fullPath, &codeSize);
	if (code == nullptr)
	{
		SDL_Log("Failed to load shader from disk! %s", fullPath);
		return nullptr;
	}

	SDL_GPUShaderCreateInfo shaderInfo = {
		.code_size = codeSize,
		.code = static_cast<Uint8*>(code),
		.entrypoint = entrypoint,
		.format = format,
		.stage = stage,
		.num_samplers = samplerCount,
		.num_storage_textures = storageTextureCount,
		.num_storage_buffers = storageBufferCount,
		.num_uniform_buffers = uniformBufferCount
	};
	SDL_GPUShader* shader = SDL_CreateGPUShader(device, &shaderInfo);
	if (shader == nullptr)
	{
		SDL_Log("Failed to create shader!");
		SDL_free(code);
		return nullptr;
	}

	SDL_free(code);
	return shader;
}

void Renderer::BindGraphicsPipeline(SDL_GPUGraphicsPipeline* pipeline) const {
	SDL_BindGPUGraphicsPipeline(renderPass, pipeline);
}

void Renderer::SetGPUViewport(const SDL_GPUViewport& viewport) const {
	SDL_SetGPUViewport(renderPass, &viewport);
}
void Renderer::SetGPUScissorRect(const SDL_Rect& rect) const {
	SDL_SetGPUScissor(renderPass, &rect);
}

void Renderer::DrawGPUPrimitive(int numVertices, int numInstances, int firstVertex, int firstInstance) const {
	SDL_DrawGPUPrimitives(renderPass, numVertices, numInstances, firstVertex, firstInstance);
}

