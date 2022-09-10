/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "bindings.hlsl"

struct PixelInput {
  float4 position : SV_POSITION;
  [[vk::location(0)]] float4 color : COLOR;
};

float4 main(PixelInput input) : SV_TARGET { return input.color; }
