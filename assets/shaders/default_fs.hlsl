/*
 * Copyright (c) 2023 SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

struct PixelInput {
  [[vk::location(0)]] float4 color : COLOR;
};

float4 main(PixelInput input) : SV_TARGET {
  return input.color;
}
