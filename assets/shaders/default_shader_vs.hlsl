/*
 * Copyright (c) 2022, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "bindings.hlsl"

struct Bindings {
  ArrayBuffer vertices;
  ArrayBuffer transforms;
};

struct Vertex {
  float3 position;
  float3 color;
};

struct VertexOutput {
  float4 position : SV_POSITION;
  [[vk::location(0)]] float4 color : COLOR;
};

VertexOutput main(uint vertex_id : SV_VertexID) {
  Bindings bindings = load_bindings<Bindings>();

  Vertex vertex = bindings.vertices.load<Vertex>(vertex_id);
  float4x4 transform = bindings.transforms.load<float4x4>(0);

  VertexOutput output = (VertexOutput)0;
  output.position = mul(float4(vertex.position, 1.0), transform);
  output.color = float4(vertex.color, 1.0);
  return output;
}
