/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "globals.hlsli"
#include "shader_interop.h"

HE_PUSH_CONSTANT(BrdfPushConstants, g_push);

// Describes the self-shadowing property of the microfacets
float geometry_schlick_ggx(float n_dot_v, float roughness) {
    const float a = roughness;
    const float k = (a * a) / 2.0;

    const float num   = n_dot_v;
    const float denom = n_dot_v * (1.0 - k) + k;

    return num / denom;
}

float geometry_smith(float3 n, float3 v, float3 l, float roughness) {
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

float2 hammersley(uint i, uint n) {
    return float2(float(i) / float(n), radical_inverse_vdc(i));
}

float3 importance_sample_ggx(float2 xi, float3 normal, float roughness) {
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

    return normalize(sample_vector);
}

float2 integrate_brdf(float n_dot_v, float roughness) {
    float3 v = float3(
        sqrt(1.0 - n_dot_v * n_dot_v),
        0.0,
         n_dot_v
    );

    float a = 0.0;
    float b = 0.0;
    float3 normal = float3(0.0, 0.0, 1.0);

    const uint sample_count = 1024;
    for(uint i = 0; i < sample_count; ++i)
    {
        const float2 xi = hammersley(i, sample_count);
        const float3 h = importance_sample_ggx(xi, normal, roughness);
        const float3 l = normalize(2.0 * dot(v, h) * h - v);

        const float n_dot_l = max(l.z, 0.0);
        const float n_dot_h = max(h.z, 0.0);
        const float v_dot_h = max(dot(v, h), 0.0);

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
    float2 brdf_value = integrate_brdf(float(id.x) / float(g_push.size), 1.0 - (float(id.y) / float(g_push.size)));
    g_push.brdf_texture.store_2d(id.xy, brdf_value);
}
