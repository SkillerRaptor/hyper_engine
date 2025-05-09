/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "globals.hlsli"
#include "shader_interop.h"

static float3 s_skybox[36] = {
    float3(-1.0,  1.0, -1.0),
    float3(-1.0, -1.0, -1.0),
    float3( 1.0, -1.0, -1.0),
    float3( 1.0, -1.0, -1.0),
    float3( 1.0,  1.0, -1.0),
    float3(-1.0,  1.0, -1.0),
    
    float3(-1.0, -1.0,  1.0),
    float3(-1.0, -1.0, -1.0),
    float3(-1.0,  1.0, -1.0),
    float3(-1.0,  1.0, -1.0),
    float3(-1.0,  1.0,  1.0),
    float3(-1.0, -1.0,  1.0),
    
    float3( 1.0, -1.0, -1.0),
    float3( 1.0, -1.0,  1.0),
    float3( 1.0,  1.0,  1.0),
    float3( 1.0,  1.0,  1.0),
    float3( 1.0,  1.0, -1.0),
    float3( 1.0, -1.0, -1.0),
    
    float3(-1.0, -1.0,  1.0),
    float3(-1.0,  1.0,  1.0),
    float3( 1.0,  1.0,  1.0),
    float3( 1.0,  1.0,  1.0),
    float3( 1.0, -1.0,  1.0),
    float3(-1.0, -1.0,  1.0),

    float3(-1.0,  1.0, -1.0),
    float3( 1.0,  1.0, -1.0),
    float3( 1.0,  1.0,  1.0),
    float3( 1.0,  1.0,  1.0),
    float3(-1.0,  1.0,  1.0),
    float3(-1.0,  1.0, -1.0),
    
    float3(-1.0, -1.0, -1.0),
    float3(-1.0, -1.0,  1.0),
    float3( 1.0, -1.0, -1.0),
    float3( 1.0, -1.0, -1.0),
    float3(-1.0, -1.0,  1.0),
    float3( 1.0, -1.0,  1.0)
};

HE_PUSH_CONSTANT(SkyboxPushConstants, g_push);

struct VertexOutput {
    float4 position : SV_POSITION;
    float3 uvw : TEXCOORD;
};

VertexOutput vs_main(
  uint vertex_id : SV_VertexID
) {
    const ShaderCamera camera = get_camera();

    const float4 position = float4(s_skybox[vertex_id], 1.0);
    
    VertexOutput output = (VertexOutput) 0;
    output.position = mul(camera.projection, mul(camera.smaller_view, position)).xyww;
    output.uvw = position.xyz;
    return output;
}

float4 fs_main(VertexOutput input) : SV_TARGET {
    const float4 color = g_push.skybox_texture.sample_cube<float4>(g_push.skybox_sampler.load(), input.uvw);
    return float4(color.xyz, 1.0);
}
