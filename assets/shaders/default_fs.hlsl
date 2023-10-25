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
  Scene scene = get_scene();

  ObjectBindings object = get_bindings<ObjectBindings>();
  ObjectMaterial material = object.get_material(scene);

  return material.base_color;
}
