/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <string>

namespace HyperEngine
{
	class Error
	{
	public:
		explicit Error(std::string error);

		std::string error() const;

	private:
		std::string m_error;
	};
} // namespace HyperEngine
