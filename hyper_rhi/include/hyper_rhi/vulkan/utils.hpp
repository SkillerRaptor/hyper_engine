/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <fmt/format.h>
#include <vulkan/vk_enum_string_helper.h>

#include <hyper_core/assertion.hpp>
#include <hyper_core/string_utils.hpp>

#define HE_VK_CHECK(expression, ...)                                                      \
    do {                                                                                  \
        const VkResult _result = (expression);                                            \
        if ((_result) != VK_SUCCESS) [[unlikely]] {                                       \
            HE_PANIC(                                                                     \
                "{}(...) failed: {}",                                                     \
                ::he::string_utils::strip_namespace(                                      \
                    ::he::string_utils::extract_function_name(HE_STRINGIFY(expression))), \
                _result);                                                                 \
        }                                                                                 \
    } while (false)

template <>
struct fmt::formatter<VkResult> : fmt::formatter<std::string_view> {
    auto format(const VkResult &result, fmt::format_context &context) const
    {
        return fmt::format_to(context.out(), "{}", string_VkResult(result));
    }
};
