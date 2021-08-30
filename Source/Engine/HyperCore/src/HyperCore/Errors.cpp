/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include "HyperCore/Errors.hpp"

namespace HyperCore
{
	auto operator<<(std::ostream& ostream, const ConstructError& construct_error) -> std::ostream&
	{
		switch (construct_error)
		{
		case ConstructError::None:
			ostream << "none";
			break;
		case ConstructError::BadArgument:
			ostream << "bad argument";
			break;
		case ConstructError::BadMemory:
			ostream << "bad memory";
			break;
		case ConstructError::Incomplete:
			ostream << "incomplete";
			break;
		case ConstructError::OutOfMemory:
			ostream << "out of memory";
			break;
		case ConstructError::UndefinedBehaviour:
			ostream << "undefined behaviour";
			break;
		case ConstructError::Uninitialized:
			ostream << "uninitialized";
			break;
		default:
			ostream << "undefined";
			break;
		}
		return ostream;
	}
	
	auto operator<<(std::ostream& ostream, const RuntimeError& runtime_error) -> std::ostream&
	{
		switch (runtime_error)
		{
		case RuntimeError::None:
			ostream << "none";
			break;
		case RuntimeError::OutOfBounds:
			ostream << "out of bounds";
			break;
		case RuntimeError::Uninitialized:
			ostream << "uninitialized";
			break;
		case RuntimeError::UndefinedBehaviour:
			ostream << "undefined behaviour";
			break;
		default:
			ostream << "undefined";
			break;
		}
		return ostream;
	}
} // namespace HyperCore
