#pragma once

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

		void Push(Node* node)
		{
			node->NextNode = m_Head;
			m_Head = node;
		}

		Node* Pop()
		{
			Node* node = m_Head;
			m_Head = m_Head->NextNode;
			return node;
		}
	};
}
