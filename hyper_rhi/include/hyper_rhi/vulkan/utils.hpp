/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <fmt/format.h>
#include <vulkan/vk_enum_string_helper.h>

#include <hyper_core/assertion.hpp>
#include <hyper_core/macro_utils.hpp>
#include <hyper_core/string_utils.hpp>

#define HE_VK_CHECK(expression, ...)                                                                 \
    do {                                                                                             \
        static constexpr usize _argument_count = HE_MACRO_SIZE(__VA_ARGS__);                         \
                                                                                                     \
        const VkResult _result = (expression);                                                       \
        if ((_result) != VK_SUCCESS) [[unlikely]] {                                                  \
            if constexpr (_argument_count == 0) {                                                    \
                HE_PANIC(                                                                            \
                    "{}(...) failed: {}",                                                            \
                    ::he::string_utils::strip_namespace(                                             \
                        ::he::string_utils::extract_function_name(HE_STRINGIFY(expression))),        \
                    _result);                                                                        \
            } else {                                                                                 \
                HE_PANIC(__VA_OPT__("{}: ") "{}", __VA_OPT__(::fmt::format(__VA_ARGS__), ) _result); \
            }                                                                                        \
        }                                                                                            \
    } while (false)

template <>
struct fmt::formatter<VkResult> : fmt::formatter<std::string_view> {
    auto format(const VkResult &result, fmt::format_context &context) const
    {
        return fmt::format_to(context.out(), "{}", string_VkResult(result));
    }
};
