/*
 * Copyright (c) 2024 SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "./assets/shaders/globals.hlsli"

struct Bindings {
  ArrayBuffer vertices;
};

struct Vertex {
  float4 position;
  float4 color;
};

struct VertexOutput {
  float4 position : SV_POSITION;
  float4 color : COLOR;
};

static const float3 positions[3] = {
  float3( 0.0,  0.5, 0.0),
  float3( 0.5, -0.5, 0.0),
  float3(-0.5, -0.5, 0.0),
};

static const float3 colors[3] = {
  float3(1.0, 0.0, 0.0),
  float3(0.0, 1.0, 0.0),
  float3(0.0, 0.0, 1.0),
};

VertexOutput vs_main(
  uint vertex_id : SV_VertexID
) {
  // Bindings bindings = load_bindings<Bindings>();
  // Vertex vertex = bindings.vertices.load<Vertex>(vertex_id);

  VertexOutput output = (VertexOutput) 0;
  output.position = float4(positions[vertex_id], 1.0);
  output.color = float4(colors[vertex_id], 1.0);
  return output;
}

float4 fs_main(VertexOutput input) : SV_TARGET {
  return input.color;
}
