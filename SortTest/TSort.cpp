#include "TSort.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN32
int gettimeofday(struct timeval *tp, void *tzp)
{
	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	clock = mktime(&tm);
	tp->tv_sec = clock;
	tp->tv_usec = wtm.wMilliseconds * 1000;
	return (0);
}
#endif

//---------------------------------------------//
Queue<char, 12> g_bufferQueue;//FIFO队列
pthread_mutex_t g_queueMutex;//互斥量
pthread_mutex_t g_timeMutex;//互斥量
bool g_bQueueFull = false;//是否队满
bool g_bReadySort = false;//是否开始排序
//---------------------------------------------//
TSort* TSort::_instance = NULL;
//---------------------------------------------//

TSort* TSort::instance()
{
	if (!_instance){
		_instance = new TSort();
	}
	return _instance;
}

void TSort::unInstance()
{
	printf("----TSort::unInstance----\n");
	if (_instance){
		delete _instance;
		_instance = NULL;
	}
}

TSort::TSort()
{
	m_stopProducer = false;
	m_stopConsumer = false;

	pthread_cond_init(&cond, NULL);
	pthread_mutex_init(&g_queueMutex, NULL);
	pthread_mutex_init(&g_timeMutex, NULL);

	memset(m_initString, 0, 15);
	memset(m_orderedString, 0, 15);
	memset(m_unorderString, 0, 15);

	char temp[13] = {0};
	makeRandomString(temp, 12);
	strncpy(m_initString, temp, 12);
	strncpy(m_unorderString, temp, 12);
}

TSort::~TSort()
{
	pthread_cond_destroy(&cond);
	pthread_mutex_destroy(&g_queueMutex);
	pthread_mutex_destroy(&g_timeMutex);
}

void TSort::startThread()
{
	if (0 != pthread_create(&m_producer_h, NULL, producerFunc, this))
	{
		return;
	}
	if (0 != pthread_create(&m_consumer_h, NULL, consumerFunc, this))
	{
		this->stopThread();
		return;
	}
}

void TSort::stopThread(){	
	m_stopProducer = true;
	m_stopConsumer = true;
	//pthread_join(m_producer_h, NULL);
	//pthread_join(m_consumer_h, NULL);
}

//快速排序
void TSort::QuickSort(char a[], int low, int high)
{
	if (low >= high)
	{
		return;
	}
	int first = low;
	int last  = high;
	int key = a[first];//用字表的第一个记录作为枢轴

	while (first < last)
	{
		while (first < last && (int)a[last] >= key)
		{
			--last;
		}

		a[first] = a[last];//将比第一个小的移到低端

		while (first < last && (int)a[first] <= key)
		{
			++first;
		}

		a[last] = a[first];//将比第一个大的移到高端
	}
	a[first] = key;//枢轴记录到位
	QuickSort(a, low, first - 1);//比key小的
	QuickSort(a, first + 1, high);//比key大的
}

void* TSort::producerFunc(void *arg)  //生产者线程
{
	TSort* pSortThread = (TSort*)arg;
	if (!pSortThread) {
		return NULL;
	}

	struct timeval now;
	struct timespec outtime;

	int index = 0;

	while ( !pSortThread->m_stopProducer )
	{
		pthread_mutex_lock(&g_queueMutex);
		//printf("in producer thread=%ld\n", pthread_self());
        if (index < 12) {
			g_bufferQueue.enqueue(pSortThread->m_unorderString[index++]);
			g_bufferQueue.traverse();
		}        
        else if ( index>=12 ) {
            pSortThread->m_stopProducer = true;//退出线程
		}		
		pthread_mutex_unlock(&g_queueMutex);

		//这个等待还不会玩

		//Sleep 1s
		gettimeofday(&now, NULL);
		outtime.tv_sec = now.tv_sec;
		outtime.tv_nsec = now.tv_usec * 1000;
		pthread_cond_timedwait(&pSortThread->cond, &g_timeMutex, &outtime);
		
		//struct timespec abstime;
		//struct timeval now;
		//long timeout_ms = 100; // wait time 100ms
		//gettimeofday(&now, NULL);
		//long nsec = now.tv_usec * 1000 + (timeout_ms % 1000) * 1000000;
		//abstime.tv_sec = now.tv_sec + nsec / 1000000000 + timeout_ms / 1000;
		//abstime.tv_nsec = nsec % 1000000000;
		//pthread_cond_timedwait(&pSortThread->cond, &g_timeMutex, &abstime);
	}

	return NULL;
}

void* TSort::consumerFunc(void *arg)  //消费者线程
{
	TSort* pSortThread = (TSort*)arg;
	if (!pSortThread) {
		return NULL;
	}

	struct timeval now;
	struct timespec outtime;

	int index = 0;
	while ( !pSortThread->m_stopConsumer )
	{
		pthread_mutex_lock(&g_queueMutex);
		//printf("in consumer thread=%ld\n", pthread_self());
        if ( !g_bufferQueue.is_empty() ) {
			pSortThread->m_orderedString[index++] = g_bufferQueue.dequeue();
			g_bufferQueue.traverse();
            if ( index>=12 && g_bufferQueue.is_empty() ) {
                g_bReadySort = true;//可以开始排序了
			}
		}				
		pthread_mutex_unlock(&g_queueMutex);

        if ( g_bReadySort ){//开始排序
			
			int llen = strlen(pSortThread->m_orderedString);			
			
			printf("unorder string:\n");
			for (int i = 0; i < llen; ++i)
			{
				printf("%c", pSortThread->m_orderedString[i]);
			}
			printf("\n");

			pSortThread->QuickSort(pSortThread->m_orderedString, 0, 11);

			//排序结果
			printf("ordered string:\n");
			for (int i = 0; i < llen; ++i)
			{
				printf("%c", pSortThread->m_orderedString[i]);
			}
			printf("\n");
			break;//退出线程
		}

		//这个等待还不会玩

		//Sleep 1s
		gettimeofday(&now, NULL);
		outtime.tv_sec = now.tv_sec;
		outtime.tv_nsec = now.tv_usec * 1000;
		pthread_cond_timedwait(&pSortThread->cond, &g_timeMutex, &outtime);

		//struct timespec abstime;
		//struct timeval now;
		//long timeout_ms = 100; // wait time 100ms
		//gettimeofday(&now, NULL);
		//long nsec = now.tv_usec * 1000 + (timeout_ms % 1000) * 1000000;
		//abstime.tv_sec = now.tv_sec + nsec / 1000000000 + timeout_ms / 1000;
		//abstime.tv_nsec = nsec % 1000000000;
		//pthread_cond_timedwait(&pSortThread->cond, &g_timeMutex, &abstime);
	}
	return NULL;
}


//随机生成字符串
void TSort::makeRandomString(char* buff, int length)
{
	if (!buff || length < 12) {
		return;
	}

	const int LEN = 62; // 26 + 26 + 10
	char g_arrCharElem[LEN] = {
		'0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
		'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
		'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'x', 'y', 'z' };
	//
	char* szStr = new char[length + 1];
	memset(szStr, 0, length + 1);
	srand((unsigned)time(0));//播撒随机种子
	int iRand = 0, num = 0;
	for (int i = 0; i < length;)
	{
		int sj = rand();//伪随机，有更好的方法么？
		iRand = sj % LEN;// iRand = 0 - 61
		if (iRand >= 0 && iRand < 10){
			num++;
		}
		if ( num>0 ) {//保证至少有数字	
			szStr[i++] = g_arrCharElem[iRand];
		}		
	}
	szStr[length] = '\0';
	printf("the string maded: %s\n", szStr);
	strncpy(buff, szStr, length);
	delete[] szStr;
}

void TSort::QuickSortEx()
{
	QuickSort(m_initString, 0, 11);

	//排序结果
	printf("single thread ordered string:\n");
	for (int i = 0; i < 12; ++i)
	{
		printf("%c", m_initString[i]);
	}
	printf("\n");
}


