#pragma once

#include <vector>

#include "HyperECS/Entity.hpp"

namespace HyperECS
{
	class SparseSet
	{
	public:
		using Iterator = typename std::vector<Entity>::const_iterator;
		using ConstIterator = typename std::vector<Entity>::const_iterator;

	private:
		std::vector<Entity> m_Sparse = {};
		std::vector<Entity> m_Packed = {};
		std::vector<void*> m_ComponentArray = {};

	public:
		SparseSet() = default;
		SparseSet(SparseSet&& other) = default;
		~SparseSet();

		SparseSet& operator=(SparseSet&& other) = default;

		void Emplace(const Entity entity, void* component = nullptr);
		void Remove(const Entity entity);
		void* Get(const Entity entity);

		bool Contains(const Entity entity) const;

		void Clear();

		inline bool Empty() const noexcept
		{
			return m_Packed.empty();
		}

		inline const Entity* Data() const noexcept
		{
			return m_Packed.data();
		}

		inline size_t Capacity() const noexcept
		{
			return m_Packed.capacity();
		}

		inline size_t Size() const noexcept
		{
			return m_Packed.size();
		}

		inline Iterator begin()
		{
			return m_Packed.begin();
		}

		inline ConstIterator begin() const
		{
			return m_Packed.begin();
		}

		inline Iterator end()
		{
			return m_Packed.end();
		}

		inline ConstIterator end() const
		{
			return m_Packed.end();
		}
	};
}
