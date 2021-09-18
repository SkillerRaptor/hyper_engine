/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#version 450

layout (location = 0) out vec4 v_output_color;

layout (location = 0) in vec3 v_fragment_color;

void main()
{
    v_output_color = vec4(v_fragment_color, 1.0);
}
