/*
 * Copyright (c) 2024-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

float3 apply_srgb(float3 x) {
    return pow(x, 1.0 / 2.2);
}

float3 remove_srgb(float3 x) {
    return pow(x, 2.2);
}

float3 apply_reinhard_tone_mapping(float3 x) {
    return x / (x + float3(1.0, 1.0, 1.0));
}
