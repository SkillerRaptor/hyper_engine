/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#if defined(__clang__)
#	include <HyperCore/Compiler/ClangSpecific.hpp>
#elif defined(__GNUC__) || defined(__GNUG__)
#	include <HyperCore/Compiler/GCCSpecific.hpp>
#elif defined(_MSC_VER)
#	include <HyperCore/Compiler/MSVCSpecific.hpp>
#endif
