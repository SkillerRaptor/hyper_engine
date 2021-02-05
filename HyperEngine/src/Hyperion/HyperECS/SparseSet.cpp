#include "SparseSet.hpp"

#include <algorithm>

namespace Hyperion
{
	SparseSet::~SparseSet()
	{
		Clear();
	}

	void SparseSet::Emplace(const Entity entity, void* component)
	{
		if (Contains(entity))
			return;

		size_t size = m_Packed.size();

		m_Packed.push_back(entity);
		m_ComponentArray.push_back(std::move(component));

		if (size <= entity)
			m_Sparse.resize(entity + 1);

		m_Sparse[entity] = size;
	}

	void SparseSet::Remove(const Entity entity)
	{
		if (!Contains(entity))
			return;

		std::swap(m_ComponentArray[m_Sparse[entity]], m_ComponentArray.back());
		m_Packed[m_Sparse[entity]] = m_Packed.back();
		m_Sparse[m_Packed.back()] = m_Sparse[entity];

		delete m_ComponentArray.back();
		m_ComponentArray.pop_back();
		m_Packed.pop_back();
	}

	void* SparseSet::Get(const Entity entity)
	{
		if (!Contains(entity))
			return nullptr;

		return m_ComponentArray[m_Sparse[entity]];
	}

	bool SparseSet::Contains(const Entity entity) const
	{
		if (entity >= m_Sparse.size())
			return false;

		if (m_Sparse[entity] == Null)
			return false;

		try
		{
			return m_Packed.at(m_Sparse.at(entity)) == entity;
		}
		catch (std::exception ex)
		{
			return false;
		}
	}

	void SparseSet::Clear()
	{
		m_Sparse.clear();
		m_Packed.clear();

		for (size_t i = 0; i < m_ComponentArray.size(); i++)
			delete m_ComponentArray[i];
		m_ComponentArray.clear();
	}

	bool SparseSet::Empty() const noexcept
	{
		return m_Packed.empty();
	}

	const Entity* SparseSet::Data() const noexcept
	{
		return m_Packed.data();
	}

	size_t SparseSet::Size() const noexcept
	{
		return m_Packed.size();
	}

	size_t SparseSet::Capacity() const noexcept
	{
		return m_Packed.capacity();
	}

	SparseSet::Iterator SparseSet::begin()
	{
		return m_Packed.begin();
	}

	SparseSet::ConstIterator SparseSet::begin() const
	{
		return m_Packed.begin();
	}

	SparseSet::Iterator SparseSet::end()
	{
		return m_Packed.end();
	}

	SparseSet::ConstIterator SparseSet::end() const
	{
		return m_Packed.end();
	}
}
