/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Events/KeyEvents.hpp>

namespace HyperCore
{
	IKeyEvent::IKeyEvent(int32_t key_code)
		: m_key_code(key_code)
	{
	}

	int32_t IKeyEvent::key_code() const
	{
		return m_key_code;
	}

	IEvent::ECategory IKeyEvent::category() const
	{
		return IEvent::ECategory::Key | IEvent::ECategory::Input;
	}

	CKeyPressedEvent::CKeyPressedEvent(int32_t key_code, int32_t repeat_count)
		: IKeyEvent(key_code)
		, m_repeat_count(repeat_count)
	{
	}

	int32_t CKeyPressedEvent::repeat_count() const
	{
		return m_repeat_count;
	}

	std::string CKeyPressedEvent::name() const
	{
		return "KeyPressed";
	}

	IEvent::EType CKeyPressedEvent::type() const
	{
		return IEvent::EType::KeyPressed;
	}

	CKeyReleasedEvent::CKeyReleasedEvent(int32_t key_code)
		: IKeyEvent(key_code)
	{
	}

	std::string CKeyReleasedEvent::name() const
	{
		return "KeyReleased";
	}

	IEvent::EType CKeyReleasedEvent::type() const
	{
		return IEvent::EType::KeyReleased;
	}

	CKeyTypedEvent::CKeyTypedEvent(int32_t key_code)
		: IKeyEvent(key_code)
	{
	}

	std::string CKeyTypedEvent::name() const
	{
		return "KeyTyped";
	}

	IEvent::EType CKeyTypedEvent::type() const
	{
		return IEvent::EType::KeyTyped;
	}
} // namespace HyperCore
