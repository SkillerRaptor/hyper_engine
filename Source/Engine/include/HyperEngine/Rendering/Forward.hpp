/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#define HYPERENGINE_VULKAN_HANDLE(x) using x = struct x##_T *

HYPERENGINE_VULKAN_HANDLE(VkDebugUtilsMessengerEXT);
HYPERENGINE_VULKAN_HANDLE(VkDevice);
HYPERENGINE_VULKAN_HANDLE(VkInstance);
HYPERENGINE_VULKAN_HANDLE(VkPhysicalDevice);
HYPERENGINE_VULKAN_HANDLE(VkSurfaceKHR);
HYPERENGINE_VULKAN_HANDLE(VkQueue);

#undef HYPERENGINE_VULKAN_HANDLE
