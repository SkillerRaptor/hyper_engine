/*
 * Copyright (c) 2023 SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "globals.hlsli"

struct FragmentInput {
  [[vk::location(0)]] float4 color : COLOR;
  [[vk::location(1)]] float2 uv : TEXCOORD;
};

float4 main(FragmentInput input) : SV_TARGET {
  GuiBindings gui = get_bindings<GuiBindings>();

  Texture font_texture = gui.font_texture;

  float4 value = input.color * font_texture.sample_2d<float4>(input.uv);
  return value;
}
