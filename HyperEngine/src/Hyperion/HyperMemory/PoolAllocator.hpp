#pragma once

#include <memory>
#include <queue>

#include "HyperUtilities/NonCopyable.hpp"
#include "HyperUtilities/NonMoveable.hpp"
#include "HyperUtilities/StackLinkedList.hpp"

namespace Hyperion
{
	class PoolAllocator : public NonCopyable, NonMoveable
	{
	private:
		struct FreeHeader {};

		StackLinkedList<FreeHeader> m_FreeList;
		using Node = StackLinkedList<FreeHeader>::Node;

		void* m_StartPtr = nullptr;
		size_t m_TotalSize = 0;
		size_t m_ChunkSize = 0;
		size_t m_Size = 0;
		size_t m_Capacity = 0;

	public:
		explicit PoolAllocator(size_t totalSize = 100, size_t chunkSize = 1024);
		~PoolAllocator();

		void* Allocate(const size_t allocationSize, const size_t alignment = 0);
		void Free(void* ptr);
		void Reset();
	};
}
