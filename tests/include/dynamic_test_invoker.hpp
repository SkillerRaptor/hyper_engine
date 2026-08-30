/*
 * Copyright (c) 2026-present, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <functional>

#include <catch2/catch_test_macros.hpp>

class DynamicTestInvoker : public Catch::ITestInvoker {
public:
    explicit DynamicTestInvoker(std::function<void()> test_callback)
        : m_test_callback(std::move(test_callback))
    {
    }

    void invoke() const override { m_test_callback(); }

private:
    std::function<void()> m_test_callback;
};

Catch::Detail::unique_ptr<DynamicTestInvoker> create_dynamic_test_invoker(std::function<void()> test_callback)
{
    return Catch::Detail::make_unique<DynamicTestInvoker>(std::move(test_callback));
}
