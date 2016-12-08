/*****************************************************************************\
Copyright (c) 2016 Daniel Newby

Part of the codetools library: https://github.com/DanielANewby/codetools

The following code is licensed under The MIT License (MIT).  You should have
received a copy of the license with the software as 'MIT-LICENSE'.  The test
of the license can be found at https://opensource.org/licenses/MIT . See also
'LICENSE' or 'LICENSE.txt' for more information.

libnew.h -- Defines the interface for the libnew / ctMemory library

ctMemory provides allocator redirection and rudimentary memory debugging
facilities to all dependencies, allowing an application to redefine how memory
is allocated and released by dependent libraries.
\*****************************************************************************/
#pragma once

#ifdef EXPORT
#undef EXPORT
#endif // EXPORT
#ifdef LIBNEWWART
#undef LIBNEWWART
#endif // LIBNEWWART
#ifdef LIBNEWNAMESPACE
#undef LIBNEWNAMESPACE
#endif // LIBNEWNAMESPACE

// CONFIGURE THIS CORRECTLY FOR YOUR PROJECT
// Replace LIBNEW_EXPORTS with the appropriate macro from your build
// Define LIBNEWWART with the [prepended] wart for the C function interface
// Define LIBNEWNAMESPACE with the C++ namespace for your library
#ifdef CTMEMORY_EXPORTS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif

#define LIBNEWWART cdt
#define LIBNEWNAMESPACE codetools

#define DECL_NAME_HELPER1(x) x
#define DECL_NAME_HELPER2(x, y) x ## y
#define DECL_NAME(x) DECL_NAME_HELPER1(LIBNEWWART) ## x
#define DECL_EXPORT_C(ret, name) extern "C" EXPORT ret __cdecl DECL_NAME(name)
#define DECL_EXPORT_CPP(ret, name) EXPORT ret __cdecl name

#define LIB_ALLOC_FUNC DECL_NAME(LibAllocFunc)
#define LIB_FREE_FUNC  DECL_NAME(LibFreeFunc)

typedef void* (__cdecl *LIB_ALLOC_FUNC)(size_t size);
typedef void(__cdecl *LIB_FREE_FUNC)(void*);

struct DECL_NAME(AllocInfo)
{
	void* address;
	size_t size;
	DECL_NAME(LibAllocFunc) allocator;
	DECL_NAME(LibFreeFunc) deallocator;
};

struct DECL_NAME(MemoryStats)
{
	size_t allocated;
	size_t total_allocations;
	size_t total_deallocations;
	size_t high_water_mark;
};

DECL_EXPORT_C(void,   BeginTrackAllocs)    ();
DECL_EXPORT_C(void,   EndTrackAllocs)      ();
DECL_EXPORT_C(size_t, GetAllocationInfo)   (DECL_NAME(AllocInfo)* pBuffer, size_t count);
DECL_EXPORT_C(void,   GetMemoryStatistics) (DECL_NAME(MemoryStats)* pStats);
DECL_EXPORT_C(void,   SetAllocator)        (LIB_ALLOC_FUNC alloc, LIB_FREE_FUNC free);
DECL_EXPORT_C(void*,  Alloc)               (size_t size);
DECL_EXPORT_C(void,   Dealloc)             (void* vp);

#ifdef __cplusplus
namespace LIBNEWNAMESPACE
{
	typedef DECL_NAME(AllocInfo) AllocInfo;
	typedef DECL_NAME(MemoryStats) MemoryStats;

	DECL_EXPORT_CPP(void,   BeginTrackAllocs)    ();
	DECL_EXPORT_CPP(void,   EndTrackAllocs)      ();
	DECL_EXPORT_CPP(size_t, GetAllocationInfo)   (DECL_NAME(AllocInfo)* pBuffer, size_t count);
	DECL_EXPORT_CPP(void,   GetMemoryStatistics) (DECL_NAME(MemoryStats)*);
	DECL_EXPORT_CPP(void,   SetAllocator)        (LIB_ALLOC_FUNC alloc, LIB_FREE_FUNC free);
	DECL_EXPORT_CPP(void,   SetAllocator)        (LIB_ALLOC_FUNC, LIB_FREE_FUNC);
	DECL_EXPORT_CPP(void*,  Alloc)               (size_t size);
	DECL_EXPORT_CPP(void,   Dealloc)             (void* vp);

}
#endif // __cplusplus
