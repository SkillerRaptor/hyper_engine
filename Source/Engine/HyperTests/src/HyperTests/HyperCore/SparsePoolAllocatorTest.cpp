/*
 * Copyright (c) 2020-2021, SkillerRaptor <skillerraptor@protonmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#include <HyperCore/Memory/SparsePoolAllocator.hpp>
#include <catch2/catch_test_macros.hpp>
#include <limits>

using namespace HyperCore;

TEST_CASE("Sparse pool allocator can be created", "[SparsePoolAllocator]")
{
	static constexpr const size_t max_size = 100;
	CSparsePoolAllocator<int64_t> sparse_pool_allocator(max_size);

	REQUIRE(sparse_pool_allocator.empty());
	REQUIRE(sparse_pool_allocator.data() != nullptr);
	REQUIRE(sparse_pool_allocator.max_size() == max_size);
}

TEST_CASE("SparsePoolAllocator allocating and deallocating item", "[SparsePoolAllocator]")
{
	CSparsePoolAllocator<int64_t> sparse_pool_allocator;
	using IndexType = CSparsePoolAllocator<int64_t>::IndexType;

	auto index = std::numeric_limits<IndexType>::max();

	sparse_pool_allocator.allocate(index);

	REQUIRE(index == 0);
	REQUIRE_FALSE(index == std::numeric_limits<IndexType>::max());

	REQUIRE(sparse_pool_allocator.size() == 1);
	REQUIRE_FALSE(sparse_pool_allocator.empty());

	sparse_pool_allocator.deallocate(index);

	REQUIRE(index == std::numeric_limits<IndexType>::max());
	REQUIRE_FALSE(index == 0);

	REQUIRE(sparse_pool_allocator.empty());
	REQUIRE_FALSE(sparse_pool_allocator.size() == 1);
}

TEST_CASE("SparsePoolAllocator allocating and deallocating multiple items", "[SparsePoolAllocator]")
{
	CSparsePoolAllocator<int64_t> sparse_pool_allocator;
	using IndexType = CSparsePoolAllocator<int64_t>::IndexType;

	static constexpr const size_t max_items = 100;
	std::vector<IndexType> indices(max_items);

	for (size_t i = 0; i < max_items; ++i)
	{
		auto index = std::numeric_limits<IndexType>::max();
		sparse_pool_allocator.allocate(index);

		REQUIRE(index == i);
		REQUIRE_FALSE(index == std::numeric_limits<IndexType>::max());

		REQUIRE_FALSE(sparse_pool_allocator.empty());

		indices.push_back(index);
	}

	for (size_t i = 0; i < max_items; ++i)
	{
		IndexType& index = indices[i];
		sparse_pool_allocator.deallocate(index);

		REQUIRE(index == std::numeric_limits<IndexType>::max());
		REQUIRE_FALSE(index == 0);
	}

	REQUIRE(sparse_pool_allocator.empty());
}
