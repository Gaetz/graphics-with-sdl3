//
// Created by Gaëtan Blaise-Cazalet on 21/11/2024.
//

#include "Scene05TriangleStencil.hpp"
#include "Renderer.hpp"
#include <SDL3/SDL.h>

#include "PositionColorVertex.hpp"

void Scene05TriangleStencil::Load(Renderer& renderer) {
    basePath = SDL_GetBasePath();
    vertexShader = renderer.LoadShader(basePath, "PositionColor.vert", 0, 0, 0, 0);
    fragmentShader = renderer.LoadShader(basePath, "SolidColor.frag", 0, 0, 0, 0);


	SDL_GPUTextureFormat depthStencilFormat;

	if (SDL_GPUTextureSupportsFormat(
		renderer.device,
		SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT,
		SDL_GPU_TEXTURETYPE_2D,
		SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET
	))
	{
		depthStencilFormat = SDL_GPU_TEXTUREFORMAT_D24_UNORM_S8_UINT;
	}
	else if (SDL_GPUTextureSupportsFormat(
		renderer.device,
		SDL_GPU_TEXTUREFORMAT_D32_FLOAT_S8_UINT,
		SDL_GPU_TEXTURETYPE_2D,
		SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET
	))
	{
		depthStencilFormat = SDL_GPU_TEXTUREFORMAT_D32_FLOAT_S8_UINT;
	}
	else
	{
		SDL_Log("Stencil formats not supported!");
	}

	SDL_GPUGraphicsPipelineCreateInfo pipelineCreateInfo = {
        .vertex_shader = vertexShader,
        .fragment_shader = fragmentShader,
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
        .rasterizer_state = SDL_GPURasterizerState {
            .fill_mode = SDL_GPU_FILLMODE_FILL,
            .cull_mode = SDL_GPU_CULLMODE_NONE,
            .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE
        },
        .depth_stencil_state = SDL_GPUDepthStencilState {
            .back_stencil_state = SDL_GPUStencilOpState {
                .fail_op = SDL_GPU_STENCILOP_REPLACE,
                .pass_op = SDL_GPU_STENCILOP_KEEP,
                .depth_fail_op = SDL_GPU_STENCILOP_KEEP,
                .compare_op = SDL_GPU_COMPAREOP_NEVER
            },
            .front_stencil_state = SDL_GPUStencilOpState {
                .fail_op = SDL_GPU_STENCILOP_REPLACE,
                .pass_op = SDL_GPU_STENCILOP_KEEP,
                .depth_fail_op = SDL_GPU_STENCILOP_KEEP,
                .compare_op = SDL_GPU_COMPAREOP_NEVER
            },
            .write_mask = 0xFF,
            .enable_stencil_test = true,
        },
        .target_info = {
            .color_target_descriptions = new SDL_GPUColorTargetDescription[1] {{
                .format = SDL_GetGPUSwapchainTextureFormat(renderer.device, renderer.renderWindow)
            }},
            .num_color_targets = 1,
            .depth_stencil_format = depthStencilFormat,
            .has_depth_stencil_target = true,
        },

    };

	maskerPipeline = SDL_CreateGPUGraphicsPipeline(renderer.device, &pipelineCreateInfo);
	if (maskerPipeline == NULL)
	{
		SDL_Log("Failed to create masker pipeline!");
	}

	pipelineCreateInfo.depth_stencil_state = SDL_GPUDepthStencilState {
		.back_stencil_state = SDL_GPUStencilOpState {
			.fail_op = SDL_GPU_STENCILOP_REPLACE,
			.pass_op = SDL_GPU_STENCILOP_KEEP,
			.depth_fail_op = SDL_GPU_STENCILOP_KEEP,
			.compare_op = SDL_GPU_COMPAREOP_NEVER
		},
		.front_stencil_state = SDL_GPUStencilOpState {
			.fail_op = SDL_GPU_STENCILOP_REPLACE,
			.pass_op = SDL_GPU_STENCILOP_KEEP,
			.depth_fail_op = SDL_GPU_STENCILOP_KEEP,
			.compare_op = SDL_GPU_COMPAREOP_NEVER
		},
		.write_mask = 0xFF,
		.enable_stencil_test = true,

        };

	maskeePipeline = SDL_CreateGPUGraphicsPipeline(renderer.device, &pipelineCreateInfo);
	if (maskeePipeline == NULL)
	{
		SDL_Log("Failed to create maskee pipeline!");
	}

	SDL_ReleaseGPUShader(renderer.device, vertexShader);
	SDL_ReleaseGPUShader(renderer.device, fragmentShader);

	vertexBuffer = SDL_CreateGPUBuffer(
		renderer.device,
		new SDL_GPUBufferCreateInfo {
			.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
			.size = sizeof(PositionColorVertex) * 6
		}
	);

	int w, h;
	SDL_GetWindowSizeInPixels(renderer.renderWindow, &w, &h);

	depthStencilTexture = SDL_CreateGPUTexture(
		renderer.device,
		new SDL_GPUTextureCreateInfo {
		.type = SDL_GPU_TEXTURETYPE_2D,
		.format = depthStencilFormat,
		.usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
		.width = static_cast<Uint32>(w),
		.height = static_cast<Uint32>(h),
		.layer_count_or_depth = 1,
		.num_levels = 1,
		.sample_count = SDL_GPU_SAMPLECOUNT_1
	}
	);

	SDL_GPUTransferBuffer* transferBuffer = SDL_CreateGPUTransferBuffer(
		renderer.device,
		new SDL_GPUTransferBufferCreateInfo {
			.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
			.size = sizeof(PositionColorVertex) * 6
		}
	);

	PositionColorVertex* transferData = (PositionColorVertex*)SDL_MapGPUTransferBuffer(
		renderer.device,
		transferBuffer,
		false
	);

	transferData[0] = PositionColorVertex { -0.5f, -0.5f, 0, 255, 255,   0, 255 };
	transferData[1] = PositionColorVertex {  0.5f, -0.5f, 0, 255, 255,   0, 255 };
	transferData[2] = PositionColorVertex {     0,  0.5f, 0, 255, 255,   0, 255 };
	transferData[3] = PositionColorVertex {    -1,    -1, 0, 255,   0,   0, 255 };
	transferData[4] = PositionColorVertex {     1,    -1, 0,   0, 255,   0, 255 };
	transferData[5] = PositionColorVertex {     0,     1, 0,   0,   0, 255, 255 };

	SDL_UnmapGPUTransferBuffer(renderer.device, transferBuffer);

	SDL_GPUCommandBuffer* uploadCmdBuf = SDL_AcquireGPUCommandBuffer(renderer.device);
	SDL_GPUCopyPass* copyPass = SDL_BeginGPUCopyPass(uploadCmdBuf);

	SDL_UploadToGPUBuffer(
		copyPass,
		new SDL_GPUTransferBufferLocation {
			.transfer_buffer = transferBuffer,
			.offset = 0
		},
		new SDL_GPUBufferRegion {
			.buffer = vertexBuffer,
			.offset = 0,
			.size = sizeof(PositionColorVertex) * 6
		},
		false
	);

	SDL_EndGPUCopyPass(copyPass);
	SDL_SubmitGPUCommandBuffer(uploadCmdBuf);
	SDL_ReleaseGPUTransferBuffer(renderer.device, transferBuffer);


/*

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
        .rasterizer_state = SDL_GPURasterizerState {
            .fill_mode = SDL_GPU_FILLMODE_FILL,
            .cull_mode = SDL_GPU_CULLMODE_NONE,
            .front_face = SDL_GPU_FRONTFACE_COUNTER_CLOCKWISE
        },
        .depth_stencil_state = SDL_GPUDepthStencilState {
            .back_stencil_state = SDL_GPUStencilOpState {
                .fail_op = SDL_GPU_STENCILOP_REPLACE,
                .pass_op = SDL_GPU_STENCILOP_KEEP,
                .depth_fail_op = SDL_GPU_STENCILOP_KEEP,
                .compare_op = SDL_GPU_COMPAREOP_NEVER
            },
            .front_stencil_state = SDL_GPUStencilOpState {
                .fail_op = SDL_GPU_STENCILOP_REPLACE,
                .pass_op = SDL_GPU_STENCILOP_KEEP,
                .depth_fail_op = SDL_GPU_STENCILOP_KEEP,
                .compare_op = SDL_GPU_COMPAREOP_NEVER
            },
            .write_mask = 0xFF,
            .enable_stencil_test = true,
        },
        .target_info = {
            .color_target_descriptions = new SDL_GPUColorTargetDescription[1] {{
                .format = SDL_GetGPUSwapchainTextureFormat(renderer.device, renderer.renderWindow)
            }},
            .num_color_targets = 1,
            .depth_stencil_format = depthStencilFormat,
            .has_depth_stencil_target = true,
        },

    };

    maskerPipeline = SDL_CreateGPUGraphicsPipeline(renderer.device, &pipelineCreateInfo);
    if (maskerPipeline == nullptr)
    {
        SDL_Log("Failed to create masker pipeline!");
    }

    pipelineCreateInfo.depth_stencil_state = SDL_GPUDepthStencilState {
        .back_stencil_state = SDL_GPUStencilOpState {
            .fail_op = SDL_GPU_STENCILOP_KEEP,
            .pass_op = SDL_GPU_STENCILOP_KEEP,
            .depth_fail_op = SDL_GPU_STENCILOP_KEEP,
            .compare_op = SDL_GPU_COMPAREOP_NEVER,
        },
        .front_stencil_state = SDL_GPUStencilOpState {
            .fail_op = SDL_GPU_STENCILOP_KEEP,
            .pass_op = SDL_GPU_STENCILOP_KEEP,
            .depth_fail_op = SDL_GPU_STENCILOP_KEEP,
            .compare_op = SDL_GPU_COMPAREOP_EQUAL,
        },
        .compare_mask = 0xFF,
        .write_mask = 0,
        .enable_stencil_test = true,
    };

    maskeePipeline = SDL_CreateGPUGraphicsPipeline(renderer.device, &pipelineCreateInfo);
    if (maskeePipeline == nullptr)
    {
        SDL_Log("Failed to create maskee pipeline!");
    }

    // Clean up shader resources
    SDL_ReleaseGPUShader(renderer.device, vertexShader);
    SDL_ReleaseGPUShader(renderer.device, fragmentShader);

	SDL_GPUBufferCreateInfo vertexBufferCreateInfo = {
		.usage = SDL_GPU_BUFFERUSAGE_VERTEX,
		.size = sizeof(PositionColorVertex) * 6
	};
	vertexBuffer = renderer.CreateGPUBuffer(vertexBufferCreateInfo);

	int w, h;
	SDL_GetWindowSizeInPixels(renderer.renderWindow, &w, &h);
	SDL_GPUTextureCreateInfo depthStencilTextureCreateInfo = {
		.type = SDL_GPU_TEXTURETYPE_2D,
	    .format = depthStencilFormat,
	    .usage = SDL_GPU_TEXTUREUSAGE_DEPTH_STENCIL_TARGET,
		.width = static_cast<Uint32>(w),
		.height = static_cast<Uint32>(h),
		.layer_count_or_depth = 1,
		.num_levels = 1,
		.sample_count = SDL_GPU_SAMPLECOUNT_1
	};
	depthStencilTexture = renderer.CreateTexture(depthStencilTextureCreateInfo);

	SDL_GPUTransferBufferCreateInfo transferBufferCreateInfo = {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = sizeof(PositionColorVertex) * 6,
	};
	SDL_GPUTransferBuffer* transferBuffer = renderer.CreateGPUTransferBuffer(transferBufferCreateInfo);

	auto* transferData = static_cast<PositionColorVertex *>(
		renderer.MapTransferBuffer(transferBuffer, false)
	);
	transferData[0] = PositionColorVertex { -0.5f, -0.5f, 0, 255, 255,   0, 255 };
	transferData[1] = PositionColorVertex {  0.5f, -0.5f, 0, 255, 255,   0, 255 };
	transferData[2] = PositionColorVertex {     0,  0.5f, 0, 255, 255,   0, 255 };
	transferData[3] = PositionColorVertex {    -1,    -1, 0, 255,   0,   0, 255 };
	transferData[4] = PositionColorVertex {     1,    -1, 0,   0, 255,   0, 255 };
	transferData[5] = PositionColorVertex {     0,     1, 0,   0,   0, 255, 255 };
    renderer.UnmapTransferBuffer(transferBuffer);

    SDL_GPUTransferBufferLocation transferBufferLocation = {
        .transfer_buffer = transferBuffer,
        .offset = 0
    };
    SDL_GPUBufferRegion vertexBufferRegion = {
        .buffer = vertexBuffer,
        .offset = 0,
        .size = sizeof(PositionColorVertex) * 6
    };

    renderer.BeginUploadToGPUBuffer();
    renderer.UploadToGPUBuffer(transferBufferLocation, vertexBufferRegion, false);
    renderer.EndUploadToGPUBuffer(transferBuffer);
    */
}

bool Scene05TriangleStencil::Update(float dt) {
    const bool isRunning = ManageInput(inputState);
    return isRunning;
}

void Scene05TriangleStencil::Draw(Renderer& renderer) {

    SDL_GPUCommandBuffer* cmdbuf = SDL_AcquireGPUCommandBuffer(renderer.device);
    if (cmdbuf == NULL)
    {
        SDL_Log("AcquireGPUCommandBuffer failed: %s", SDL_GetError());
    }

    SDL_GPUTexture* swapchainTexture;
    if (!SDL_AcquireGPUSwapchainTexture(cmdbuf, renderer.renderWindow, &swapchainTexture, NULL, NULL)) {
        SDL_Log("AcquireGPUSwapchainTexture failed: %s", SDL_GetError());
    }

    if (swapchainTexture != NULL)
    {
        SDL_GPUColorTargetInfo colorTargetInfo = { 0 };
        colorTargetInfo.texture = swapchainTexture;
        colorTargetInfo.clear_color = SDL_FColor { 0.0f, 0.0f, 0.0f, 1.0f };
        colorTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        colorTargetInfo.store_op = SDL_GPU_STOREOP_STORE;

        SDL_GPUDepthStencilTargetInfo depthStencilTargetInfo = { 0 };
        depthStencilTargetInfo.texture = depthStencilTexture;
        depthStencilTargetInfo.cycle = true;
        depthStencilTargetInfo.clear_depth = 0;
        depthStencilTargetInfo.clear_stencil = 0;
        depthStencilTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
        depthStencilTargetInfo.store_op = SDL_GPU_STOREOP_DONT_CARE;
        depthStencilTargetInfo.stencil_load_op = SDL_GPU_LOADOP_CLEAR;
        depthStencilTargetInfo.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;

        SDL_GPURenderPass* renderPass = SDL_BeginGPURenderPass(
            cmdbuf,
            &colorTargetInfo,
            1,
            &depthStencilTargetInfo
        );

        SDL_BindGPUVertexBuffers(renderPass, 0, new SDL_GPUBufferBinding {.buffer = vertexBuffer, .offset = 0 }, 1);

        SDL_SetGPUStencilReference(renderPass, 1);
        SDL_BindGPUGraphicsPipeline(renderPass, maskerPipeline);
        SDL_DrawGPUPrimitives(renderPass, 3, 1, 0, 0);

        SDL_SetGPUStencilReference(renderPass, 0);
        SDL_BindGPUGraphicsPipeline(renderPass, maskeePipeline);
        SDL_DrawGPUPrimitives(renderPass, 3, 1, 3, 0);


        SDL_EndGPURenderPass(renderPass);
    }

    SDL_SubmitGPUCommandBuffer(cmdbuf);







    /*
    SDL_GPUDepthStencilTargetInfo depthStencilTargetInfo {};
    depthStencilTargetInfo.texture = depthStencilTexture;
    depthStencilTargetInfo.cycle = true;
    depthStencilTargetInfo.clear_depth = 0;
    depthStencilTargetInfo.clear_stencil = 0;
    depthStencilTargetInfo.load_op = SDL_GPU_LOADOP_CLEAR;
    depthStencilTargetInfo.store_op = SDL_GPU_STOREOP_DONT_CARE;
    depthStencilTargetInfo.stencil_load_op = SDL_GPU_LOADOP_CLEAR;
    depthStencilTargetInfo.stencil_store_op = SDL_GPU_STOREOP_DONT_CARE;

    renderer.Begin(&depthStencilTargetInfo);
    SDL_GPUBufferBinding vertexBindings { .buffer = vertexBuffer, .offset = 0 };
    renderer.BindVertexBuffers(0, vertexBindings, 1);

    renderer.SetGPUStencilReference(1);
    renderer.BindGraphicsPipeline(maskerPipeline);
    renderer.DrawGPUPrimitive(3, 1, 0, 0);

    renderer.SetGPUStencilReference(0);
    renderer.BindGraphicsPipeline(maskeePipeline);
    renderer.DrawGPUPrimitive(3, 1, 3, 0);

    renderer.End();
    */
}

void Scene05TriangleStencil::Unload(Renderer& renderer) {
    SDL_ReleaseGPUTexture(renderer.device, depthStencilTexture);
    renderer.ReleaseBuffer(vertexBuffer);
    renderer.ReleaseGraphicsPipeline(maskeePipeline);
    renderer.ReleaseGraphicsPipeline(maskerPipeline);
}