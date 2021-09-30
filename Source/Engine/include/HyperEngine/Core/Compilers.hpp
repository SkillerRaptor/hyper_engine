/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#if defined(__clang__)
#	include "HyperEngine/Core/Compilers/ClangSpecific.hpp"
#elif defined(__GNUC__) || defined(__GNUG__)
#	include "HyperEngine/Core/Compilers/GCCSpecific.hpp"
#elif defined(_MSC_VER)
#	include "HyperEngine/Core/Compilers/MSVCSpecific.hpp"
#else
#	error Unsupported compiler was used.
#endif
