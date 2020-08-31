#pragma once

#include <memory>

#include "Utilities/NonCopyable.h"
#include "Utilities/NonMoveable.h"

namespace Hyperion
{
	struct PoolChunk {
		PoolChunk* NextPoolChunk;
	};

	class PoolAllocator : public NonCopyable, NonMoveable
	{
	private:
		size_t m_Size;

		PoolChunk* m_Head = nullptr;

	public:
		explicit PoolAllocator(size_t size = 1024);

		void* Allocate(size_t size);
		void Deallocate(void* chunk, size_t size = 0);
	};
}