/*
 * Copyright (c) 2023 SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "globals.hlsli"

struct Bindings {
  ArrayBuffer vertices;

  Texture font_texture;
};

struct Vertex {
  float4 position;
  float4 color;
  float4 uv;
};

struct VertexOutput {
  float4 position : SV_POSITION;
  [[vk::location(0)]] float4 color : COLOR;
  [[vk::location(1)]] float2 uv : TEXCOORD;
};

VertexOutput main(
  uint vertex_id : SV_VertexID
) {
  Bindings bindings = load_bindings<Bindings>();

  Frame frame = get_frame();
  Camera camera = get_camera();

  Vertex vertex = bindings.vertices.load<Vertex>(vertex_id);

  VertexOutput output = (VertexOutput) 0;
  output.position = float4(
    2.0 * vertex.position.x / frame.screen_size.x - 1.0,
    2.0 * vertex.position.y / frame.screen_size.y - 1.0,
    0.0, 1.0
  );
  output.color = float4(srgb_to_linear(vertex.color.xyz), vertex.color.a);
  output.uv = vertex.uv.xy;
  return output;
}
