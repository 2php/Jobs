#ifndef __FIFO_QUEUE_H
#define __FIFO_QUEUE_H

#include <iostream>
using namespace std;

template<typename T, int size = 0>
class Queue{
public:
	Queue();
public:
	bool is_empty() const;//是否队空
	bool is_full() const;//是否队满

	void enqueue(const T&);//入队
	T dequeue();//出队

	void traverse() const;//遍历队列（打印）
private:
	T data[size];//数组实现FIFO队列
	int first;
	int last;
};

//注意模板的声明与实现要放在同一个.h文件里

template<typename T, int size>
Queue<T, size>::Queue()
{
	first = last = -1;
}

template<typename T, int size>
bool Queue<T, size>::is_empty() const
{
	return (first == -1);
}

template<typename T, int size>
bool Queue<T, size>::is_full() const
{
	return first == 0 && last == size - 1 || last == first - 1;
}

template<typename T, int size>
void Queue<T, size>::enqueue(const T& elem)
{
	if (!is_full())
	{
		if (last == -1 || last == size - 1)
		{
			data[0] = elem;
			last = 0;
			if (first == -1)
			{
				first = 0;
			}
		}
		else
		{
			data[++last] = elem;
		}
	}
	else
	{
		cout << "Queue full." << endl;
	}
}

template<typename T, int size>
T Queue<T, size>::dequeue()
{
	if (is_empty())
	{
		cout << "Queue empty." << endl;
	}

	T tmp;
	tmp = data[first];
	if (first == last)
	{
		last = first = -1;
	}
	else if (first == size - 1)
	{
		first = 0;
	}
	else
	{
		++first;
	}

	return tmp;
}

template<typename T, int size>
void Queue<T, size>::traverse() const
{
	for (int i = first; i <= last; ++i)
	{
		cout << data[i] << " ";
	}
	cout << endl;
}

#endif //__FIFO_QUEUE_H
