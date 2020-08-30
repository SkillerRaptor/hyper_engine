#pragma once

#include <memory>

namespace Hyperion
{
	struct PoolChunk {
		PoolChunk* NextPoolChunk;
	};

	class PoolAllocator
	{
	private:
		size_t m_Size;

		PoolChunk* m_Head = nullptr;

	public:
		explicit PoolAllocator(size_t size = 1024);

		PoolAllocator(const PoolAllocator& poolAllocator) = delete;
		PoolAllocator& operator=(const PoolAllocator& poolAllocator) = delete;
		PoolAllocator(PoolAllocator&& poolAllocator) = delete;
		PoolAllocator& operator=(PoolAllocator&& poolAllocator) = delete;

		void* Allocate(size_t size);
		void Deallocate(void* chunk, size_t size = 0);
	};
}