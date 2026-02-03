/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef HE_SHADER_INTEROP_HPP
#define HE_SHADER_INTEROP_HPP

#ifdef __cplusplus
#    include <glm/glm.hpp>
#
#    include <rhi/resource_handle.hpp>

#    define float2 ::glm::vec2
#    define float3 ::glm::vec3
#    define float4 ::glm::vec4
#    define float3x3 ::glm::mat3
#    define float4x4 ::glm::mat4

#    define int2 ::glm::i32vec2
#    define int3 ::glm::i32vec3
#    define int4 ::glm::i32vec4

#    define uint uint32_t
#    define uint2 ::glm::u32vec2
#    define uint3 ::glm::u32vec3
#    define uint4 ::glm::u32vec4

#    define RESOURCE_HANDLE ::he::ResourceHandle
#    define SIMPLE_BUFFER RESOURCE_HANDLE
#    define RW_SIMPLE_BUFFER RESOURCE_HANDLE
#    define ARRAY_BUFFER RESOURCE_HANDLE
#    define RW_ARRAY_BUFFER RESOURCE_HANDLE
#    define TEXTURE RESOURCE_HANDLE
#    define RW_TEXTURE RESOURCE_HANDLE
#    define SAMPLER RESOURCE_HANDLE
#else
#    include "globals.hlsli"

#    define RESOURCE_HANDLE uint
#    define SIMPLE_BUFFER SimpleBuffer
#    define RW_SIMPLE_BUFFER RwSimpleBuffer
#    define ARRAY_BUFFER ArrayBuffer
#    define RW_ARRAY_BUFFER RwArrayBuffer
#    define TEXTURE Texture
#    define RW_TEXTURE RwTexture
#    define SAMPLER Sampler
#endif

////////////////////////////////////////////////////////////////////////////////
// Shader Interop
////////////////////////////////////////////////////////////////////////////////

struct ShaderCamera
{
    float4 position;

    float4x4 view;
    float4x4 inverse_view;
    float4x4 projection;
    float4x4 inverse_projection;
    float4x4 view_projection;
    float4x4 inverse_view_projection;

    float4x4 smaller_view;

    float near_plane;
    float far_plane;
    float padding_0;
    float padding_1;
};

////////////////////////////////////////////////////////////////////////////////
// Push Constants
////////////////////////////////////////////////////////////////////////////////

#ifndef __cplusplus
#    ifdef HE_VULKAN
#        define HE_PUSH_CONSTANT(value_type, name) [[vk::push_constant]] value_type name
#    else
#        define HE_PUSH_CONSTANT(value_type, name) ConstantBuffer<value_type> name : register(b0, space0)
#    endif
#endif

struct DebugPushConstants
{
    SIMPLE_BUFFER camera;
    uint padding_1;
    uint padding_2;
    uint padding_3;
};

struct CompositionPushConstants
{
    TEXTURE composition_texture;
    SAMPLER composition_sampler;
    uint padding_0;
    uint padding_1;
};

#ifdef __cplusplus
#    undef float2
#    undef float3
#    undef float4
#    undef float4x4

#    undef int2
#    undef int3
#    undef int4

#    undef uint
#    undef uint2
#    undef uint3
#    undef uint4

#    undef RESOURCE_HANDLE
#    undef ARRAY_BUFFER
#else
#    undef RESOURCE_HANDLE
#    undef ARRAY_BUFFER
#endif

#endif // HE_SHADER_INTEROP_HPP
