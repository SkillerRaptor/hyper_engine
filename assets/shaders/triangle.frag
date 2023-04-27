/*
 * Copyright (c) 2022-2023 SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#version 450

layout (location = 0) out vec4 o_color;

layout (location = 0) in vec3 v_fragment_color;

void main() {
    o_color = vec4(v_fragment_color, 1.0);
}
