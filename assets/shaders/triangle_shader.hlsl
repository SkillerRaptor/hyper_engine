/*
* Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

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
    VertexOutput output = (VertexOutput) 0;
    output.position = float4(g_positions[vertex_id], 0.0, 1.0);
    output.color = g_colors[vertex_id];
    return output;
}

float4 fs_main(VertexOutput input) : SV_TARGET {
    return float4(input.color, 1.0);
}
