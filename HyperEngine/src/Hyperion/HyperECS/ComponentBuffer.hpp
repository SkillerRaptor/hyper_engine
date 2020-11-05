#pragma once

#include <queue>

#include "HyperCore/Core.hpp"

namespace Hyperion
{
	class ComponentBuffer
	{
	private:
		void* m_Data = nullptr;

		std::queue<std::pair<size_t, void*>> m_FreeList;

		size_t m_TotalSize;
		size_t m_Size;
		size_t m_Offset;

	public:
		ComponentBuffer(size_t totalSize = 1024);
		~ComponentBuffer();

		std::pair<size_t, void*> Allocate(const size_t& size, const size_t& alignment = 0);
		void Free(std::pair<size_t, void*> ptrPair);

		void* Get(size_t offset);

		const size_t Size() const { return m_Size; }

	private:
	};
}
