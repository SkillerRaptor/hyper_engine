/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <catch2/catch_session.hpp>
#include <cstdint>

int32_t main(int32_t argc, char** argv)
{
	return Catch::Session().run(argc, argv);
}
