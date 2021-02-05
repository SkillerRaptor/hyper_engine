#pragma once

#include <vector>

#include "Entity.hpp"

namespace Hyperion
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
		bool Empty() const noexcept;

		const Entity* Data() const noexcept;

		size_t Capacity() const noexcept;
		size_t Size() const noexcept;

		Iterator begin();
		ConstIterator begin() const;

		Iterator end();
		ConstIterator end() const;
	};
}
