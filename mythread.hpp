// mythread.hpp 
// best viewed with tabstop=4

#include <string>
#include <pthread.h>
#include "fifoqueue.hpp"
#include "keyvaluestore.hpp"
#include <vector>
#include <chrono>

class MyThread
{
	private:
		void *(*start_routine)(void *);
	
	public:
		MyThread(int id, ThreadSafeListenerQueue<MyThread*> *addr, container <std::string, std::string> *stores, container<std::string, std::string> *hstores, void *(*start)(void *), int n_s);
		pthread_t the_thread;
		pthread_cond_t idle_cv;
		pthread_mutex_t idle_mutex;
		std::vector<double> stats;
		int sockfd;
		int tid;
		int n_stores;
		int inserts;
		int deletes;
		int lookups;
		std::chrono::steady_clock::time_point conn_rec;
		ThreadSafeListenerQueue<MyThread*> *parent; // the parent queue
		container<std::string, std::string> *kvstores;	
		container<std::string, std::string> *khstores;	
		void *work(void *arg);
};

