#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>

#include "HyperECS/Entity.hpp"

namespace HyperECS
{
	class SparseSet
	{
	public:
		SparseSet() = default;
		virtual ~SparseSet() = default;
		
		virtual void Remove(const Entity entity) = 0;
		
		virtual void* Get(const Entity entity) = 0;
		virtual bool Contains(const Entity entity) const = 0;
		
		virtual void Clear() = 0;
	};
	
	template <typename T>
	class SparseSetImpl : public SparseSet
	{
	public:
		using Iterator = typename std::vector<Entity>::const_iterator;
		using ConstIterator = typename std::vector<Entity>::const_iterator;

	private:
		std::vector<Entity> m_Sparse = {};
		std::vector<Entity> m_Packed = {};
		std::vector<T> m_ComponentArray = {};

	public:
		SparseSetImpl() = default;
		SparseSetImpl(SparseSetImpl&& other) = default;
		~SparseSetImpl()
		{
			Clear();
		}
		
		SparseSetImpl& operator=(SparseSetImpl&& other) = default;
		
		template<typename... Args>
		T& Emplace(const Entity entity, Args&&... args)
		{
			if (Contains(entity))
				throw std::runtime_error("Sparse Set already contains entity");
			
			size_t size = m_Packed.size();
			
			m_Packed.push_back(entity);
			
			if (size <= entity)
				m_Sparse.resize(entity + 1);
			
			m_Sparse[entity] = size;
			return m_ComponentArray.emplace_back(std::forward<Args>(args)...);
		}
		
		virtual void Remove(const Entity entity) override
		{
			if (!Contains(entity))
				return;
			
			std::swap(m_ComponentArray[m_Sparse[entity]], m_ComponentArray.back());
			m_Packed[m_Sparse[entity]] = m_Packed.back();
			m_Sparse[m_Packed.back()] = m_Sparse[entity];
			
			m_ComponentArray.pop_back();
			m_Packed.pop_back();
		}
		
		void* Get(const Entity entity)
		{
			if (!Contains(entity))
				return nullptr;
			
			return &m_ComponentArray[m_Sparse[entity]];
		}
		
		virtual bool Contains(const Entity entity) const override
		{
			if (entity >= m_Sparse.size())
				return false;
			
			if (m_Sparse[entity] == Null)
				return false;
			
			try
			{
				return m_Packed.at(m_Sparse.at(entity)) == entity;
			}
			catch (const std::exception& ex)
			{
				return false;
			}
		}

		virtual void Clear() override
		{
			m_Sparse.clear();
			m_Packed.clear();
			m_ComponentArray.clear();
		}

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
