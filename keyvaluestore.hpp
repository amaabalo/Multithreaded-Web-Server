// keyvaluestore.hpp

#include <unordered_map>
#include <pthread.h>
#include <vector>

template <typename K, typename V>
class ThreadSafeKVStore
{
	private:
		pthread_rwlock_t KVMutex;
		std::unordered_map<K, V> KVMap;
	public:
		ThreadSafeKVStore();
		int insert(const K key, const V value);
		int accumulate(const K key, const V value);
		int lookup(const K key, V& value);
		int remove(const K key);
		typename std::unordered_map<K, V>::iterator begin();
		typename std::unordered_map<K, V>::iterator end();
		void printout();
};

template <typename K, typename V>
using container = typename std::vector<ThreadSafeKVStore<K, V> *>;

#include "keyvaluestore.cc" 
