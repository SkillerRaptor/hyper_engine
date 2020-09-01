#pragma once

#include <cmath>

namespace Hyperion
{
	namespace Sorting
	{
		void Swap(int* list, int index)
		{
			Swap(list, index, index + 1);
		}

		void Swap(float* list, int index)
		{
			Swap(list, index, index + 1);
		}

		void Swap(double* list, int index)
		{
			Swap(list, index, index + 1);
		}

		void Swap(int* list, int indexOne, int indexTwo)
		{
			int temp = list[indexOne];
			list[indexOne] = list[indexTwo];
			list[indexTwo] = temp;
		}

		void Swap(float* list, int indexOne, int indexTwo)
		{
			float temp = list[indexOne];
			list[indexOne] = list[indexTwo];
			list[indexTwo] = temp;
		}

		void Swap(double* list, int indexOne, int indexTwo)
		{
			double temp = list[indexOne];
			list[indexOne] = list[indexTwo];
			list[indexTwo] = temp;
		}

		void BubbleSort(int* list, size_t arraySize)
		{
			for (size_t i = 1; i < arraySize; i++)
				for (size_t j = 0; j < arraySize - 1; j++)
					Swap(list, j);
		}

		void BubbleSort(float* list, size_t arraySize)
		{
			for (size_t i = 1; i < arraySize; i++)
				for (size_t j = 0; j < arraySize - 1; j++)
					Swap(list, j);
		}

		void BubbleSort(double* list, size_t arraySize)
		{
			for (size_t i = 1; i < arraySize; i++)
				for (size_t j = 0; j < arraySize - 1; j++)
					Swap(list, j);
		}

		void SelectionSort(int* list, size_t arraySize)
		{
			for (size_t i = 0; i < arraySize - 1; i++)
			{
				size_t minIndex = i;

				for (size_t j = i + 1; j < arraySize; j++)
					if (list[j] < list[minIndex])
						minIndex = j;

				if (minIndex != i)
					Swap(list, i, minIndex);
			}
		}

		void SelectionSort(float* list, size_t arraySize)
		{
			for (size_t i = 0; i < arraySize - 1; i++)
			{
				size_t minIndex = i;

				for (size_t j = i + 1; j < arraySize; j++)
					if (list[j] < list[minIndex])
						minIndex = j;

				if (minIndex != i)
					Swap(list, i, minIndex);
			}
		}

		void SelectionSort(double* list, size_t arraySize)
		{
			for (size_t i = 0; i < arraySize - 1; i++)
			{
				size_t minIndex = i;

				for (size_t j = i + 1; j < arraySize; j++)
					if (list[j] < list[minIndex])
						minIndex = j;

				if (minIndex != i)
					Swap(list, i, minIndex);
			}
		}

		void QuickSort(int* list, size_t leftIndex, size_t rightIndex)
		{
			size_t i = leftIndex;
			size_t j = rightIndex;
			int splitValue = list[(size_t) ceil((leftIndex + rightIndex) / 2)];

			while (i <= j)
			{
				while (list[i] < splitValue)
					i++;
				while (list[j] > splitValue)
					j--;

				if (i <= j)
				{
					Swap(list, i, j);
					i++;
					j--;
				}
			}

			if (leftIndex < j)
				QuickSort(list, leftIndex, j);
			if (i < rightIndex)
				QuickSort(list, i, rightIndex);
		}

		void QuickSort(float* list, size_t leftIndex, size_t rightIndex)
		{
			size_t i = leftIndex;
			size_t j = rightIndex;
			float splitValue = list[(size_t)ceil((leftIndex + rightIndex) / 2)];

			while (i <= j)
			{
				while (list[i] < splitValue)
					i++;
				while (list[j] > splitValue)
					j--;

				if (i <= j)
				{
					Swap(list, i, j);
					i++;
					j--;
				}
			}

			if (leftIndex < j)
				QuickSort(list, leftIndex, j);
			if (i < rightIndex)
				QuickSort(list, i, rightIndex);
		}

		void QuickSort(double* list, size_t leftIndex, size_t rightIndex)
		{
			size_t i = leftIndex;
			size_t j = rightIndex;
			double splitValue = list[(size_t)ceil((leftIndex + rightIndex) / 2)];

			while (i <= j)
			{
				while (list[i] < splitValue)
					i++;
				while (list[j] > splitValue)
					j--;

				if (i <= j)
				{
					Swap(list, i, j);
					i++;
					j--;
				}
			}

			if (leftIndex < j)
				QuickSort(list, leftIndex, j);
			if (i < rightIndex)
				QuickSort(list, i, rightIndex);
		}
	}
}