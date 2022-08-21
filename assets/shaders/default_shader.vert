/*
 * Copyright (c) 2022, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#version 450

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_color;
layout(location = 2) in vec3 i_normal;

layout(location = 0) out vec3 o_color;

layout(push_constant) uniform constants {
  vec4 data;
  mat4 render_matrix;
}
v_push_constant;

void main() {
  gl_Position = v_push_constant.render_matrix * vec4(i_position, 1.0);
  o_color = i_color;
}
