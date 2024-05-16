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

VertexOutput main(
  uint vertex_id : SV_VertexID
) {
  Bindings bindings = load_bindings<Bindings>();
  Vertex vertex = bindings.vertices.load<Vertex>(vertex_id);

  VertexOutput output = (VertexOutput) 0;
  output.position = float4(vertex.position.xyz, 1.0);
  output.color = float4(vertex.color.xyz, 1.0);
  return output;
}
