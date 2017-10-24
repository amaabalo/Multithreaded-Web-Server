// main.cc
// Best viewed with tabstop=4

#include <iostream>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <random>
#include <string>
#include <cstdint>
#include <vector>
#include <chrono>
#include <functional>
#include "threadpool.hpp"
#include "httpresp.hpp"
#include "httpreq.hpp"
#include "md5.h"
using namespace std;

void *funk(void *dummy);
void respond(const int sockfd, const unsigned int code, const std::string& body, const bool keep_alive);

int main(int argc, char *argv[])
{
	int no_of_threads;
	int port_no;
	int no_of_locks;
	if (argc != 7 || strcmp(argv[1], "-n") || strcmp(argv[3], "-p") || strcmp(argv[5], "-l"))
	{
		std::cout << "Usage: ./lab2 -n <N> -p <P> -l<L>\n"; 
		return -1;
	}
	else
	{
		no_of_threads = atoi(argv[2]);
		port_no = atoi(argv[4]);	
		no_of_locks = atoi(argv[6]);
	}

	ThreadPoolServer t(no_of_threads, &funk, no_of_locks);
	t.listen_and_assign(port_no);
	
	return 0;
}

/* Worker threads handle each session */
void *funk(void *arg)
{
	MyThread *me = (MyThread *)arg;
	container<std::string, std::string> kvstores = *me->kvstores;
	container<std::string, std::string> khstores = *me->khstores;
	chrono::steady_clock::time_point start;
	chrono::steady_clock::time_point end;
	while (1)
    {
        pthread_mutex_lock(&(me->idle_mutex));
        while (me->sockfd < 0)
		{
            pthread_cond_wait(&(me->idle_cv), &(me->idle_mutex));
		}
		
		bool verdict = true;		
		bool first = true;
		do
		{
			if (first)
			{
				start = me->conn_rec;
				first = false;
			}
			else
				start = chrono::steady_clock::now();
        	HTTPReq request(me->sockfd);    
        	if (request.parse() != 0 || request.isMalformed())
        	{   
				respond(me->sockfd, 400, "", false);	// session ends on bad request or no more requests
            	break;
        	}

        	std::string method = request.getMethod();
        	std::string key = request.getURI();
			verdict = request.getVerdict();
			std::hash<std::string> str_hash;
			long int key_hash = str_hash(key) % me->n_stores;
            std::string temp;
			
			if (method == "GET")
        	{
            	if (kvstores[key_hash]->lookup(key, temp) == 0)
            	{
					respond(me->sockfd, 200, key, verdict);		// OK
            	}
            	else
            	{
					respond(me->sockfd, 404, "", verdict);		// not found
            	}
				me->lookups = me->lookups + 1;
        	}
        	else if (method == "POST")
        	{
				std::string value = request.getBody();
				std::string hash = md5(value);
				if (kvstores[key_hash]->insert(key, value) || khstores[key_hash]->insert(key, hash))		// error if either of the removes fail
					respond(me->sockfd, 500, "", verdict);
				else
				{
					respond(me->sockfd, 200, "", verdict);
				}
				me->inserts = me->inserts + 1;
        	}
        	else if (method == "DELETE")
        	{
				bool kv = kvstores[key_hash]->lookup(key, temp) == 0;	// kv is true if the key was found
				bool kh = khstores[key_hash]->lookup(key, temp) == 0;
				if (kv && kh)
				{
            		if (kvstores[key_hash]->remove(key) || khstores[key_hash]->remove(key))	// error if either of the removes fail
						respond(me->sockfd, 500, "", verdict);
					else
					{
						respond(me->sockfd, 200, "", verdict);
					}
				}
				else if (kv != kh)
					respond(me->sockfd, 500, "", verdict);	// value exists <=> hash exists
				else
					respond(me->sockfd, 404, "", verdict);	// not found
				
				me->deletes = me->deletes + 1;
        	}
			else
			{
				respond(me->sockfd, 501, "", verdict);
			}
			end = chrono::steady_clock::now();
			chrono::duration<double> span = chrono::duration_cast<chrono::duration<double>>(end - start);
			me->stats.push_back(span.count());	// store request time
		} while (1);
		close(me->sockfd);		// connection closed once done

		
        me->sockfd = -1; // back to idle
        pthread_mutex_unlock(&(me->idle_mutex));
        me->parent->push(me); // put self back on the queue
		
	}
    return nullptr;
}

/* Wrapper for responding to requests */
void respond(const int sockfd, const unsigned int code, const std::string& body, const bool keep_alive)
{
	//if (code != 200)
		//cout << "There was an error\n";
	HTTPResp response(code, body, keep_alive);
	if (!response.isMalformed())
	{
		std::string r = response.getResponse();
		write(sockfd, r.c_str(), r.length());
	}
	else
	{
		HTTPResp response(500, "", keep_alive);
		std::string r = response.getResponse();
		write(sockfd, r.c_str(), r.length());
	} 
}
