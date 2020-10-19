#include "ComponentBuffer.hpp"

namespace Hyperion
{
	ComponentBuffer::ComponentBuffer(size_t totalSize)
		: m_TotalSize(totalSize), m_Offset(0), m_Size(0)
	{
		if (m_Data != nullptr)
			free(m_Data);

		m_Data = malloc(m_TotalSize);
	}

	ComponentBuffer::~ComponentBuffer()
	{
		 free (m_Data);
		 m_Data = nullptr;
	}

	std::pair<size_t, void*> ComponentBuffer::Allocate(const size_t& size, const size_t& alignment)
	{
		size_t padding = 0;
		const size_t currentAddress = (size_t) m_Data + m_Offset;

		if (alignment != 0 && m_Offset % alignment != 0)
			padding = ((currentAddress / alignment) + 1) * alignment - currentAddress;

		if (!m_FreeList.empty())
		{
			std::pair<size_t, void*> ptrPair = m_FreeList.front();
			m_FreeList.pop();
			return ptrPair;
		}

		if (m_Offset + padding + size > m_TotalSize)
			return std::pair<uint32_t, void*>(-1, nullptr);

		m_Offset += padding;

		const size_t nextAddress = currentAddress + padding;
		m_Offset += size;
		m_Size++;

		return std::pair<size_t, void*>(m_Offset - size, (void*) nextAddress);
	}

	void ComponentBuffer::Free(std::pair<size_t, void*> ptrPair)
	{
		m_FreeList.push(ptrPair);
	}

	void* ComponentBuffer::Get(size_t offset)
	{
		return (void*) ((size_t) m_Data + offset);
	}
}
