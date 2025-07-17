/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "color_space.hlsli"
#include "globals.hlsli"
#include "shader_interop.h"

HE_PUSH_CONSTANT(ObjectPushConstants, g_push);

struct VertexOutput {
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3x3 tbn : TBN;
    float3 color : COLOR;
    float2 uv : TEXCOORD;
    float3 world_pos : TEXCOORD1;
};

VertexOutput vs_main(
  uint vertex_id : SV_VertexID
) {
    const ShaderCamera camera = get_camera();

    const ShaderMaterial material = g_push.get_material();

    const ShaderModel model = g_push.get_model();
    const float4 position = float4(model.get_position(vertex_id), 1.0);
    const float3 normal = model.get_normal(vertex_id);
    const float4 tangent = model.get_tangent(vertex_id);
    const float3 color = model.get_color(vertex_id);
    const float2 uv = model.get_uv(vertex_id);

    const float3 n = normalize(float3(mul(g_push.transform_matrix, float4(normal, 0.0)).xyz));
    float3 t = normalize(float3(mul(g_push.transform_matrix, float4(tangent.xyz, 0.0)).xyz));

    t = normalize(t - dot(t, n) * n);

    const float3 b = cross(n, t) * tangent.w;
    const float3x3 tbn = float3x3(t, b, n);

    const float4 world_position = mul(g_push.transform_matrix, position);

    VertexOutput output = (VertexOutput) 0;
    output.position = mul(camera.view_projection, world_position);
    output.normal = n;
    output.tbn = transpose(tbn);
    output.color = color;
    output.uv = uv;
    output.world_pos = world_position.xyz;
    return output;
}

// Approximates the amount the surface's microfacets are aligned to the halfway vector, influenced by the roughness of the surface
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

// Describes the self-shadowing property of the microfacets
float geometry_schlick_ggx(float n_dot_v, float roughness) {
    const float r = (roughness + 1.0);
    const float k = (r * r) / 8.0;

    const float num = n_dot_v;
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

// Describes the ratio of surface reflection at different surface angles
float3 fresnel_schlick(float cos_theta, float3 f_0) {
    return f_0 + (1.0 - f_0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

float3 fresnel_schlick_roughness(float cos_theta, float3 f_0, float roughness) {
    return f_0 + (max(float3(1.0 - roughness, 1.0 - roughness, 1.0 - roughness), f_0) - f_0) * pow(clamp(1.0 - cos_theta, 0.0, 1.0), 5.0);
}

float4 fs_main(VertexOutput input) : SV_TARGET {
    const ShaderScene scene = g_push.get_scene();
    const ShaderMaterial material = g_push.get_material();

    float3 albedo = input.color;
    if (material.albedo_texture.handle.is_valid()) {
        const SamplerState albedo_sampler = material.albedo_sampler.load();
        const float3 albedo_value = material.albedo_texture.sample_2d<float3>(albedo_sampler, input.uv);
        const float3 final_albedo_value = material.albedo_factors.xyz * albedo_value;
        albedo = final_albedo_value;
    }

    float roughness = 1.0;
    float metallic = 0.0;
    if (material.metal_roughness_texture.handle.is_valid()) {
        const SamplerState metal_roughness_sampler = material.metal_roughness_sampler.load();
        const float3 metal_roughness_value = material.metal_roughness_texture.sample_2d<float3>(metal_roughness_sampler, input.uv);

        const float final_roughness_value = material.metal_roughness_factors.x * metal_roughness_value.g;
        roughness = final_roughness_value;

        const float final_metallic_value = material.metal_roughness_factors.y * metal_roughness_value.b;
        metallic = final_metallic_value;
    }

    float3 normal = normalize(input.normal);
    if (material.normal_texture.handle.is_valid()) {
        const SamplerState normal_sampler = material.normal_sampler.load();
        const float3 normal_value = material.normal_texture.sample_2d<float3>(normal_sampler, input.uv);

        const float3 tangent_space_normal = normalize(normal_value * 2.0 - 1.0);
        const float3 scaled_tangent_space_normal = material.normal_scale * tangent_space_normal;
        const float3 tbn_normal = mul(input.tbn, tangent_space_normal);
        normal = normalize(tbn_normal);
    }

    const ShaderCamera camera = get_camera();
    const float3 view_direction = normalize(camera.position.xyz - input.world_pos);
    const float3 r = reflect(-view_direction, normal);

    float3 f_0 = float3(0.04, 0.04, 0.04);
    f_0 = lerp(f_0, albedo, metallic);

    float3 l_0 = float3(0.0, 0.0, 0.0);
    // FIXME: Loop through all lights
    {
        /**/
        const float3 light_position = float3(0.0, 5.0, 0.0);
        const float3 light_color = float3(100.0, 100.0, 100.0);
        /**/

        const float3 light_direction = normalize(light_position - input.world_pos);
        const float3 halfway = normalize(view_direction + light_direction);

        const float distance = length(light_position - input.world_pos);
        const float attenuation = 1.0 / (distance * distance);
        const float3 radiance = light_color * attenuation;

        // Cook-Torrance BRDF
        const float ndf = distribution_ggx(normal, halfway, roughness);
        const float g = geometry_smith(normal, view_direction, light_direction, roughness);
        const float3 f = fresnel_schlick(max(dot(halfway, view_direction), 0.0), f_0);

        const float3 k_s = f;
        float3 k_d = float3(1.0, 1.0, 1.0) - k_s;
        k_d *= 1.0 - metallic;

        const float3 numerator = ndf * g * f;
        const float denominator = 4.0 * max(dot(normal, view_direction), 0.0) * max(dot(normal, light_direction), 0.0) + 0.0001;
        const float3 specular = numerator / denominator;

        // Add to outgoing radiance L_0
        const float n_dot_l = max(dot(normal, light_direction), 0.0);
        l_0 += (k_d * albedo / PI + specular) * radiance * n_dot_l;
    }

    const float3 f = fresnel_schlick_roughness(max(dot(normal, view_direction), 0.0), f_0, roughness);

    const float3 k_s = f;
    float3 k_d = 1.0 - k_s;
    k_d *= 1.0 - metallic;

    const float3 irradiance = scene.irradiance_texture.sample_cube<float4>(scene.irradiance_sampler.load(), normal).xyz;
    const float3 diffuse = irradiance * albedo;

    const float max_reflection_lod = 4.0;
    const float3 prefiltered_color = scene.prefilter_texture.sample_level_cube<float4>(scene.prefilter_sampler.load(), r, roughness * max_reflection_lod).xyz;
    const float2 brdf = scene.brdf_texture.sample_2d<float2>(scene.brdf_sampler.load(), float2(max(dot(normal, view_direction), 0.0), roughness));
    const float3 specular = prefiltered_color * (f * brdf.x + brdf.y);

    // FIXME: Add ambient occlusion
    const float3 ambient = (k_d * diffuse + specular); // * ao;

    float3 color = ambient + l_0;
    color = apply_reinhard_tone_mapping(color);
    color = apply_srgb(color);

    return float4(color, 1.0);
}
