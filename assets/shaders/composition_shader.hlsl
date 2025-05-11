/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "color_space.hlsli"
#include "globals.hlsli"
#include "shader_interop.h"

HE_PUSH_CONSTANT(CompositionPushConstants, g_push);

struct VertexOutput {
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

VertexOutput vs_main(
  uint vertex_id : SV_VertexID
) {
    const float2 uv = float2((vertex_id << 1) & 2, vertex_id & 2);
    const float4 position = float4(uv * 2.0 - 1.0, 0.0, 1.0);

    VertexOutput output = (VertexOutput) 0;
    output.position = position;
    output.uv = float2(uv.x, 1.0 - uv.y);
    return output;
}

float4 fs_main(VertexOutput input) : SV_TARGET {
    const SamplerState hdr_sampler = g_push.hdr_sampler.load();
    const float4 color = g_push.hdr_texture.sample_2d<float4>(hdr_sampler, input.uv);
    return color;
}
