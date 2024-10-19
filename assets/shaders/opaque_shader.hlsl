/*
* Copyright (c) 2024 SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "globals.hlsli"

HE_PUSH_CONSTANT(ObjectPushConstants, g_push);

struct VertexOutput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

VertexOutput vs_main(
  uint vertex_id : SV_VertexID
) {
    const Mesh mesh = g_push.get_mesh();
    const float4 position = mesh.get_position(vertex_id);

    const Material material = g_push.get_material();
    const float4 base_color = material.base_color;

    VertexOutput output = (VertexOutput) 0;
    output.position = float4(position.xyz, 1.0);
    output.color = float4(base_color.xyz, 1.0);
    return output;
}

float4 fs_main(VertexOutput input) : SV_TARGET {
    return input.color;
}
