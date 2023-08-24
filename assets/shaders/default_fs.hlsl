/*
 * Copyright (c) 2023 SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "globals.hlsli"

struct FragmentInput {
  [[vk::location(0)]] float4 color : COLOR;
};

float4 main(FragmentInput input) : SV_TARGET {
  ObjectBindings object = get_bindings<ObjectBindings>();
  ObjectMaterial material = object.get_material();

  return material.base_color;
}
