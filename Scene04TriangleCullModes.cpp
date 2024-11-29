//
// Created by Gaëtan Blaise-Cazalet on 20/11/2024.
//

#include "Scene04TriangleCullModes.hpp"
#include "Renderer.hpp"
#include "PositionColorVertex.hpp"
#include <SDL3/SDL.h>

void Scene04TriangleCullModes::Load(Renderer& renderer) {
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

    for (int i = 0; i < pipelines.size(); ++i)
    {
        pipelineCreateInfo.rasterizer_state.cull_mode = (SDL_GPUCullMode) (i % 3);
        pipelineCreateInfo.rasterizer_state.front_face = (i > 2) ?
            SDL_GPU_FRONTFACE_CLOCKWISE :
            SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE;

        pipelines[i] = renderer.CreateGPUGraphicsPipeline(pipelineCreateInfo);
        if (pipelines[i] == nullptr)
        {
            SDL_Log("Failed to create pipeline!");
            return;
        }
    }

    // Clean up shader resources
    renderer.ReleaseShader(vertexShader);
    renderer.ReleaseShader(fragmentShader);

    // Create the vertex buffer
    SDL_GPUBufferCreateInfo vertexBufferCreateInfo = {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = sizeof(PositionColorVertex) * 3
    };
    vertexBufferCW = renderer.CreateBuffer(vertexBufferCreateInfo);
    vertexBufferCCW = renderer.CreateBuffer(vertexBufferCreateInfo);

    // To get data into the vertex buffer, we have to use a transfer buffer
    SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = sizeof(PositionColorVertex) * 6,
    };
    SDL_GPUTransferBuffer* transferBuffer = renderer.CreateTransferBuffer(transferBufferCreateInfo);

    // Map the transfer buffer and fill it with data (data is bound to the transfer buffer)
    auto* transferData = static_cast<PositionColorVertex *>(
        renderer.MapTransferBuffer(transferBuffer, false)
    );
    transferData[0] = PositionColorVertex {    -1,    -1, 0, 255,   0,   0, 255 };
    transferData[1] = PositionColorVertex {     1,    -1, 0,   0, 255,   0, 255 };
    transferData[2] = PositionColorVertex {     0,     1, 0,   0,   0, 255, 255 };
    transferData[3] = PositionColorVertex {     0,     1, 0, 255,   0,   0, 255 };
    transferData[4] = PositionColorVertex {     1,    -1, 0,   0, 255,   0, 255 };
    transferData[5] = PositionColorVertex {    -1,    -1, 0,   0,   0, 255, 255 };
    renderer.UnmapTransferBuffer(transferBuffer);

    // Upload the transfer data to the vertex buffer
    SDL_GPUTransferBufferLocation transferBufferLocationCW = {
        .transfer_buffer = transferBuffer,
        .offset = 0
    };
    SDL_GPUTransferBufferLocation transferBufferLocationCCW = {
            .transfer_buffer = transferBuffer,
            .offset = sizeof (PositionColorVertex) * 3
    };
    SDL_GPUBufferRegion vertexBufferRegionCW = {
        .buffer = vertexBufferCW,
        .offset = 0,
        .size = sizeof(PositionColorVertex) * 3
    };
    SDL_GPUBufferRegion vertexBufferRegionCCW = {
            .buffer = vertexBufferCCW,
            .offset = 0,
            .size = sizeof(PositionColorVertex) * 3
    };

    renderer.BeginUploadToBuffer();
    renderer.UploadToBuffer(transferBufferLocationCW, vertexBufferRegionCW, false);
    renderer.UploadToBuffer(transferBufferLocationCCW, vertexBufferRegionCCW, false);
    renderer.EndUploadToBuffer(transferBuffer);

    // Finally, print instructions!
    SDL_Log("Press Left/Right to switch between modes");
    SDL_Log("Current Mode: %s", modeNames[0].c_str());
}

bool Scene04TriangleCullModes::Update(float dt) {
    const bool isRunning = ManageInput(inputState);

    if (inputState.left)
    {
        currentMode -= 1;
        if (currentMode < 0)
        {
            currentMode = pipelines.size() - 1;
        }
        SDL_Log("Current Mode: %s", modeNames[currentMode].c_str());
    }

    if (inputState.right)
    {
        currentMode = (currentMode + 1) % pipelines.size();
        SDL_Log("Current Mode: %s", modeNames[currentMode].c_str());
    }

    return isRunning;
}

void Scene04TriangleCullModes::Draw(Renderer& renderer) {
    renderer.Begin();

    renderer.BindGraphicsPipeline(pipelines[currentMode]);

    renderer.SetViewport({ 0, 0, 320, 480 });
    SDL_GPUBufferBinding vertexBindingsCW = { .buffer = vertexBufferCW, .offset = 0 };
    renderer.BindVertexBuffers(0, vertexBindingsCW, 1);
    renderer.DrawPrimitives(3, 1, 0, 0);

    renderer.SetViewport({ 320, 0, 320, 480 });
    SDL_GPUBufferBinding vertexBindingsCCW = { .buffer = vertexBufferCCW, .offset = 0 };
    renderer.BindVertexBuffers(0, vertexBindingsCCW, 1);
    renderer.DrawPrimitives(3, 1, 0, 0);

    renderer.End();
}

void Scene04TriangleCullModes::Unload(Renderer& renderer) {
    renderer.ReleaseBuffer(vertexBufferCW);
    renderer.ReleaseBuffer(vertexBufferCCW);
    for (int i = 0; i < pipelines.size(); ++i) {
        renderer.ReleaseGraphicsPipeline(pipelines[i]);
    }
}