#pragma once

namespace Hyperion
{
	template <typename T>
	class HyperStackLinkedList
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
		HyperStackLinkedList() = default;
		HyperStackLinkedList(const HyperStackLinkedList& other) = default;

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
