/*****************************************************************************\
Copyright (c) 2016 Daniel Newby

Part of the codetools library: https://github.com/DanielANewby/codetools

The following code is licensed under The MIT License (MIT).  You should have
received a copy of the license with the software as 'MIT-LICENSE'.  The test
of the license can be found at https://opensource.org/licenses/MIT . See also
'LICENSE' or 'LICENSE.txt' for more information.

libnew.cpp -- Implements the libnew / ctMemory library

ctMemory provides allocator redirection and rudimentary memory debugging
facilities to all dependencies, allowing an application to redefine how memory
is allocated and released by dependent libraries.
\*****************************************************************************/
#include "libnew.h"

#include <Windows.h>
#undef max

#include <map>

LIB_ALLOC_FUNC libAlloc = malloc;
LIB_FREE_FUNC  libFree = free;

template <class T>
struct no_track_allocator {
	typedef T value_type;
	no_track_allocator() {}
	template <class U> no_track_allocator(const no_track_allocator<U>&) {}
	T* allocate(std::size_t n) {
		return (T*)libAlloc(sizeof(T) * n);
	}
	void deallocate(T* p, std::size_t) { libFree(p); }
};
template <class T, class U>
bool operator==(const no_track_allocator<T>&, const no_track_allocator<U>&) { return true; }
template <class T, class U>
bool operator!=(const no_track_allocator<T>&, const no_track_allocator<U>&) { return false; }

struct AllocInfoInt
{
	size_t size;
	LIB_ALLOC_FUNC allocator;
	LIB_FREE_FUNC deallocator;

	AllocInfoInt(size_t s = 0) :size(s), allocator(libAlloc), deallocator(libFree) {};
};

typedef void* AllocMapKey;
typedef no_track_allocator<std::pair<AllocMapKey, AllocInfoInt>> AllocMapAllocator;
typedef std::map<AllocMapKey, AllocInfoInt, std::less<void*>, AllocMapAllocator> AllocMap;

namespace {
	AllocMap* pAllocMap = 0;
	DECL_NAME(MemoryStats) s_statistics;

	struct lnMutex
	{
		lnMutex() { m_mutex = CreateMutex(NULL, FALSE, NULL); }

		void acquire() { WaitForSingleObject(m_mutex, INFINITE); }
		void release() { ReleaseMutex(m_mutex); }

	private:
		HANDLE m_mutex;
	} libnewMutex;

	struct libnewMutexGuard {
		libnewMutexGuard() { libnewMutex.acquire(); }
		~libnewMutexGuard() { libnewMutex.release(); }
	};

	template <class T> T max(T a, T b) { return (a > b) ? a : b; }
}

#define THREAD_GUARD libnewMutexGuard _guard

DECL_EXPORT_C(void, BeginTrackAllocs)()
{
	THREAD_GUARD;
	if (!pAllocMap)
	{
		pAllocMap = new AllocMap;
	}
}

DECL_EXPORT_C(void, EndTrackAllocs)()
{
	AllocMap* p;
	{
		THREAD_GUARD;
		p = pAllocMap;
		pAllocMap = 0;
	}
	delete p;
}

DECL_EXPORT_C(size_t, GetAllocationInfo)(DECL_NAME(AllocInfo)* pBuffer, size_t count)
{
	unsigned n = 0;
	THREAD_GUARD;
	if (pAllocMap)
	{
		if (!pBuffer)
			return pAllocMap->size();
		AllocMap::iterator iter = pAllocMap->begin(), e = pAllocMap->end();
		for (n = 0; n < count && iter != e; ++n, ++iter)
		{
			AllocInfoInt& aii = iter->second;;
			pBuffer[n].address = iter->first;
			pBuffer[n].size = aii.size;
			pBuffer[n].allocator = aii.allocator;
			pBuffer[n].deallocator = aii.deallocator;
		}
	}
	return n;
}

DECL_EXPORT_C(void, GetMemoryStatistics) (DECL_NAME(MemoryStats)* pStats)
{
	if (pStats)
	{
		THREAD_GUARD;
		pStats->allocated = s_statistics.allocated;
		pStats->total_allocations = s_statistics.total_allocations;
		pStats->total_deallocations = s_statistics.total_deallocations;
		pStats->high_water_mark = s_statistics.high_water_mark;
	}
}

DECL_EXPORT_C(void, SetAllocator)(LIB_ALLOC_FUNC palloc, LIB_FREE_FUNC pfree)
{
	if (!palloc) palloc = malloc;
	if (!pfree) pfree = free;
	THREAD_GUARD;
	libAlloc = palloc;
	libFree = pfree;
}

DECL_EXPORT_C(void*, Alloc)(size_t size)
{
	void* result = libAlloc(size);
	if (result)
	{
		THREAD_GUARD;
		if (pAllocMap)
			(*pAllocMap)[result] = AllocInfoInt(size);
		s_statistics.allocated += size;
		s_statistics.total_allocations += size;
		s_statistics.high_water_mark = max(s_statistics.allocated, s_statistics.high_water_mark);
	}
	return result;
}

DECL_EXPORT_C(void, Dealloc)(void* vp)
{
	LIB_FREE_FUNC freeFunc = libFree;
	{
		THREAD_GUARD;
		if (pAllocMap)
		{
			AllocMap::iterator iter = pAllocMap->find(vp);
			if (iter != pAllocMap->end())
			{
				size_t size = iter->second.size;
				freeFunc = iter->second.deallocator;
				s_statistics.allocated -= size;
				s_statistics.total_deallocations += size;
				pAllocMap->erase(iter);
			}
		}
	}
	freeFunc(vp);
}

namespace LIBNEWNAMESPACE
{
	DECL_EXPORT_CPP(void, BeginTrackAllocs)() { DECL_NAME(BeginTrackAllocs)(); }
	DECL_EXPORT_CPP(void, EndTrackAllocs)() { DECL_NAME(BeginTrackAllocs)(); }
	DECL_EXPORT_CPP(size_t, GetAllocationInfo)(DECL_NAME(AllocInfo)* pBuffer, size_t count)
	{
		return DECL_NAME(GetAllocationInfo)(pBuffer, count);
	}
	DECL_EXPORT_CPP(void, GetMemoryStatistics)(DECL_NAME(MemoryStats)* pStats)
	{
		return DECL_NAME(GetMemoryStatistics)(pStats);
	}
	DECL_EXPORT_CPP(void, SetAllocator)(LIB_ALLOC_FUNC alloc, LIB_FREE_FUNC free)
	{
		DECL_NAME(SetAllocator)(alloc, free);
	}
	DECL_EXPORT_CPP(void*, Alloc)(size_t size) { return DECL_NAME(Alloc)(size); }
	DECL_EXPORT_CPP(void, Dealloc)(void* vp) { DECL_NAME(Dealloc)(vp); }
}

void* operator new(size_t size) { return LIBNEWNAMESPACE::Alloc(size); }
void* operator new[](size_t size) { return LIBNEWNAMESPACE::Alloc(size); }
void operator delete(void* vp) { LIBNEWNAMESPACE::Dealloc(vp); }
void operator delete[](void* vp) { LIBNEWNAMESPACE::Dealloc(vp); }
