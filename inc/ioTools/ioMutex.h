/*****************************************************************************\
Copyright (c) 2017 Daniel Newby

Part of the codetools library: https://github.com/DanielANewby/codetools

The following code is licensed under The MIT License (MIT).  You should have
received a copy of the license with the software as 'MIT-LICENSE'.  The test
of the license can be found at https://opensource.org/licenses/MIT . See also
'LICENSE' or 'LICENSE.txt' for more information.

ioMutex.h -- Definition for an alertable mutex for the ioTools library

Intended for use with ioFutures
\*****************************************************************************/
#ifndef IOTOOLS_MUTEX_H
#define IOTOOLS_MUTEX_H
#pragma once

#ifdef _WINDOWS
#include <Windows.h>
#endif // _WINDOWS

#include <utility>

struct ioMutex
{
	ioMutex(bool bOwned = false);
	ioMutex(const ioMutex&);
	ioMutex(ioMutex&& rhs) { swap(*this, rhs); }
	ioMutex& operator= (const ioMutex& rhs) { swap1(ioMutex { rhs }); return *this; }
	ioMutex& operator= (ioMutex&& rhs) { swap(*this, rhs); return *this; }
	~ioMutex();

	bool Lock(bool wait = true);
	void Release();

private:
#ifdef _WINDOWS
	HANDLE m_mutex = NULL;
#endif // _WINDOWS

	void swap1(ioMutex&& rhs) { swap(*this, rhs); }
	friend void swap(ioMutex&, ioMutex&);
};

inline void swap(ioMutex& lhs, ioMutex& rhs)
{
	using std::swap;
	swap(lhs.m_mutex, rhs.m_mutex);
}

#endif // IOTOOLS_MUTEX_H
