//
// Created by Gaëtan Blaise-Cazalet on 19/11/2024.
//

#include "Scene02Triangle.hpp"
#include "Renderer.hpp"
#include <SDL3/SDL.h>

void Scene02Triangle::Load(Renderer& renderer) {
    basePath = SDL_GetBasePath();
    vertexShader = renderer.LoadShader(basePath, "RawTriangle.vert", 0, 0, 0, 0);
    fragmentShader = renderer.LoadShader(basePath, "SolidColor.frag", 0, 0, 0, 0);

    // Create the pipelines
    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .vertex_shader = vertexShader,
        .fragment_shader = fragmentShader,
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .target_info = {
            .color_target_descriptions = new SDL_GPUColorTargetDescription[1] {{
                .format = SDL_GetGPUSwapchainTextureFormat(renderer.device, renderer.renderWindow)
            }},
            .num_color_targets = 1,
        },

    };

    pipelineCreateInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    FillPipeline = SDL_CreateGPUGraphicsPipeline(renderer.device, &pipelineCreateInfo);
    if (FillPipeline == nullptr)
    {
        SDL_Log("Failed to create fill pipeline!");
    }

    pipelineCreateInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_LINE;
    LinePipeline = SDL_CreateGPUGraphicsPipeline(renderer.device, &pipelineCreateInfo);
    if (LinePipeline == nullptr)
    {
        SDL_Log("Failed to create line pipeline!");
    }

    // Clean up shader resources
    SDL_ReleaseGPUShader(renderer.device, vertexShader);
    SDL_ReleaseGPUShader(renderer.device, fragmentShader);

    // Finally, print instructions!
    SDL_Log("Press Left to toggle wireframe mode");
    SDL_Log("Press Down to toggle small viewport");
    SDL_Log("Press Right to toggle scissor rect");

}

bool Scene02Triangle::Update(float dt) {
    const bool isRunning = ManageInput(inputState);

    if (inputState.left) {
        UseWireframeMode = !UseWireframeMode;
    }
    if (inputState.down) {
        UseSmallViewport = !UseSmallViewport;
    }
    if (inputState.right) {
        UseScissorRect = !UseScissorRect;
    }

    return isRunning;
}

void Scene02Triangle::Draw(Renderer& renderer) {
    renderer.Begin();

    renderer.BindGraphicsPipeline(UseWireframeMode ? LinePipeline : FillPipeline);
    if (UseSmallViewport) {
        renderer.SetGPUViewport(SmallViewport);
    }
    if (UseScissorRect) {
        renderer.SetGPUScissorRect(ScissorRect);
    }
    renderer.DrawGPUPrimitive(3, 1, 0, 0);

    renderer.End();
}

void Scene02Triangle::Unload() {

}