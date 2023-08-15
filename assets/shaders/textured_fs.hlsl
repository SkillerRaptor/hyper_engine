/*
 * Copyright (c) 2023 SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "globals.hlsli"

struct Bindings {
  ArrayBuffer vertices;
  ArrayBuffer transforms;

  Texture texture;
};

struct PixelInput {
  [[vk::location(0)]] float4 color : COLOR;
  [[vk::location(1)]] float2 tex_coord : TEXCOORD;
};

float4 main(PixelInput input) : SV_TARGET {
  Bindings bindings = load_bindings<Bindings>();

  float4 value = bindings.texture.sample_2d<float4>(input.tex_coord);
  return value;
}
