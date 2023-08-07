/*
 * Copyright (c) 2023 SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "bindings.hlsl"

struct Bindings {
  SimpleBuffer projection_view;
  ArrayBuffer vertices;
  ArrayBuffer transforms;
};

struct Vertex {
  float4 position;
  float4 normal;
  float4 color;
};

struct VertexOutput {
  float4 position : SV_POSITION;
  [[vk::location(0)]] float4 color : COLOR;
};

VertexOutput main(
  uint vertex_id : SV_VertexID,
  uint instance_id : SV_InstanceID
) {
  Bindings bindings = load_bindings<Bindings>();

  float4x4 projection_view = bindings.projection_view.load<float4x4>();
  Vertex vertex = bindings.vertices.load<Vertex>(vertex_id);
  float4x4 transform = bindings.transforms.load<float4x4>(instance_id);

  VertexOutput output = (VertexOutput)0;
  output.position = mul(mul(projection_view, transform), float4(vertex.position.xyz, 1.0));
  output.color = float4(vertex.color.xyz, 1.0);
  return output;
}
