/*
 * Copyright (c) 2022-present, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#version 450

layout(location = 0) in vec3 i_color;

layout(location = 0) out vec4 o_fragment_color;

void main() { o_fragment_color = vec4(i_color, 1.0); }
