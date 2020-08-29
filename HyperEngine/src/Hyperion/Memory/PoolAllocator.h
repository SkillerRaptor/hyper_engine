#pragma once

#include <memory>

namespace Hyperion
{
	template <typename T>
	union PoolChunk {
		T Value;
		PoolChunk<T>* NextPoolChunk;

		PoolChunk() {};
		~PoolChunk() {};
	};

	template <typename T, typename Allocator = std::allocator<PoolChunk<T>>>
	class PoolAllocator
	{
	private:
		size_t m_Size;
		std::unique_ptr<Allocator> m_ParentAllocator;

		PoolChunk<T>* m_Data = nullptr;
		PoolChunk<T>* m_Head = nullptr;

		size_t m_Allocations = 0;

	public:
		explicit PoolAllocator(size_t size = 1024, Allocator* parentAllocator = nullptr)
			: m_Size(size), m_ParentAllocator(parentAllocator)
		{
			if (parentAllocator == nullptr) m_ParentAllocator = std::make_unique<Allocator>();
			m_Data = m_ParentAllocator->allocate(m_Size);
			m_Head = m_Data;

			for (size_t i = 0; i < m_Size - 1; i++)
				m_Data[i].NextPoolChunk = std::addressof(m_Data[i + 1]);
			m_Data[m_Size - 1].NextPoolChunk = nullptr;
		}

		~PoolAllocator()
		{
			if (m_Allocations != 0)
				__debugbreak();

			m_ParentAllocator->deallocate(m_Data, m_Size);

			m_Data = nullptr;
			m_Head = nullptr;
		}

		PoolAllocator(const PoolAllocator& poolAllocator) = delete;
		PoolAllocator& operator=(const PoolAllocator& poolAllocator) = delete;
		PoolAllocator(PoolAllocator&& poolAllocator) = delete;
		PoolAllocator& operator=(PoolAllocator&& poolAllocator) = delete;

		template <typename... Targs>
		T* Allocate(Targs&&... Fargs)
		{
			if (m_Head == nullptr)
				return nullptr;

			PoolChunk<T>* poolChunk = m_Head;
			m_Head = poolChunk->NextPoolChunk; 

			T* cls = new (std::addressof(poolChunk->Value)) T(std::forward<Targs>(Fargs)...);
			m_Allocations++;
			return cls;
		}

		void Deallocate(T* data)
		{
			m_Allocations--;
			data->~T();
			PoolChunk<T>* poolChunk = reinterpret_cast<PoolChunk<T>*>(data);
			poolChunk->NextPoolChunk = m_Head;
			m_Head = poolChunk;
		}
	};
}