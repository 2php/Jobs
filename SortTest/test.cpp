#include "FIFO.hpp"
#include "TSort.h"
#include <stdio.h>


int main(int argc, char * argv[])
{
	//int a[8] = { 50, 10, 20, 30, 70, 40, 80, 60 };
	//Queue<int, 8> duilie;
	//for (int i = 0; i < 8; ++i)
	//{
	//	duilie.enqueue(a[i]);
	//}
	//duilie.traverse();
	//duilie.dequeue();//出队
	//duilie.dequeue();//出队
	//duilie.traverse();

	//单线程方式
	TSort::instance()->QuickSortEx();
	printf("press any key to go:\n");
	getchar();
	//多线程方式
	TSort::instance()->startThread();
	//
	getchar();
	TSort::instance()->stopThread();
	TSort::unInstance();



	return 0;
}


