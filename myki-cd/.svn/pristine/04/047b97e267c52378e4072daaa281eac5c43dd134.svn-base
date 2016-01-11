#ifndef CD_CACHE_H
#define CD_CACHE_H

#include <map>
#include <list>
#include "csmutex.h"
#include "cs.h"

template <typename CDSTRUCT> void CacheMemoryCopy(CDSTRUCT& target, const CDSTRUCT& source)
{
	memcpy(&target, &source, sizeof(CDSTRUCT));
}
template <typename CDSTRUCT> void CacheAssignCopy(CDSTRUCT& target, const CDSTRUCT& source)
{
	target = source;
}

template <typename ID, typename CDSTRUCT> class CdCache
{
private:
	typedef std::list<ID> RecentList;
	struct CacheItem
	{
		typename RecentList::iterator itr;
		bool null;
		CDSTRUCT cd;
	};
	typedef std::map<ID, CacheItem> Cache;
	typedef void (*CopyFunc)(CDSTRUCT& target, const CDSTRUCT& source);
	typedef void (*FreeFunc)(CDSTRUCT& target);
	unsigned int m_maxCacheSize;
	RecentList m_recentList;
	Cache m_cache;
	CsMutex_t m_mutex;
	CopyFunc m_copyFunc;
	FreeFunc m_freeFunc;
	const char* m_debugDesc;
	unsigned int m_cacheHits;
	unsigned int m_cacheMisses;

public:
	CdCache(const char* debugDesc, unsigned int maxCacheSize, CopyFunc copyFunc, FreeFunc freeFunc=0)
	: m_maxCacheSize(maxCacheSize),
	  m_copyFunc(copyFunc),
	  m_freeFunc(freeFunc),
	  m_debugDesc(debugDesc),
	  m_cacheHits(0),
	  m_cacheMisses(0)
	{
		CsMutexInit(&m_mutex, CSSYNC_THREAD);
	}
	~CdCache()
	{
		if ( (m_cacheHits > 0) || (m_cacheMisses > 0) )
		{
			CsVerbose("%s cache hits=%u,misses=%u", m_debugDesc, m_cacheHits, m_cacheMisses);
		}
		clear();
		CsMutexDestroy(&m_mutex);
	}

	bool getCacheValue(ID id, CDSTRUCT& output, bool& isNull)
	{
		bool result = false;
		CsMutexLock(&m_mutex);
		if ( m_cache.find(id) != m_cache.end() )
		{
			CacheItem& cacheItem = m_cache[id];
			isNull = cacheItem.null;
			if ( !isNull )
				m_copyFunc(output, cacheItem.cd);
			m_recentList.erase(cacheItem.itr);
			m_recentList.push_front(id);
			cacheItem.itr = m_recentList.begin();
			++m_cacheHits;
			result = true;
		}
		else
		{
			++m_cacheMisses;
		}
		CsMutexUnlock(&m_mutex);
		return result;
	}

	void addCacheValue(ID id, const CDSTRUCT& input, bool isNull=false)
	{
		CsMutexLock(&m_mutex);
		bool exists = (m_cache.find(id) != m_cache.end());
		CacheItem& cacheItem = m_cache[id];
		m_copyFunc(cacheItem.cd, input);
		if ( exists )
			m_recentList.erase(cacheItem.itr);
		m_recentList.push_front(id);
		cacheItem.itr = m_recentList.begin();
		cacheItem.null = isNull;

		if ( m_cache.size() > m_maxCacheSize )
		{
			if ( m_freeFunc != 0 )
				m_freeFunc(m_cache[m_recentList.back()].cd);
			m_cache.erase(m_recentList.back());
			m_recentList.pop_back();
		}
		CsMutexUnlock(&m_mutex);
	}

	void clear()
	{
		CsMutexLock(&m_mutex);
		if ( m_freeFunc != 0 )
		{
			for ( typename Cache::iterator it = m_cache.begin(); it != m_cache.end(); ++it )
				m_freeFunc(it->second.cd);
		}
		m_cache.clear();
		m_recentList.clear();
		CsMutexUnlock(&m_mutex);
	}
};

#endif /* CD_CACHE_H */
