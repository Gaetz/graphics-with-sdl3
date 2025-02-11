//
// Created by Gaëtan Blaise-Cazalet on 10/02/2025.
//

#include "Scene09BasicCompute.hpp"
#include "Renderer.hpp"
#include <SDL3/SDL.h>

#include "PositionTextureVertex.hpp"

void Scene09BasicCompute::Load(Renderer& renderer) {
    basePath = SDL_GetBasePath();
    vertexShader = renderer.LoadShader(basePath, "TexturedQuad.vert", 0, 0, 0, 0);
    fragmentShader = renderer.LoadShader(basePath, "TexturedQuad.frag", 1, 0, 0, 0);

    // Create the pipelines
    // -- Compute pipeline
    SDL_GPUComputePipelineCreateInfo computePipelineCreateInfo = {
        .num_readwrite_storage_textures = 1,
        .threadcount_x = 8,
        .threadcount_y = 8,
        .threadcount_z = 1,
    };
    computePipeline = renderer.CreateComputePipelineFromShader(basePath, "FillTexture.comp", &computePipelineCreateInfo);


    // -- Graphics pipeline
    SDL_GPUGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
        .vertex_shader = vertexShader,
        .fragment_shader = fragmentShader,
        .vertex_input_state = {
            .vertex_buffer_descriptions = new SDL_GPUVertexBufferDescription[1] {{
                .slot = 0,
                .pitch = sizeof(PositionTextureVertex),
                .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                .instance_step_rate = 0,
            }},
            .num_vertex_buffers = 1,
            .vertex_attributes = new SDL_GPUVertexAttribute[2]{{
                .location = 0,
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                .offset = 0
            }, {
                .location = 1,
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
                .offset = sizeof(float) * 3
            }},
            .num_vertex_attributes = 2,
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .target_info = {
            .color_target_descriptions = new SDL_GPUColorTargetDescription[1] {{
                .format = SDL_GetGPUSwapchainTextureFormat(renderer.device, renderer.renderWindow)
            }},
            .num_color_targets = 1,
        },

    };

    graphicsPipeline = SDL_CreateGPUGraphicsPipeline(renderer.device, &graphicsPipelineCreateInfo);
    if (graphicsPipeline == nullptr)
    {
        SDL_Log("Failed to create fill pipeline!");
    }

    // Clean up shader resources
    SDL_ReleaseGPUShader(renderer.device, vertexShader);
    SDL_ReleaseGPUShader(renderer.device, fragmentShader);

    // Screen texture
    int w, h;
    SDL_GetWindowSizeInPixels(renderer.renderWindow, &w, &h);

    screenTexture = renderer.CreateTexture(SDL_GPUTextureCreateInfo {
        .type = SDL_GPU_TEXTURETYPE_2D,
        .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
        .usage = SDL_GPU_TEXTUREUSAGE_COMPUTE_STORAGE_WRITE | SDL_GPU_TEXTUREUSAGE_SAMPLER,
        .width = static_cast<Uint32>(w),
        .height = static_cast<Uint32>(h),
        .layer_count_or_depth = 1,
        .num_levels = 1,
    });
    sampler = renderer.CreateSampler(SDL_GPUSamplerCreateInfo {
        .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
        .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT
    });

    // Set the buffer data
    // Create the vertex buffer
    SDL_GPUBufferCreateInfo vertexBufferCreateInfo = {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = sizeof(PositionTextureVertex) * 4
    };
    vertexBuffer = renderer.CreateBuffer(vertexBufferCreateInfo);

    // Create the index buffer
    SDL_GPUBufferCreateInfo indexBufferCreateInfo = {
        .usage = SDL_GPU_BUFFERUSAGE_INDEX,
        .size = sizeof(Uint16) * 6
    };
    indexBuffer = renderer.CreateBuffer(indexBufferCreateInfo);

    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = (sizeof(PositionTextureVertex) * 4) + (sizeof(Uint16) * 6),
    };
    SDL_GPUTransferBuffer* transferBuffer = renderer.CreateTransferBuffer(transferBufferCreateInfo);

    // Map the transfer buffer and fill it with data (data is bound to the transfer buffer)
    auto transferData = static_cast<PositionTextureVertex *>(
        renderer.MapTransferBuffer(transferBuffer, false)
    );
	transferData[0] = PositionTextureVertex { -1,  1, 0, 0, 0 };
	transferData[1] = PositionTextureVertex {  1,  1, 0, 1, 0 };
	transferData[2] = PositionTextureVertex {  1, -1, 0, 1, 1 };
	transferData[3] = PositionTextureVertex { -1, -1, 0, 0, 1 };
    auto indexData = reinterpret_cast<Uint16*>(&transferData[4]);
	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;
	indexData[3] = 0;
	indexData[4] = 2;
	indexData[5] = 3;
    renderer.UnmapTransferBuffer(transferBuffer);

	// Setup texture transfer buffer
    renderer.BeginUploadToBuffer();
	// Upload the transfer data to the vertex and index buffer
    SDL_GPUTransferBufferLocation transferVertexBufferLocation {
        .transfer_buffer = transferBuffer,
        .offset = 0
    };
    SDL_GPUBufferRegion vertexBufferRegion {
        .buffer = vertexBuffer,
        .offset = 0,
        .size = sizeof(PositionTextureVertex) * 4
    };
    SDL_GPUTransferBufferLocation transferIndexBufferLocation {
        .transfer_buffer = transferBuffer,
        .offset = sizeof(PositionTextureVertex) * 4
    };
    SDL_GPUBufferRegion indexBufferRegion {
        .buffer = indexBuffer,
        .offset = 0,
        .size = sizeof(Uint16) * 6
    };

    renderer.UploadToBuffer(transferVertexBufferLocation, vertexBufferRegion, false);
    renderer.UploadToBuffer(transferIndexBufferLocation, indexBufferRegion, false);
    renderer.EndUploadToBuffer(transferBuffer);

    // Execute compute shader to fill the texture
    SDL_GPUStorageTextureReadWriteBinding storageTexture {
        .texture = screenTexture
    };
    renderer.BeginCompute(&storageTexture, 1, nullptr, 0);
    renderer.BindComputePipeline(computePipeline);
    renderer.DispatchCompute(w / 8, h / 8, 1);
    renderer.EndCompute();
    renderer.ReleaseComputePipeline(computePipeline);
}

bool Scene09BasicCompute::Update(float dt) {
    const bool isRunning = ManageInput(inputState);

    return isRunning;
}

void Scene09BasicCompute::Draw(Renderer& renderer) {
    renderer.Begin();

    renderer.BindGraphicsPipeline(graphicsPipeline);
    SDL_GPUBufferBinding vertexBindings { .buffer = vertexBuffer, .offset = 0 };
    renderer.BindVertexBuffers(0, vertexBindings, 1);
    SDL_GPUBufferBinding indexBindings { .buffer = indexBuffer, .offset = 0 };
    renderer.BindIndexBuffer(indexBindings, SDL_GPU_INDEXELEMENTSIZE_16BIT);
    renderer.BindFragmentSamplers(0, SDL_GPUTextureSamplerBinding { .texture = screenTexture, .sampler = sampler }, 1);
    renderer.DrawIndexedPrimitives(6, 1, 0, 0, 0);

    renderer.End();
}

void Scene09BasicCompute::Unload(Renderer& renderer) {
    renderer.ReleaseBuffer(vertexBuffer);
    renderer.ReleaseBuffer(indexBuffer);
    renderer.ReleaseSampler(sampler);
    renderer.ReleaseTexture(screenTexture);
    renderer.ReleaseGraphicsPipeline(graphicsPipeline);
}