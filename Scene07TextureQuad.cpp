//
// Created by Gaëtan Blaise-Cazalet on 29/11/2024.
//

#include "Scene07TextureQuad.hpp"
#include "Renderer.hpp"
#include "PositionColorVertex.hpp"
#include "PositionTextureVertex.hpp"
#include <SDL3/SDL.h>

void Scene07TextureQuad::Load(Renderer& renderer) {
    basePath = SDL_GetBasePath();
    vertexShader = renderer.LoadShader(basePath, "PositionColorInstanced.vert", 0, 0, 0, 0);
    fragmentShader = renderer.LoadShader(basePath, "SolidColor.frag", 0, 0, 0, 0);

    SDL_Surface* imageData = renderer.LoadBMPImage(basePath, "ravioli.bmp", 4);
    if (imageData == nullptr) {
        SDL_Log("Could not load image data!");
    }

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

    // Clean up shader resources
    renderer.ReleaseShader(vertexShader);
    renderer.ReleaseShader(fragmentShader);

	// PointClamp
	samplers[0] = renderer.CreateSampler(SDL_GPUSamplerCreateInfo {
		.min_filter = SDL_GPU_FILTER_NEAREST,
		.mag_filter = SDL_GPU_FILTER_NEAREST,
		.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
		.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
	});
	// PointWrap
	samplers[1] = renderer.CreateSampler(SDL_GPUSamplerCreateInfo {
		.min_filter = SDL_GPU_FILTER_NEAREST,
		.mag_filter = SDL_GPU_FILTER_NEAREST,
		.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
		.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
	});
	// LinearClamp
	samplers[2] = renderer.CreateSampler(SDL_GPUSamplerCreateInfo {
		.min_filter = SDL_GPU_FILTER_LINEAR,
		.mag_filter = SDL_GPU_FILTER_LINEAR,
		.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
		.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
	});
	// LinearWrap
	samplers[3] = renderer.CreateSampler(SDL_GPUSamplerCreateInfo {
		.min_filter = SDL_GPU_FILTER_LINEAR,
		.mag_filter = SDL_GPU_FILTER_LINEAR,
		.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
		.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
	});
	// AnisotropicClamp
	samplers[4] = renderer.CreateSampler(SDL_GPUSamplerCreateInfo{
		.min_filter = SDL_GPU_FILTER_LINEAR,
		.mag_filter = SDL_GPU_FILTER_LINEAR,
		.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
		.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
		.enable_anisotropy = true,
		.max_anisotropy = 4
	});
	// AnisotropicWrap
	samplers[5] = renderer.CreateSampler(SDL_GPUSamplerCreateInfo {
		.min_filter = SDL_GPU_FILTER_LINEAR,
		.mag_filter = SDL_GPU_FILTER_LINEAR,
		.mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_LINEAR,
		.address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		.address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		.address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_REPEAT,
		.enable_anisotropy = true,
		.max_anisotropy = 4
	});

    // Create the vertex buffer
    SDL_GPUBufferCreateInfo vertexBufferCreateInfo = {
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX,
        .size = sizeof(PositionTextureVertex) * 4
    };
    vertexBuffer = renderer.CreateBuffer(vertexBufferCreateInfo);
	renderer.SetBufferName(vertexBuffer, "Ravioli Vertex Buffer");

    // Create the index buffer
    SDL_GPUBufferCreateInfo indexBufferCreateInfo = {
        .usage = SDL_GPU_BUFFERUSAGE_INDEX,
        .size = sizeof(Uint16) * 6
    };
    indexBuffer = renderer.CreateBuffer(indexBufferCreateInfo);

	// Create texture
	SDL_GPUTextureCreateInfo textureInfo {
		.type = SDL_GPU_TEXTURETYPE_2D,
		.format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
		.width = static_cast<Uint32>(imageData->w),
		.height = static_cast<Uint32>(imageData->h),
		.layer_count_or_depth = 1,
		.num_levels = 1,
		.usage = SDL_GPU_TEXTUREUSAGE_SAMPLER
	};
	texture = renderer.CreateTexture(textureInfo);
	renderer.SetTextureName(texture,"Ravioli Texture");

    // Set the buffer data
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
	transferData[1] = PositionTextureVertex {  1,  1, 0, 4, 0 };
	transferData[2] = PositionTextureVertex {  1, -1, 0, 4, 4 };
	transferData[3] = PositionTextureVertex { -1, -1, 0, 0, 4 };
    auto indexData = reinterpret_cast<Uint16*>(&transferData[4]);
	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;
	indexData[3] = 0;
	indexData[4] = 2;
	indexData[5] = 3;
    renderer.UnmapTransferBuffer(transferBuffer);

	// Setup texture transfer buffer
	SDL_GPUTransferBufferCreateInfo textureTransferBufferCreateInfo = {
		.usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
		.size = imageData->w * imageData->h * 4
	};
	SDL_GPUTransferBuffer* textureTransferBuffer = renderer.CreateTransferBuffer(textureTransferBufferCreateInfo);
	Uint8* textureTransferPtr = static_cast<Uint8*>(

    renderer.BeginUploadToBuffer();
	// Upload the transfer data to the vertex and index buffer
    SDL_GPUTransferBufferLocation transferVertexBufferLocation {
        .transfer_buffer = transferBuffer,
        .offset = 0
    };
    SDL_GPUBufferRegion vertexBufferRegion {
        .buffer = vertexBuffer,
        .offset = 0,
        .size = sizeof(PositionColorVertex) * 9
    };
    SDL_GPUTransferBufferLocation transferIndexBufferLocation {
        .transfer_buffer = transferBuffer,
        .offset = sizeof(PositionColorVertex) * 9
    };
    SDL_GPUBufferRegion indexBufferRegion {
        .buffer = indexBuffer,
        .offset = 0,
        .size = sizeof(Uint16) * 6
    };

    renderer.UploadToBuffer(transferVertexBufferLocation, vertexBufferRegion, false);
    renderer.UploadToBuffer(transferIndexBufferLocation, indexBufferRegion, false);
    renderer.EndUploadToBuffer(transferBuffer);
}

bool Scene07TextureQuad::Update(float dt) {
    const bool isRunning = ManageInput(inputState);

    if (inputState.left)
    {
        useVertexOffset = !useVertexOffset;
        SDL_Log("Using vertex offset: %s", useVertexOffset ? "true" : "false");
    }

    if (inputState.right)
    {
        useIndexOffset = !useIndexOffset;
        SDL_Log("Using index offset: %s", useIndexOffset ? "true" : "false");
    }

    if (inputState.up)
    {
        useIndexBuffer = !useIndexBuffer;
        SDL_Log("Using index buffer: %s", useIndexBuffer ? "true" : "false");
    }

    return isRunning;
}

void Scene07TextureQuad::Draw(Renderer& renderer) {
    Uint32 vertexOffset = useVertexOffset ? 3 : 0;
    Uint32 indexOffset = useIndexOffset ? 3 : 0;

    renderer.Begin();

    renderer.BindGraphicsPipeline(pipeline);

    SDL_GPUBufferBinding vertexBindings = { .buffer = vertexBuffer, .offset = 0 };
    renderer.BindVertexBuffers(0, vertexBindings, 1);
    if (useIndexBuffer) {
        SDL_GPUBufferBinding indexBindings = { .buffer = indexBuffer, .offset = 0 };
        renderer.BindIndexBuffer(indexBindings, SDL_GPU_INDEXELEMENTSIZE_16BIT);
        renderer.DrawIndexedPrimitives(3, 16, indexOffset, vertexOffset, 0);
    } else {
        renderer.DrawPrimitives(3, 16, vertexOffset, 0);
    }

    renderer.End();
}

void Scene07TextureQuad::Unload(Renderer& renderer) {
    renderer.ReleaseBuffer(vertexBuffer);
    renderer.ReleaseBuffer(indexBuffer);
    renderer.ReleaseGraphicsPipeline(pipeline);
}