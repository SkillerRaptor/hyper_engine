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
    float3 color : COLOR;
    float2 uv : TEXCOORD;
};

VertexOutput vs_main(
  uint vertex_id : SV_VertexID
) {
    const ShaderCamera camera = get_camera();

    const ShaderMaterial material = g_push.get_material();

    const ShaderModel model = g_push.get_model();
    const float4 position = float4(model.get_position(vertex_id), 1.0);
    const float3 normal = model.get_normal(vertex_id);
    const float3 color = model.get_color(vertex_id);
    const float2 uv = model.get_uv(vertex_id);

    VertexOutput output = (VertexOutput) 0;
    output.position = mul(camera.view_projection, mul(g_push.transform_matrix, position));
    output.normal = normal;
    output.color = color * material.color_factors.xyz;
    output.uv = uv;
    return output;
}

float4 fs_main(VertexOutput input) : SV_TARGET {
    const ShaderScene scene = g_push.get_scene();

    const ShaderMaterial material = g_push.get_material();

    const float light_value = max(dot(input.normal, scene.sunlight_direction.xyz), 0.1);

    const float4 color = float4(input.color, 1.0) * material.color_texture.sample_2d<float4>(material.color_sampler.load(), input.uv);
    const float3 ambient = color.xyz * scene.ambient_color.xyz;

    if (color.a < 0.1) {
        discard;
    }

    return float4(apply_srgb(color.xyz * light_value * scene.sunlight_color.w + ambient), 1.0);
}
