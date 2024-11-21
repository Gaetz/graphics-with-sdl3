//
// Created by Gaëtan Blaise-Cazalet on 19/11/2024.
//

#include "Scene03TriangleVertexBuffer.hpp"
#include "Renderer.hpp"
#include "PositionColorVertex.hpp"
#include <SDL3/SDL.h>

void Scene03TriangleVertexBuffer::Load(Renderer& renderer) {
    basePath = SDL_GetBasePath();
    vertexShader = renderer.LoadShader(basePath, "PositionColor.vert", 0, 0, 0, 0);
    fragmentShader = renderer.LoadShader(basePath, "SolidColor.frag", 0, 0, 0, 0);

    // Create the pipeline
    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .vertex_shader = vertexShader,
        .fragment_shader = fragmentShader,
        // This is set up to match the vertex shader layout!
        .vertex_input_state = SDL_GPUVertexInputState {
            .vertex_buffer_descriptions = new SDL_GPUVertexBufferDescription[1] {{
                .slot = 0,
                .pitch = sizeof(PositionColorVertex),
                .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                .instance_step_rate = 0,
            }},
            .num_vertex_buffers = 1,
            .vertex_attributes = new SDL_GPUVertexAttribute[2] {{
                .location = 0,
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
                .offset = 0
            }, {
                .location = 1,
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_UBYTE4_NORM,
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

    pipeline = renderer.CreateGPUGraphicsPipeline(pipelineCreateInfo);
    if (pipeline == nullptr)
    {
        SDL_Log("Failed to create fill pipeline!");
    }

    // Clean up shader resources
    renderer.ReleaseShader(vertexShader);
    renderer.ReleaseShader(fragmentShader);

    // Create the vertex buffer
    SDL_GPUBufferCreateInfo vertexBufferCreateInfo = {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = sizeof(PositionColorVertex) * 3
    };
    vertexBuffer = renderer.CreateGPUBuffer(vertexBufferCreateInfo);

    // To get data into the vertex buffer, we have to use a transfer buffer
    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = sizeof(PositionColorVertex) * 3
    };
    SDL_GPUTransferBuffer* transferBuffer = renderer.CreateGPUTransferBuffer(transferBufferCreateInfo);

    // Map the transfer buffer and fill it with data (data is bound to the transfer buffer)
    auto* transferData = static_cast<PositionColorVertex *>(
        renderer.MapTransferBuffer(transferBuffer, false)
    );
    transferData[0] = PositionColorVertex {  -0.5,  -0.5, 0, 255,   0,   0, 255 };
    transferData[1] = PositionColorVertex {   0.5,  -0.5, 0,   0, 255,   0, 255 };
    transferData[2] = PositionColorVertex {     0,   0.5, 0,   0,   0, 255, 255 };
    renderer.UnmapTransferBuffer(transferBuffer);

    // Upload the transfer data to the vertex buffer
    SDL_GPUTransferBufferLocation transferBufferLocation = {
        .transfer_buffer = transferBuffer,
        .offset = 0
    };
    SDL_GPUBufferRegion vertexBufferRegion = {
        .buffer = vertexBuffer,
        .offset = 0,
        .size = sizeof(PositionColorVertex) * 3
    };

    renderer.BeginUploadToGPUBuffer();
    renderer.UploadToGPUBuffer(transferBufferLocation, vertexBufferRegion, false);
    renderer.EndUploadToGPUBuffer(transferBuffer);
}

bool Scene03TriangleVertexBuffer::Update(float dt) {
    const bool isRunning = ManageInput(inputState);
    return isRunning;
}

void Scene03TriangleVertexBuffer::Draw(Renderer& renderer) {
    renderer.Begin();

    renderer.BindGraphicsPipeline(pipeline);
    SDL_GPUBufferBinding vertexBindings = { .buffer = vertexBuffer, .offset = 0 };
    renderer.BindVertexBuffers(0, vertexBindings, 1);
    renderer.DrawGPUPrimitive(3, 1, 0, 0);

    renderer.End();
}

void Scene03TriangleVertexBuffer::Unload() {

}