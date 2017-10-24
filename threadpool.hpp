// threadpool.hpp
// best viewed with tabstop=4

#include "mythread.hpp"

class ThreadPoolServer
{
	private:
		int n;
		int n_stores;
		pthread_mutex_t *stats_mutex;
		std::vector<double> *stats;
		ThreadSafeListenerQueue<MyThread *> idle_queue;
		/*vector<ThreadSafeKVStore<std::string, std::string> *>*/container<std::string, std::string> KVStores;
		/*vector<ThreadSafeKVStore<std::string, std::string> *>*/ container<std::string, std::string> KHStores;
		void *(*start_routine)(void *);	
	public:	
		ThreadPoolServer(int number_of_threads, void *(start)(void *), int number_of_stores);
		void listen_and_assign(int port);
};

