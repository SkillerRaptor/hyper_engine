/*
 * Copyright (c) 2024 SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

struct FragmentInput {
  float4 position : SV_POSITION;
  float4 color : COLOR;
};

float4 main(FragmentInput input) : SV_TARGET {
  return input.color;
}
