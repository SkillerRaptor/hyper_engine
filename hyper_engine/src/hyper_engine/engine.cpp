/*
 * Copyright (c) 2024, SkillerRaptor
 *
 * SPDX-License-Identifier: MIT
 */

#include "hyper_engine/engine.hpp"
#include "hyper_platform/window.hpp"

namespace hyper_engine
{
    Engine::Engine(const EngineDescriptor &descriptor)
        : m_window({
              .title = "HyperEngine",
              .width = descriptor.width,
              .height = descriptor.height,
          })
    {
    }

    void Engine::run()
    {
    }
} // namespace hyper_engine
