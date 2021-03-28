#pragma once

#include <HyperCore/HyperAssert.hpp>
#include <HyperCore/HyperPrerequisites.hpp>

#include <cstdint>
#include <cstddef>

namespace HyperEngine
{
	template <typename T>
	class SparseMemoryPool
	{
	public:
		explicit SparseMemoryPool(size_t poolSize)
			: m_poolSize{ poolSize }
		{
			HYPERENGINE_ASSERT(sizeof(T) >= 4, "SparseMemoryPool: Size of T is less than 4 bytes!");
			
			m_pMemory = new T[poolSize];
			for (size_t i = 0; i < m_poolSize; i++)
			{
				uint32_t* pBlock{ reinterpret_cast<uint32_t*>(&m_pMemory[i]) };
				*pBlock = i + 1;
			}
		}
		
		~SparseMemoryPool()
		{
			delete[] m_pMemory;
		}
		
		T& Allocate(size_t& index)
		{
			HYPERENGINE_ASSERT(m_allocationCount < m_poolSize, "SparseMemoryPool: Out of memory!");
			
			uint32_t* pBlock{ reinterpret_cast<uint32_t*>(&m_pMemory[m_nextAllocation]) };
			m_nextAllocation = *pBlock;
			
			index = m_allocationCount++;
			
			return m_pMemory[index];
		}
		
		void Deallocate(size_t index)
		{
			HYPERENGINE_ASSERT(index < m_poolSize, "SparseMemoryPool: Index out of range!");
			
			uint32_t* pBlock{ reinterpret_cast<uint32_t*>(&m_pMemory[m_nextAllocation]) };
			*pBlock = m_nextAllocation;
			
			m_nextAllocation = index;
			m_allocationCount--;
		}
		
		void Clear()
		{
			m_nextAllocation = 0;
			m_allocationCount = 0;
			
			for (size_t i = 0; i < m_poolSize; i++)
			{
				uint32_t* pBlock{ reinterpret_cast<uint32_t*>(&m_pMemory[i]) };
				*pBlock = i + 1;
			}
		}
		
		inline T& operator[](size_t index) noexcept
		{
			HYPERENGINE_ASSERT(index < m_poolSize, "SparseMemoryPool: Index out of range!");
			return m_pMemory[index];
		}
		
		inline const T& operator[](size_t index) const noexcept
		{
			HYPERENGINE_ASSERT(index < m_poolSize, "SparseMemoryPool: Index out of range!");
			return m_pMemory[index];
		}
		
		inline size_t GetPoolSize() const { return m_poolSize; }
		inline size_t GetAllocationCount() const { return m_allocationCount; }
	
	private:
		HYPERENGINE_NON_COPYABLE(SparseMemoryPool);
	
	private:
		T* m_pMemory{ nullptr };
		size_t m_poolSize{ 0 };
		size_t m_allocationCount{ 0 };
		uint32_t m_nextAllocation{ 0 };
	};
}