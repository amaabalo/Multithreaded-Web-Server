// mythread.cc
// best viewed with tabstop=4
#include "mythread.hpp"

MyThread::MyThread(int id, ThreadSafeListenerQueue<MyThread*> *addr, container<std::string, std::string> *stores, container<std::string, std::string> *hstores, void *(*start)(void *), int n_s)
{
	
	tid = id;
	sockfd = -1;	// signifies the thread is idle
	parent = addr;
	kvstores = stores;
	khstores = hstores;
	n_stores = n_s;
	inserts = 0;
	deletes = 0;
	lookups = 0;
	start_routine = start;	
	pthread_cond_init(&idle_cv, NULL);
	pthread_mutex_init(&idle_mutex, NULL);
	pthread_create(&the_thread, NULL, start_routine, this);
}
