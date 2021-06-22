/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperCore/Events/IEvent.hpp>

namespace HyperCore
{
	class IKeyEvent : public IEvent
	{
	public:
		int32_t key_code() const;

		virtual IEvent::ECategory category() const override;

	protected:
		explicit IKeyEvent(int32_t key_code);

	protected:
		int32_t m_key_code{ 0 };
	};

	class CKeyPressedEvent : public IKeyEvent
	{
	private:
		int32_t m_repeat_count;

	public:
		explicit CKeyPressedEvent(int32_t key_code, int32_t repeat_count);

		int32_t repeat_count() const;

		virtual std::string name() const override;
		virtual IEvent::EType type() const override;
	};

	class CKeyReleasedEvent : public IKeyEvent
	{
	public:
		explicit CKeyReleasedEvent(int32_t key_code);

		virtual std::string name() const override;
		virtual IEvent::EType type() const override;
	};

	class CKeyTypedEvent : public IKeyEvent
	{
	public:
		explicit CKeyTypedEvent(int32_t key_code);

		virtual std::string name() const override;
		virtual IEvent::EType type() const override;
	};
} // namespace HyperCore
