#pragma once

namespace Hyperion
{
	template <typename T>
	class HyperSinglyLinkedList
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
		HyperSinglyLinkedList() = default;
		HyperSinglyLinkedList(const HyperSinglyLinkedList& other) = default;

		void Insert(Node* previousNode, Node* newNode)
		{
			if (previousNode == nullptr) {
				if (head != nullptr) 
					newNode->NextNode = head;
				else 
					newNode->NextNode = nullptr;
				head = newNode;
			}
			else 
			{
				if (previousNode->NextNode == nullptr)
				{
					previousNode->NextNode = newNode;
					newNode->NextNode = nullptr;
				}
				else 
				{
					newNode->NextNode = previousNode->NextNode;
					previousNode->NextNode = newNode;
				}
			}
		}

		void Remove(Node* previousNode, Node* deleteNode)
		{
			if (previousNode == nullptr) 
				if (deleteNode->NextNode == nullptr)
					head = nullptr;
				else 
					head = deleteNode->NextNode;
			else 
				previousNode->NextNode = deleteNode->NextNode;
		}
	};
}
