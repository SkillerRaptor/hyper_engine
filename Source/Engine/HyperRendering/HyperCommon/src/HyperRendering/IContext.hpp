/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#define HYPERENGINE_SHARED_EXPORT
#include <HyperPlatform/SharedLibrary.hpp>

namespace HyperPlatform
{
	class IWindow;
}

namespace HyperRendering
{
	class HYPERENGINE_API IContext
	{
	public:
		IContext() = default;
		virtual ~IContext() = default;

		virtual bool initialize(HyperPlatform::IWindow* window) = 0;
		virtual void shutdown() = 0;

		virtual void update() = 0;
	};

	extern "C" HYPERENGINE_API IContext* create_context();
} // namespace HyperRendering
