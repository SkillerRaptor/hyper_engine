/*
 * Copyright (c) 2025-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "color_space.hlsli"
#include "globals.hlsli"
#include "shader_interop.h"

struct VertexOutput {
    float4 position : SV_POSITION;
    float3 color : COLOR;
};

static float2 g_positions[3] = {
    float2(-0.5, -0.5),
    float2(0.5, -0.5),
    float2(0.0, 0.5),
};

static float3 g_colors[3] = {
    float3(0.0, 0.0, 1.0),
    float3(0.0, 1.0, 0.0),
    float3(1.0, 0.0, 0.0),
};

VertexOutput vs_main(
  uint vertex_id : SV_VertexID
) {
    const ShaderCamera camera = get_camera();

    VertexOutput output = (VertexOutput) 0;
    output.position = mul(camera.view_projection, float4(g_positions[vertex_id], 0.0, 1.0));
    output.color = g_colors[vertex_id];
    return output;
}

float4 fs_main(VertexOutput input) : SV_TARGET {
    return float4(apply_srgb(input.color), 1.0);
}
