// threadpool.cc
// with help from http://www.linuxhowtos.org/C_C++/socket.htm
// best viewed with tabstop=4

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <string.h>
#include <list>
#include <poll.h>
#include <algorithm>
#include "threadpool.hpp"

ThreadPoolServer::ThreadPoolServer(int number_of_threads, void *(*start)(void *), int number_of_stores)
{
	n = number_of_threads;
	n_stores = number_of_stores;
	start_routine = start;
	
	int i;
	for (i = 0; i < n_stores; i++)
	{
		KVStores.push_back(new ThreadSafeKVStore<std::string, std::string>);
		KHStores.push_back(new ThreadSafeKVStore<std::string, std::string>);
	}

	for (i = 0; i < n; i++)
	{
		MyThread *addr = new MyThread(i, &idle_queue, &KVStores, &KHStores, start_routine, n_stores);
		idle_queue.push(addr);		
	}
}

void ThreadPoolServer::listen_and_assign(int port)
{
	int sockfd, newsockfd, client_addr_len;
	struct sockaddr_in server_addr, client_addr;

	// create a new socket
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		std::cout << "Error creating socket";

	// initialize the server address
	memset((char *) &server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	server_addr.sin_addr.s_addr = INADDR_ANY;

	// bind socket to the server address
	if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
		std::cout << "Error binding socket to server address";

	// allow the process to listen on the socket for connections
	listen(sockfd, 5);
	
	std::cout << "\nListening for requests...\n\n";	
	
	struct pollfd fds[1];
	fds[0].fd = sockfd;
	fds[0].events = POLLIN;	

	// repeatedly handle connections
	while (1)
	{
		client_addr_len = sizeof(client_addr);
		
		if (poll(fds, 1, 3000) != 0)
		{
			newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, (socklen_t *)&client_addr_len);
			if (newsockfd < 0)
				std::cout << "Error on accept";
		
			// put an idle thread to work
			MyThread *thrd;
			idle_queue.listen(thrd);
			thrd->conn_rec = std::chrono::steady_clock::now();
			pthread_mutex_lock(&(thrd->idle_mutex));
			thrd->sockfd = newsockfd;
			pthread_cond_signal(&(thrd->idle_cv));
			pthread_mutex_unlock(&(thrd->idle_mutex));
		}
		else
			break;	// 15-second time limit reached
	}
	
	std::cout << "No more incoming requests. Server terminated.\n\n";
	std::cout << "Compiling stats...\n\n";
	
	std::list<double> all_times;	
	double all_inserts = 0;
	double all_lookups = 0;
	double all_deletes = 0;
	double sum_of_times = 0;
	MyThread *t;	
	for (int i = 0; i < n; i++)
	{
		idle_queue.listen(t);
		int j;
		for (j = 0; j < t->stats.size(); j++)
		{
			all_times.push_back(t->stats[j]);
			sum_of_times += t->stats[j];
		}
		
		all_inserts += t->inserts;
		all_lookups += t->lookups;
		all_deletes += t->deletes;
	}

	all_times.sort();	
	
	/*for (auto it = all_times.begin(); it !=all_times.end(); it++)
	{
		std::cout << *it << " ";
	}
	std::cout << "\n";*/
	std::cout << "Number of Threads: " << n << "\n";
	std::cout << "Total number of requests: " << all_times.size() << "\n";
	std::cout << "Number of separate stores: " << n_stores << "\n";
	std::cout << "#Inserts: " << all_inserts << "\n";
	std::cout << "#Deletes: " << all_deletes << "\n";
	std::cout << "#Lookups: " << all_lookups << "\n";
	std::cout << "Total request time: " << sum_of_times << " seconds\n";
	std::list<double>::iterator r = all_times.begin();
	double minimum = *r;
	std::cout << "Minimum request time: " << *r << " s\n";

	std::cout << "Maximum request time: " << all_times.back() << " s\n";

	std::advance(r, all_times.size()/2);
	std::cout << "Median request time: " << *r << " s\n";	

	std::cout << "Average time per request: " << sum_of_times / (all_inserts + all_lookups + all_deletes) << " seconds per request\n\n";
}
