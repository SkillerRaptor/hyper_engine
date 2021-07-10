/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <HyperEngine/IApplication.hpp>
#include <cstdint>

namespace HyperEditor
{
	class CEditor : public HyperEngine::IApplication
	{
	public:
		CEditor() = default;
		virtual ~CEditor() override = default;

	private:
		virtual void startup() override;
		virtual void shutdown() override;
	};
} // namespace HyperEditor
