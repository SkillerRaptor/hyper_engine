/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "globals.hlsli"
#include "shader_interop.h"

HE_PUSH_CONSTANT(BrdfPushConstants, g_push);

// Describes the self-shadowing property of the microfacets
float geometry_schlick_ggx(const float n_dot_v, const float roughness) {
    const float a = roughness;
    const float k = (a * a) / 2.0;

    const float num = n_dot_v;
    const float denom = n_dot_v * (1.0 - k) + k;

    return num / denom;
}

float geometry_smith(const float3 n, const float3 v, const float3 l, const float roughness) {
    const float n_dot_l = max(dot(n, l), 0.0);
    const float ggx1  = geometry_schlick_ggx(n_dot_l, roughness);

    const float n_dot_v = max(dot(n, v), 0.0);
    const float ggx2  = geometry_schlick_ggx(n_dot_v, roughness);

    return ggx1 * ggx2;
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
    return float2(float(i) / float(n), radical_inverse_vdc(i));
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

    const float3 sample_vector = tangent * h.x + bitangent * h.y + normal * h.z;
    return normalize(sample_vector);
}

float2 integrate_brdf(const float n_dot_v, const float roughness) {
    const float3 v = float3(
        sqrt(1.0 - n_dot_v * n_dot_v),
        0.0,
         n_dot_v
    );

    const float3 normal = float3(0.0, 0.0, 1.0);

    float a = 0.0;
    float b = 0.0;

    const uint sample_count = 1024;
    for(uint i = 0; i < sample_count; ++i)
    {
        const float2 xi = hammersley(i, sample_count);
        const float3 h = importance_sample_ggx(xi, normal, roughness);
        const float3 l = normalize(2.0 * dot(v, h) * h - v);

        const float n_dot_l = saturate(l.z);
        const float n_dot_h = saturate(h.z);
        const float v_dot_h = saturate(dot(v, h));

        if(n_dot_l > 0.0)
        {
            const float g = geometry_smith(normal, v, l, roughness);
            const float g_vis = (g * v_dot_h) / (n_dot_h * n_dot_v);
            const float f_c = pow(1.0 - v_dot_h, 5.0);

            a += (1.0 - f_c) * g_vis;
            b += f_c * g_vis;
        }
    }

    a /= float(sample_count);
    b /= float(sample_count);

    return float2(a, b);
}

[numthreads(16, 16, 1)]
void main(uint3 id : SV_DispatchThreadID) {
    if (id.x >= g_push.size || id.y >= g_push.size) {
        return;
    }

    const float2 uv = float2(id.xy + 1) / float2(g_push.size, g_push.size);

    const float n_dot_v = uv.x;
    const float roughness = 1.0 - uv.y;

    const float2 integrated_brdf = integrate_brdf(n_dot_v, roughness);
    g_push.brdf_texture.store_2d(id.xy, integrated_brdf);
}
