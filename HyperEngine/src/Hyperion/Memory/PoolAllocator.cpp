#include "PoolAllocator.h"

namespace Hyperion
{
	PoolAllocator::PoolAllocator(size_t size)
		: m_Size(size)
	{}

	void* PoolAllocator::Allocate(size_t size)
	{
		if (m_Head == nullptr)
		{
			size_t poolBlockSize = m_Size * size;
			PoolChunk* poolBlockBegin = reinterpret_cast<PoolChunk*>(malloc(poolBlockSize));
			PoolChunk* poolChunk = poolBlockBegin;
			for (int i = 0; i < m_Size - 1; ++i)
			{
				poolChunk->NextPoolChunk = reinterpret_cast<PoolChunk*>(reinterpret_cast<char*>(poolChunk) + size);
				poolChunk = poolChunk->NextPoolChunk;
			}
			poolChunk->NextPoolChunk = nullptr;
			return poolBlockBegin;
		}

		PoolChunk* freePoolChunk = m_Head;
		m_Head = m_Head->NextPoolChunk;
		return freePoolChunk;
	}

	void PoolAllocator::Deallocate(void* chunk, size_t size)
	{
		PoolChunk* poolChunk = reinterpret_cast<PoolChunk*>(chunk);
		poolChunk->NextPoolChunk = m_Head;
		m_Head = poolChunk;
	}
}