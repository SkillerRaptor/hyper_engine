/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include "HyperEngine/Support/Expected.hpp"
#include "HyperEngine/Support/Prerequisites.hpp"

using VkInstance = struct VkInstance_T *;

namespace HyperEngine
{
	class RenderContext
	{
	public:
		HYPERENGINE_NON_COPYABLE(RenderContext);

	public:
		~RenderContext();

		RenderContext(RenderContext &&other) noexcept;
		RenderContext &operator=(RenderContext &&other) noexcept;

		static Expected<RenderContext> create();

	private:
		explicit RenderContext(Error &error);

		Expected<void> create_instance();

		constexpr uint32_t make_version(
			uint32_t major,
			uint32_t minor,
			uint32_t patch) const noexcept
		{
			uint32_t value = 0;
			value |= major << 22;
			value |= minor << 12;
			value |= patch << 0;

			return value;
		}

		constexpr uint32_t make_api_version(
			uint32_t variant,
			uint32_t major,
			uint32_t minor,
			uint32_t patch) const noexcept
		{
			uint32_t value = 0;
			value |= variant << 29;
			value |= major << 22;
			value |= minor << 12;
			value |= patch << 0;

			return value;
		}

	private:
		VkInstance m_instance = nullptr;
	};
} // namespace HyperEngine
