/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "globals.hlsli"
#include "shader_interop.h"

HE_PUSH_CONSTANT(EquirectangularPushConstants, g_push);

struct Face {
    float3 forward;
    float3 up;
    float3 right;
};

static Face s_faces[6] = {
    { float3(1.0, 0.0, 0.0), float3(0.0, 1.0, 0.0), float3(0.0, 0.0, -1.0)},
    { float3(-1.0, 0.0, 0.0), float3(0.0, 1.0, 0.0), float3(0.0, 0.0, 1.0)},
    { float3(0.0, -1.0, 0.0), float3(0.0, 0.0, 1.0), float3(1.0, 0.0, 0.0)},
    { float3(0.0, 1.0, 0.0), float3(0.0, 0.0, -1.0), float3(1.0, 0.0, 0.0)},
    { float3(0.0, 0.0, 1.0), float3(0.0, 1.0, 0.0), float3(1.0, 0.0, 0.0)},
    { float3(0.0, 0.0, -1.0), float3(0.0, 1.0, 0.0), float3(-1.0, 0.0, 0.0)}
};

[numthreads(16, 16, 1)]
void main(uint3 id : SV_DispatchThreadID) {
    if (id.x >= g_push.skybox_size || id.y >= g_push.skybox_size) {
        return;
    }

    const float2 dimensions = float2(g_push.skybox_size, g_push.skybox_size);
    const float2 uv = float2(id.xy) / dimensions * 2.0 - 1.0;

    const Face face = s_faces[id.z];
    const float3 spherical = normalize(face.forward + face.right * uv.x + face.up * uv.y);

    const float2 inv_atan = float2(0.1591, 0.3183);
    const float2 equirectangular_uv = float2(atan2(spherical.z, spherical.x), asin(spherical.y)) * inv_atan + 0.5;
    const float2 equirectangular_pixel = float2(equirectangular_uv * float2(g_push.equirectangular_width, g_push.equirectangular_height));

    const float4 color = g_push.equirectangular_texture.load_2d<float4>(equirectangular_pixel);
    g_push.skybox_texture.store_3d(id.xyz, color);
}
