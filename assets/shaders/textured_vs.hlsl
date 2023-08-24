/*
 * Copyright (c) 2023 SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "globals.hlsli"

struct Vertex {
  float4 position;
  float4 normal;
  float4 color;
  float4 uv;
};

struct VertexOutput {
  float4 position : SV_POSITION;
  [[vk::location(0)]] float4 color : COLOR;
  [[vk::location(1)]] float2 tex_coord : TEXCOORD;
};

VertexOutput main(
  uint vertex_id : SV_VertexID,
  uint instance_id : SV_InstanceID
) {
  Scene scene = get_scene();
  Camera camera = scene.get_camera();

  ObjectBindings object = get_bindings<ObjectBindings>();

  Vertex vertex = object.get_geometry().get_vertex<Vertex>(vertex_id);
  float4x4 transform = object.get_instance(instance_id);

  VertexOutput output = (VertexOutput) 0;
  output.position = mul(mul(camera.view_projection, transform), float4(vertex.position.xyz, 1.0));
  output.color = float4(vertex.color.xyz, 1.0);
  output.tex_coord = vertex.uv.xy;
  return output;
}
