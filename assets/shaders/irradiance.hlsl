/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "globals.hlsli"
#include "shader_interop.h"

HE_PUSH_CONSTANT(IrradiancePushConstants, g_push);

[numthreads(16, 16, 1)]
void main(uint3 id : SV_DispatchThreadID) {
    if (id.x >= g_push.size || id.y >= g_push.size) {
        return;
    }

    const float2 dimensions = float2(g_push.size, g_push.size);
    const float2 uv = float2(id.xy) / dimensions * 2.0 - 1.0;

    float3 direction = float3(0.0, 0.0, 0.0);
    switch (id.z) {
        case 0: direction = float3(1.0, uv.y, -uv.x); break;
        case 1: direction = float3(-1.0, uv.y, uv.x); break;
        case 2: direction = float3(uv.x, 1.0, -uv.y); break;
        case 3: direction = float3(uv.x, -1.0, uv.y); break;
        case 4: direction = float3(uv.x, uv.y, 1.0); break;
        case 5: direction = float3(-uv.x, uv.y, -1.0); break;
        default: break;
    }

    const float3 normal = normalize(direction);

    const float3 world_up = float3(0.0, 1.0, 0.0);
    const float3 right = normalize(cross(world_up, normal));
    const float3 up = normalize(cross(normal, right));

    const float sample_delta = 0.05;

    uint sample_count = 0;
    float3 irradiance = float3(0.0, 0.0, 0.0);
    for (float phi = 0.0; phi < 2.0 * PI; phi += sample_delta) {
        const float sin_phi = sin(phi);
        const float cos_phi = cos(phi);
        for (float theta = 0.0; theta < 0.5 * PI; theta += sample_delta) {
            const float sin_theta = sin(theta);
            const float cos_theta = cos(theta);

            const float3 tangent_sample = float3(
                sin_theta * cos_phi,
                sin_theta * sin_phi,
                cos_theta
            );

            const float3 sample_vector = tangent_sample.x * right + tangent_sample.y * up + tangent_sample.z * normal;
            const float3 color = g_push.skybox_texture.sample_level_cube<float4>(g_push.skybox_sampler.load(), sample_vector, 0.0).xyz;
            irradiance += color * cos_theta * sin_theta;
            sample_count++;
        }
    }

    irradiance = PI * irradiance * (1.0 / float(sample_count));

    const uint3 coord = uint3(id.x, g_push.size - 1 - id.y, id.z);
    g_push.irradiance_texture.store_3d(coord, irradiance);
}
