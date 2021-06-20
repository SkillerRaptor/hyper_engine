/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <cstdint>
#include <string>

namespace HyperCore
{
	class IEvent
	{
	public:
		using CategoryIndex = uint8_t;
		using TypeIndex = uint8_t;

		enum ECategory : CategoryIndex
		{
			None = 0,
			Input,
			Key,
			Mouse,
			Window
		};

		enum class EType : TypeIndex
		{
			None = 0,
			KeyPressed,
			KeyReleased,
			KeyTyped,
			MouseMoved,
			MouseScrolled,
			MouseButtonPressed,
			MouseButtonReleased,
			WindowClose,
			WindowResize,
			WindowFocus,
			WindowLostFocus,
			WindowMoved
		};

	public:
		IEvent() = default;
		virtual ~IEvent() = default;

		virtual std::string name() const = 0;
		virtual EType type() const = 0;
		virtual ECategory category() const = 0;
	};
} // namespace HyperCore
