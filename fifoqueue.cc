// fifoqueue.cpp

#include <iostream>

template <typename T>
ThreadSafeListenerQueue<T>::ThreadSafeListenerQueue()
{
	if (pthread_mutex_init(&FQMutex, NULL))
		std::cout << "Error: Mutex initialization failed.\n";
	if (pthread_cond_init(&FQConditionVariable, NULL))
		std::cout << "Error: Mutex initialization failed.\n";
}

/*	Pushes the element to the end of the list,
 *	so that it will be the last of the items
 *	currently on the queue to be removed.
 *	returns 0 on success, or -1 if some fatal
 *	error occurs.
 */
template <typename T>
int ThreadSafeListenerQueue<T>::push(const T element)
{
	if (pthread_mutex_lock(&FQMutex))
		return -1;
	try
	{
		lst.push_back(element);
	}
	catch(...)
	{
		pthread_mutex_unlock(&FQMutex);
		return -1;
	}
	if (pthread_cond_signal(&FQConditionVariable))
		return -1;
	if (pthread_mutex_unlock(&FQMutex))
		return -1;
	return 0;
}

/*	Pops the least recently inserted element from
 *	the queue and fills in the passed-by-reference
 *	variable element with its contents. Returns 0
 *	if this is successful, 1 if the queue is
 *	empty, or -1 if some fatal error occured.
 */
template <typename T>
int ThreadSafeListenerQueue<T>::pop(T& element)
{
	if (pthread_mutex_lock(&FQMutex))
		return -1;
	if (!lst.empty())
	{
		element = *lst.begin();
		lst.pop_front();
		if (pthread_mutex_unlock(&FQMutex))
			return -1;
		return 0;
	}
	else
	{
		if (pthread_mutex_unlock(&FQMutex))
			return -1;
		return 1;
	}
}

/*	Blocks until there is an element to be popped.
 *	Returns 0 if an element is returned, or -1 if
 *	some fatal error occurs.
 */
template <typename T>
int ThreadSafeListenerQueue<T>::listen(T& element)
{
	if (pthread_mutex_lock(&FQMutex))
		return -1;
	while(lst.empty())
		if (pthread_cond_wait(&FQConditionVariable, &FQMutex))
			return -1;
	element = *lst.begin();
	lst.pop_front();
	if (pthread_mutex_unlock(&FQMutex))
		return -1;
	return 0;
}

/*	Prints out all keys in the queue. */
template <typename T>
void ThreadSafeListenerQueue<T>::printout()
{
	for (auto it = lst.begin(); it != lst.end(); it++)
	{
		std::cout << "Key: " << *it << "\n";
	}
}

/* Return an iterator to the beginning of the list. */
template <typename T>
typename std::list<T>::iterator ThreadSafeListenerQueue<T>::begin()
{
	return lst.begin();
}

/* Return an iterator to the end of the list. */
template <typename T>
typename std::list<T>::iterator ThreadSafeListenerQueue<T>::end()
{
	return lst.end();
}
