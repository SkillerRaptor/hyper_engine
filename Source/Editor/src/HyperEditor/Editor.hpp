/*
 * Copyright (c) 2021, SkillerRaptor <skillerraptor@protonmail.com>
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
		CEditor(int32_t argc, char** argv);
		virtual ~CEditor() override;
		
	private:
		virtual void startup() override;
		virtual void shutdown() override;
	};
}
