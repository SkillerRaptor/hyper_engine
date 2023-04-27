/*
 * Copyright (c) 2022-2023 SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#version 450

layout (location = 0) out vec3 v_fragment_color;

vec2 g_positions[3] = vec2[](
    vec2( 0.0, -0.5),
    vec2( 0.5,  0.5),
    vec2(-0.5,  0.5)
);

vec3 g_colors[3] = vec3[](
    vec3(1.0, 0.0, 0.0),
    vec3(0.0, 1.0, 0.0),
    vec3(0.0, 0.0, 1.0)
);

void main() {
    gl_Position = vec4(g_positions[gl_VertexIndex], 0.0, 1.0);
    v_fragment_color = g_colors[gl_VertexIndex];
}
