/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "globals.hlsli"
#include "shader_interop.h"

HE_PUSH_CONSTANT(PrefilterPushConstants, g_push);

float distribution_ggx(float3 n, float3 h, float roughness) {
    const float a = roughness * roughness;
    const float a_2 = a * a;

    const float n_dot_h = max(dot(n, h), 0.0);
    const float n_dot_h_2 = n_dot_h * n_dot_h;

    const float number = a_2;
    float denominator = (n_dot_h_2 * (a_2 - 1.0) + 1.0);
    denominator = PI * denominator * denominator;

    return number / denominator;
}

float radical_inverse_vdc(uint bits) {
    bits = (bits << 16u) | (bits >> 16u);
    bits = ((bits & 0x55555555u) << 1u) | ((bits & 0xaaaaaaaau) >> 1u);
    bits = ((bits & 0x33333333u) << 2u) | ((bits & 0xccccccccu) >> 2u);
    bits = ((bits & 0x0f0f0f0fu) << 4u) | ((bits & 0xf0f0f0f0u) >> 4u);
    bits = ((bits & 0x00ff00ffu) << 8u) | ((bits & 0xff00ff00u) >> 8u);
    return float(bits) * 2.3283064365386963e-10;
}

float2 hammersley(const uint i, const uint n) {
    const float radical_inverse = radical_inverse_vdc(i);
    return float2(float(i) / float(n), radical_inverse);
}

float3 importance_sample_ggx(const float2 xi, const float3 normal, const float roughness) {
    const float a = roughness * roughness;

    const float phi = 2.0 * PI * xi.x;
    const float cos_theta = sqrt((1.0 - xi.y) / (1.0 + (a * a - 1.0) * xi.y));
    const float sin_theta = sqrt(1.0 - cos_theta * cos_theta);

    const float3 h = float3(
        cos(phi) * sin_theta,
        sin(phi) * sin_theta,
        cos_theta
    );

    const float3 up = abs(normal.z) < 0.999 ? float3(0.0, 0.0, 1.0) : float3(1.0, 0.0, 0.0);
    const float3 tangent = normalize(cross(up, normal));
    const float3 bitangent = cross(normal, tangent);

    const float3 sample_vector = normalize(
        tangent * h.x +
        bitangent * h.y +
        normal * h.z
    );

    return sample_vector;
}

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
    const float3 r = normal;
    const float3 v = r;

    if (g_push.roughness == 0.0) {
        const float3 color = g_push.skybox_texture.sample_level_cube<float4>(g_push.skybox_sampler.load(), r, 0.0).xyz;
        const uint3 coord = uint3(id.x, g_push.size - 1 - id.y, id.z);
        g_push.prefilter_texture.store_3d(coord, color);
        return;
    }

    const uint sample_count = 64;

    float total_weight = 0.0;
    float3 prefiltered_color = float3(0.0, 0.0, 0.0);
    for (uint i = 0; i < sample_count; i++) {
        const float2 xi = hammersley(i, sample_count);
        const float3 h = importance_sample_ggx(xi, normal, g_push.roughness);
        const float3 l = normalize(2.0 * dot(v, h) * h - v);

        const float n_dot_l = saturate(dot(normal, l));
        if (n_dot_l > 0.0) {
            const float n_dot_h = saturate(dot(normal, h));
            const float h_dot_v = saturate(dot(h, v));
            const float d = distribution_ggx(normal, h, g_push.roughness);
            const float pdf = d * n_dot_h / (4.0 * h_dot_v) + 0.0001;

            const float resolution = 512.0;
            const float sa_texel = 4.0 * PI / (6.0 * resolution * resolution);
            const float sa_sample = 1.0 / (float(sample_count) * pdf + 0.0001);

            const float mip_level = max(0.5 * log2(sa_sample / sa_texel), 0.0);

            const float3 color = g_push.skybox_texture.sample_level_cube<float4>(g_push.skybox_sampler.load(), l, mip_level).xyz;
            prefiltered_color += color * n_dot_l;
            total_weight += n_dot_l;
        }
    }

    prefiltered_color /= total_weight;

    const uint3 coord = uint3(id.x, g_push.size - 1 - id.y, id.z);
    g_push.prefilter_texture.store_3d(coord, prefiltered_color);
}
