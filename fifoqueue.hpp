// fifoqueue.hpp

#include <list>
#include <pthread.h>

template <typename T>
class ThreadSafeListenerQueue
{
	private:
		std::list<T> lst;
		pthread_mutex_t FQMutex;
		pthread_cond_t FQConditionVariable;
	public:
		ThreadSafeListenerQueue();
		int push(const T element);
		int pop(T& element);
		int listen(T& element);
		void printout();
		typename std::list<T>::iterator begin();
		typename std::list<T>::iterator end();
};

#include "fifoqueue.cc"
