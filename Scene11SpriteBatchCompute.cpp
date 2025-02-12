//
// Created by Gaëtan Blaise-Cazalet on 12/02/2025.
//

#include "Scene11SpriteBatchCompute.hpp"
#include "Renderer.hpp"
#include <SDL3/SDL.h>

#include "PositionTextureVertex.hpp"

void Scene11SpriteBatchCompute::Load(Renderer& renderer) {
    basePath = SDL_GetBasePath();
    vertexShader = renderer.LoadShader(basePath, "TexturedQuadColorWithMatrix.vert", 0, 1, 0, 0);
    fragmentShader = renderer.LoadShader(basePath, "TexturedQuadColor.frag", 1, 0, 0, 0);
    viewProj = Mat4::CreateOrthographicOffCenter(0, 640, 480, 0, 0, -1);
    std::srand(0);

    // Create the pipelines
    // -- Graphics pipeline
    SDL_GPUGraphicsPipelineCreateInfo graphicsPipelineCreateInfo = {
        .vertex_shader = vertexShader,
        .fragment_shader = fragmentShader,
        .vertex_input_state = {
            .vertex_buffer_descriptions = new SDL_GPUVertexBufferDescription[1] {{
                .slot = 0,
                .pitch = sizeof(PositionTextureColorVertex),
                .input_rate = SDL_GPU_VERTEXINPUTRATE_VERTEX,
                .instance_step_rate = 0,
            }},
            .num_vertex_buffers = 1,
            .vertex_attributes = new SDL_GPUVertexAttribute[3]{{
                .location = 0,
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
                .offset = 0
            }, {
                .location = 1,
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
                .offset = 16
            }, {
                .location = 2,
                .buffer_slot = 0,
                .format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
                .offset = 32
            }},
            .num_vertex_attributes = 3,
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

    SDL_ReleaseGPUShader(renderer.device, vertexShader);
    SDL_ReleaseGPUShader(renderer.device, fragmentShader);

    // -- Compute pipeline
    SDL_GPUComputePipelineCreateInfo computePipelineCreateInfo = {
            .num_readonly_storage_buffers = 1,
            .num_readwrite_storage_textures = 0,
            .num_readwrite_storage_buffers = 1,
            .threadcount_x = 64,
            .threadcount_y = 1,
            .threadcount_z = 1,
    };
    computePipeline = renderer.CreateComputePipelineFromShader(basePath, "SpriteBatch.comp", &computePipelineCreateInfo);

    // Texture resources
    // -- Load bitmap
    SDL_Surface* imageData = renderer.LoadBMPImage(basePath, "ravioli.bmp", 4);
    if (imageData == nullptr) {
        SDL_Log("Could not load image data!");
    }

    // -- Texture sampler
    sampler = renderer.CreateSampler(SDL_GPUSamplerCreateInfo {
            .min_filter = SDL_GPU_FILTER_NEAREST,
            .mag_filter = SDL_GPU_FILTER_NEAREST,
            .mipmap_mode = SDL_GPU_SAMPLERMIPMAPMODE_NEAREST,
            .address_mode_u = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
            .address_mode_v = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
            .address_mode_w = SDL_GPU_SAMPLERADDRESSMODE_CLAMP_TO_EDGE,
    });

    // -- Create texture
    SDL_GPUTextureCreateInfo textureInfo {
            .type = SDL_GPU_TEXTURETYPE_2D,
            .format = SDL_GPU_TEXTUREFORMAT_R8G8B8A8_UNORM,
            .usage = SDL_GPU_TEXTUREUSAGE_SAMPLER,
            .width = static_cast<Uint32>(imageData->w),
            .height = static_cast<Uint32>(imageData->h),
            .layer_count_or_depth = 1,
            .num_levels = 1,
    };
    texture = renderer.CreateTexture(textureInfo);
    renderer.SetTextureName(texture,"Ravioli Texture");

    // -- Setup texture transfer buffer
    Uint32 bufferSize = imageData->w * imageData->h * 4;
    SDL_GPUTransferBufferCreateInfo textureTransferBufferCreateInfo {
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size = bufferSize
    };
    SDL_GPUTransferBuffer* textureTransferBuffer = renderer.CreateTransferBuffer(textureTransferBufferCreateInfo);
    auto textureTransferData = static_cast<PositionTextureVertex *>(
            renderer.MapTransferBuffer(textureTransferBuffer, false)
    );
    std::memcpy(textureTransferData, imageData->pixels, bufferSize);
    renderer.UnmapTransferBuffer(textureTransferBuffer);

    // Buffer resources
    // -- Sprite compute buffer
    SDL_GPUBufferCreateInfo computeBufferCreateInfo = {
            .usage = SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_READ,
            .size = SPRITE_COUNT * sizeof(ComputeSpriteInstance)
    };
    spriteComputeBuffer = renderer.CreateBuffer(computeBufferCreateInfo);

    // -- Vertex buffer
    SDL_GPUBufferCreateInfo vertexBufferCreateInfo = {
            .usage = SDL_GPU_BUFFERUSAGE_COMPUTE_STORAGE_WRITE | SDL_GPU_BUFFERUSAGE_VERTEX,
            .size = SPRITE_COUNT * sizeof(PositionTextureColorVertex)
    };
    vertexBuffer = renderer.CreateBuffer(vertexBufferCreateInfo);

    // -- Index buffer
    SDL_GPUBufferCreateInfo indexBufferCreateInfo = {
            .usage = SDL_GPU_BUFFERUSAGE_INDEX,
            .size = SPRITE_COUNT * 6 * sizeof(Uint32)
    };
    indexBuffer = renderer.CreateBuffer(indexBufferCreateInfo);

    // Upload to GPU
    // -- Index buffer
    SDL_GPUTransferBufferCreateInfo indexBufferTransferBufferInfo {
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size = SPRITE_COUNT * 6 * sizeof(Uint32)
    };
    SDL_GPUTransferBuffer* indexBufferTransferBuffer = renderer.CreateTransferBuffer(indexBufferTransferBufferInfo);

    // -- Map the transfer buffer and fill it with index data
    auto transferData = static_cast<Uint32 *>(
            renderer.MapTransferBuffer(indexBufferTransferBuffer, false)
    );
    for (Uint32 i = 0, j = 0; i < SPRITE_COUNT * 6; i += 6, j += 4) {
        transferData[i]     =  j;
        transferData[i + 1] =  j + 1;
        transferData[i + 2] =  j + 2;
        transferData[i + 3] =  j + 3;
        transferData[i + 4] =  j + 2;
        transferData[i + 5] =  j + 1;
    }
    renderer.UnmapTransferBuffer(indexBufferTransferBuffer);

    // -- Start upload
    renderer.BeginUploadToBuffer();

    // -- Transfer texture
    SDL_GPUTextureTransferInfo textureBufferLocation {
            .transfer_buffer = textureTransferBuffer,
            .offset = 0
    };
    SDL_GPUTextureRegion textureBufferRegion {
            .texture = texture,
            .w = static_cast<Uint32>(imageData->w),
            .h = static_cast<Uint32>(imageData->h),
            .d = 1
    };
    renderer.UploadToTexture(textureBufferLocation, textureBufferRegion, false);

    // -- Transfer indices
    SDL_GPUTransferBufferLocation transferIndexBufferLocation {
            .transfer_buffer = indexBufferTransferBuffer,
            .offset = 0
    };
    SDL_GPUBufferRegion indexBufferRegion {
            .buffer = indexBuffer,
            .offset = 0,
            .size = SPRITE_COUNT * 6 * sizeof(Uint32)
    };
    renderer.UploadToBuffer(transferIndexBufferLocation, indexBufferRegion, false);

    renderer.EndUploadToBuffer(indexBufferTransferBuffer);
    renderer.ReleaseTransferBuffer(textureTransferBuffer);
    renderer.ReleaseSurface(imageData);

    // Create compute transfer buffer
    SDL_GPUTransferBufferCreateInfo computeTransferInfo = {
            .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
            .size = SPRITE_COUNT * sizeof(ComputeSpriteInstance)
    };
    spriteComputeTransferBuffer = renderer.CreateTransferBuffer(computeTransferInfo);
}

bool Scene11SpriteBatchCompute::Update(float dt) {
    const bool isRunning = ManageInput(inputState);

    return isRunning;
}

void Scene11SpriteBatchCompute::Draw(Renderer& renderer) {

    // Uploading position data
    // -- Map the transfer buffer and fill it with instance data
    auto dataPtr = static_cast<ComputeSpriteInstance *>(
            renderer.MapTransferBuffer(spriteComputeTransferBuffer, true)
    );
    for (Uint32 i = 0; i < SPRITE_COUNT; i += 1)
    {
        dataPtr[i].x = (float)(std::rand() % 640);
        dataPtr[i].y = (float)(std::rand() % 480);
        dataPtr[i].z = 0;
        dataPtr[i].w = 1;
        dataPtr[i].rotation = ((float)std::rand())/(RAND_MAX/(SDL_PI_F * 2));
        dataPtr[i].w = 32;
        dataPtr[i].h = 32;
        dataPtr[i].r = 1.0f;
        dataPtr[i].g = 1.0f;
        dataPtr[i].b = 1.0f;
        dataPtr[i].a = 1.0f;
    }
    renderer.UnmapTransferBuffer(spriteComputeTransferBuffer);
    // -- Upload instance data
    renderer.BeginUploadToBuffer();
    SDL_GPUTransferBufferLocation transferComputeBufferLocation {
        .transfer_buffer = spriteComputeTransferBuffer,
        .offset = 0
    };
    SDL_GPUBufferRegion computeBufferRegion {
        .buffer = spriteComputeBuffer,
        .offset = 0,
        .size = SPRITE_COUNT * sizeof(ComputeSpriteInstance)
    };
    renderer.UploadToBuffer(transferComputeBufferLocation, computeBufferRegion, true);
    renderer.EndUploadToBuffer(spriteComputeTransferBuffer, false);

    // Compute pass
    SDL_GPUStorageBufferReadWriteBinding bufferBinding {
        .buffer = vertexBuffer,
        .cycle = true
    };
    renderer.BeginCompute(nullptr, 0, &bufferBinding, 1);
    renderer.BindComputePipeline(computePipeline);
    renderer.BindComputeStorageBuffers(0, spriteComputeBuffer, 1);
    renderer.DispatchCompute(SPRITE_COUNT / 64, 1, 1);
    renderer.EndCompute();

    // Passes cannot be mingled, so we need to end the compute pass before starting the graphics pass

    // Graphics pass
    renderer.Begin();
    renderer.BindGraphicsPipeline(graphicsPipeline);
    SDL_GPUBufferBinding vertexBindings { .buffer = vertexBuffer, .offset = 0 };
    renderer.BindVertexBuffers(0, vertexBindings, 1);
    SDL_GPUBufferBinding indexBindings { .buffer = indexBuffer, .offset = 0 };
    renderer.BindIndexBuffer(indexBindings, SDL_GPU_INDEXELEMENTSIZE_32BIT);
    renderer.BindFragmentSamplers(0, SDL_GPUTextureSamplerBinding { .texture = texture, .sampler = sampler }, 1);
    renderer.PushVertexUniformData(0, &viewProj, sizeof(Mat4));
    renderer.DrawIndexedPrimitives(SPRITE_COUNT * 6, 1, 0, 0, 0);
    renderer.End();
}

void Scene11SpriteBatchCompute::Unload(Renderer& renderer) {
    renderer.ReleaseBuffer(vertexBuffer);
    renderer.ReleaseBuffer(indexBuffer);
    renderer.ReleaseBuffer(spriteComputeBuffer);
    renderer.ReleaseTransferBuffer(spriteComputeTransferBuffer);
    renderer.ReleaseSampler(sampler);
    renderer.ReleaseTexture(texture);
    renderer.ReleaseGraphicsPipeline(graphicsPipeline);
    renderer.ReleaseComputePipeline(computePipeline);
}