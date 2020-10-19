#pragma once

#include <memory.h>
#include <iostream>
#include <assert.h>     /* assert */


template <class myType>
class MyVector
{
public:
	MyVector()
	{
		vector = new myType[50];
		head = 0;
		currentSize = 50;
	}

	MyVector(int width)
	{
		vector = new myType[width];
		head = 0;
		currentSize = width;
	}

	MyVector(const MyVector& sample)
	{
		vector = new myType[sample.currentSize];
		head = 0;
		currentSize = sample.currentSize;

		this->head = sample.head;
		this->currentSize = sample.currentSize;

		for (int i = 0; i < currentSize; i++)
		{
			this->vector[i] = sample.vector[i];
		}
	}

	~MyVector()
	{
		delete vector;
	}

	void push_back(myType value)
	{
		if (head == currentSize)
			doubleVectorSize();

		vector[head] = value;
		head++;
	}

	myType pop_back()
	{
		assert(head > 0);
		head--;

		return vector[head];
	}

	myType at(int index)
	{
		assert(head > index);

		return vector[index];
	}
	
	MyVector &operator=(const MyVector& sample)
	{
		if (this != &sample) // self-assignment check expected
		{
			this->head = sample.head;
			this->currentSize = sample.currentSize;
			delete vector;
			vector = new myType[currentSize];

			for (int i = 0; i < currentSize; i++)
			{
				this->vector[i] = sample.vector[i];
			}
		}

		return *this;
	}

	myType &operator[](int index)
	{
		assert(index < head);

		return vector[index];
	}

	int size()
	{
		return head;
	}

	bool empty()
	{
		return head == 0;
	}

	void swap(int index1, int index2)
	{
		assert(index1 < head && index2 < head);

		if (index1 == index2)
			return;

		myType tmp = vector[index1];
		vector[index1] = vector[index2];
		vector[index2] = tmp;
	}

private:
	myType *vector;
	unsigned head;
	unsigned currentSize;

	void doubleVectorSize()
	{ 
		myType* newVector = new myType[currentSize*2];

		memcpy(newVector, vector, sizeof(myType) * currentSize);
		currentSize *= 2;
		
		delete vector;
		
		vector = newVector;
	}
};
