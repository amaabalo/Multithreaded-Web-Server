// keyvaluestore.cpp

#include <iostream>
#include <string>

template <typename K, typename V>
ThreadSafeKVStore<K, V>::ThreadSafeKVStore()
{
	if (pthread_rwlock_init(&KVMutex, NULL))
		std::cout << "Error: Mutex initialization failed.\n";
}

/*	Inserts a key-value pair if it doesn't exist,
 *	or updates it if it does. Return 0 on
 *	sucess, or -1 if some fatal error occurs. 
 */
template <typename K, typename V>
int ThreadSafeKVStore<K, V>::insert(const K key, const V value)
{
	if (pthread_rwlock_wrlock(&KVMutex))
		return -1;
	try
	{
		KVMap[key] = value;
	}
	catch(...)
	{
		pthread_rwlock_unlock(&KVMutex);
		return -1;
	}
	if (pthread_rwlock_unlock(&KVMutex))
		return -1;
	return 0;
}

/*	Inserts a key-value pair if it doesn't exist
 *	but adds the new value to the existing value
 *	if it does. Return 0 on success, or -1 if 
 *	some fatal error occurs.
 */
template <typename K, typename V>
int ThreadSafeKVStore<K, V>::accumulate(const K key, const V value)
{
	if (pthread_rwlock_wrlock(&KVMutex))
		return -1;
	if ( KVMap.count(key) )
	{
		KVMap[key] = KVMap[key] + value;
	}
	else
	{
		try
		{
			KVMap[key] = value;
		}
		catch(...)
		{
			pthread_rwlock_unlock(&KVMutex);
			return -1;
		}
	}
	if (pthread_rwlock_unlock(&KVMutex))
		return -1;
	return 0;					
}


/*	Return 0 if the key is present, -1 otherwise.
 *	if the key is present, fill the value
 *	variable (passed by reference) with the 
 *	associated value.
 */
template <typename K, typename V>
int ThreadSafeKVStore<K, V>::lookup(const K key, V& value)
{
	if (pthread_rwlock_rdlock(&KVMutex))
		return -1;
	if ( KVMap.count(key))
	{
		value = KVMap[key];
		if (pthread_rwlock_unlock(&KVMutex))
			return -1;
		return 0;
	}
	pthread_rwlock_unlock(&KVMutex);
	return -1;
}

/*	Delete the key-value pair whose key is key
 *	from the hashmap, if it exists. Return 0 on
 *	success, -1 if there is some fatal error.
 */
template <typename K, typename V>
int ThreadSafeKVStore<K, V>::remove(const K key)
{
	if (pthread_rwlock_wrlock(&KVMutex))
		return -1;
	if( KVMap.count(key))
		KVMap.erase(key);
	if (pthread_rwlock_unlock(&KVMutex))
		return -1;	
	return 0;
}

/*	Return an iterator to the start of the 
 * 	KVMap.
 */
template <typename K, typename V>
typename std::unordered_map<K, V>::iterator ThreadSafeKVStore<K, V>::begin()
{
	return KVMap.begin();	
}

/*	Return an iterator to the end of the 
 *	KVap.
 */
template <typename K, typename V>
typename std::unordered_map<K, V>::iterator ThreadSafeKVStore<K, V>::end()
{
	return KVMap.end();	
}

/*	Prints out all key-value pairs.
 */
template <typename K, typename V>
void ThreadSafeKVStore<K, V>::printout()
{
	for ( auto it = KVMap.begin(); it != KVMap.end(); it++)
	{
		std::cout << "Key: " << it->first <<	"; Value: " << it->second << "\n";
	}
}
