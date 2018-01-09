#ifndef _H_TSORT_
#define _H_TSORT_

#include <pthread.h>
#ifdef WIN32
#include <windows.h>
#include <time.h>
int gettimeofday(struct timeval *tp, void *tzp);
#else
#include <sys/time.h>
#endif
#include "FIFO.hpp"

class TSort
{
public:
	static TSort* instance();
	static void unInstance();

private:
	TSort();
	~TSort();

public:	
	void startThread();
	void stopThread();
	void makeRandomString(char* buff, int length);

public:
	void QuickSortEx();
	void QuickSort(char a[], int low, int high);
	static void* producerFunc(void *arg);// 生产者线程
	static void* consumerFunc(void *arg);// 消费者线程

public:
	bool m_stopProducer;
	bool m_stopConsumer;
	pthread_cond_t cond;
	char m_initString[15];
	char m_unorderString[15];
	char m_orderedString[15];

protected:
private:
	pthread_t m_producer_h;
	pthread_t m_consumer_h;

private:
	static TSort* _instance;
};

#endif
