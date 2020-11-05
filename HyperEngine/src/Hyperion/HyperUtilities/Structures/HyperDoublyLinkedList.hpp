#pragma once

namespace Hyperion
{
	template <typename T>
	class HyperDoublyLinkedList
	{
	public:
		struct Node
		{
			T Data;
			Node* PreviousNode;
			Node* NextNode;
		};

	private:
		Node* m_Head;

	public:
		HyperDoublyLinkedList() = default;
		HyperDoublyLinkedList(const HyperDoublyLinkedList& other) = default;

		void Insert(Node* previousNode, Node* newNode)
		{
			if (previousNode == nullptr) 
			{
				if (head != nullptr) 
				{
					newNode->NextNode = head;
					newNode->NextNode->PreviousNode = newNode;
				}
				else 
					newNode->NextNode = nullptr;
				head = newNode;
				head->PreviousNode = nullptr;
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
					if (newNode->NextNode != nullptr)
						newNode->NextNode->PreviousNode = newNode;
					previousNode->NextNode = newNode;
					newNode->PreviousNode = previousNode;
				}
			}
		}

		void Remove(Node* deleteNode)
		{
			if (deleteNode->PreviousNode == nullptr) 
				if (deleteNode->NextNode == nullptr) 
					head = nullptr;
				else 
				{
					head = deleteNode->NextNode;
					head->PreviousNode = nullptr;
				}
			else
				if (deleteNode->NextNode == nullptr) 
					deleteNode->PreviousNode->NextNode = nullptr;
				else 
				{
					deleteNode->PreviousNode->NextNode = deleteNode->NextNode;
					deleteNode->NextNode->PreviousNode = deleteNode->PreviousNode;
				}
		}
	};
}
