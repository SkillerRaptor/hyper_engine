#pragma once

#include "HyperCore/Core.hpp"

namespace Hyperion
{
	template <typename T>
	class StackLinkedList
	{
	public:
		struct Node
		{
			T Data;
			Node* NextNode;
		};

	private:
		Node* m_Head;

	public:
		StackLinkedList() = default;
		StackLinkedList(const StackLinkedList& other) = default;

		void push(T data)
		{
			auto* node = new Node();

			HP_ASSERT(node != nullptr, "Heap Overflow");

			node->Data = data;
			node->NextNode = m_Head;
			m_Head = node;
		}

		void push(T&& data)
		{
			auto* node = new Node();

			HP_ASSERT(node != nullptr, "Heap Overflow");

			node->Data = std::move(data);
			node->NextNode = m_Head;
			m_Head = node;
		}

		T peek()
		{
			HP_ASSERT(m_Head != nullptr, "Stack Linked List is empty!");
			return m_Head->Data;
		}

		void pop()
		{
			HP_ASSERT(m_Head != nullptr, "Stack Linked List is empty!");

			Node* node = m_Head;
			m_Head = m_Head->NextNode;
			node->NextNode = nullptr;
			delete node;
		}

		bool empty()
		{
			return m_Head == nullptr;
		}
	};
}
