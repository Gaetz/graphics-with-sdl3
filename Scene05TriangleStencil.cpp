//
// Created by Gaëtan Blaise-Cazalet on 21/11/2024.
//

#include "Scene05TriangleStencil.hpp"
#include "Renderer.hpp"
#include <SDL3/SDL.h>

#include "PositionColorVertex.hpp"

void Scene05TriangleStencil::Load(Renderer& renderer) {
    basePath = SDL_GetBasePath();
    vertexShader = renderer.LoadShader(basePath, "RawTriangle.vert", 0, 0, 0, 0);
    fragmentShader = renderer.LoadShader(basePath, "SolidColor.frag", 0, 0, 0, 0);

    // Create stencil format
    SDL_GPUTextureFormat depthStencilFormat;

    if (renderer.DoesTextureSupportFormat(
        SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT,
        SDL_GPU_TEXTURETYPE_2D,
        SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET
    )) {
        depthStencilFormat = SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT;
    }
    else if (renderer.DoesTextureSupportFormat(
        SDL_GPU_TEXTUREFORMAT_D32_FLOAT_S8_UINT,
        SDL_GPU_TEXTURETYPE_2D,
        SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET
    )) {
        depthStencilFormat = SDL_GPU_TEXTUREFORMAT_D32_FLOAT_S8_UINT;
    }
    else {
        SDL_Log("Stencil formats not supported!");
    }

    // Create the pipeline
    SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .vertex_shader = vertexShader,
        .fragment_shader = fragmentShader,
        .vertex_input_state = SDL_GPUVertexInputState{
            .num_vertex_buffers = 1,
            .vertex_buffer_descriptions = new SDL_GPUVertexBufferDescription[1] {{
                .slot = 0,
                .pitch = sizeof(PositionColorVertex),
                .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                .instance_step_rate = 0,
            }},
            .num_vertex_attributes = 2,
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
            }}
        },
        .primitive_type = SDL_GPU_PRIMITIVETYPE_TRIANGLELIST,
        .rasterizer_state = SDL_GPURasterizerState {
            .cull_mode = SDL_GPU_CULLMODE_NONE,
            .fill_mode = SDL_GPU_FILLMODE_FILL,
            .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE
        },
        .depth_stencil_state = SDL_GPUDepthStencilState {
            .back_stencil_state = SDL_GPUStencilOpState {
                .compare_op = SDL_GPU_COMPAREOP_NEVER,
                .fail_op = SDL_GPU_STENCILOP_REPLACE,
                .pass_op = SDL_GPU_STENCILOP_KEEP,
                .depth_fail_op = SDL_GPU_STENCILOP_KEEP,
            },
            .front_stencil_state = SDL_GPUStencilOpState {
                .compare_op = SDL_GPU_COMPAREOP_NEVER,
                .fail_op = SDL_GPU_STENCILOP_REPLACE,
                .pass_op = SDL_GPU_STENCILOP_KEEP,
                .depth_fail_op = SDL_GPU_STENCILOP_KEEP,
            },
            .write_mask = 0xFF,
            .enable_stencil_test = true,
        },
        .target_info = {
            .color_target_descriptions = new SDL_GPUColorTargetDescription[1] {{
                .format = SDL_GetGPUSwapchainTextureFormat(renderer.device, renderer.renderWindow)
            }},
            .num_color_targets = 1,
            .has_depth_stencil_target = true,
            .depth_stencil_format = depthStencilFormat
        },

    };

    pipelineCreateInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_FILL;
    maskeePipeline = SDL_CreateGPUGraphicsPipeline(renderer.device, &pipelineCreateInfo);
    if (maskeePipeline == nullptr)
    {
        SDL_Log("Failed to create fill pipeline!");
    }

    pipelineCreateInfo.rasterizer_state.fill_mode = SDL_GPU_FILLMODE_LINE;
    maskerPipeline = SDL_CreateGPUGraphicsPipeline(renderer.device, &pipelineCreateInfo);
    if (maskerPipeline == nullptr)
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

bool Scene05TriangleStencil::Update(float dt) {
    const bool isRunning = ManageInput(inputState);

    if (inputState.left) {
        useWireframeMode = !useWireframeMode;
    }
    if (inputState.down) {
        useSmallViewport = !useSmallViewport;
    }
    if (inputState.right) {
        useScissorRect = !useScissorRect;
    }

    return isRunning;
}

void Scene05TriangleStencil::Draw(Renderer& renderer) {
    renderer.Begin();

    renderer.BindGraphicsPipeline(useWireframeMode ? maskerPipeline : maskeePipeline);
    if (useSmallViewport) {
        renderer.SetGPUViewport(smallViewport);
    }
    if (useScissorRect) {
        renderer.SetGPUScissorRect(scissorRect);
    }
    renderer.DrawGPUPrimitive(3, 1, 0, 0);

    renderer.End();
}

void Scene05TriangleStencil::Unload(Renderer& renderer) {
    renderer.ReleaseGraphicsPipeline(maskeePipeline);
    renderer.ReleaseGraphicsPipeline(maskerPipeline);

}