/*****************************************************************************\
Copyright (c) 2016 Daniel Newby

Part of the codetools library: https://github.com/DanielANewby/codetools

The following code is licensed under The MIT License (MIT).  You should have
received a copy of the license with the software as 'MIT-LICENSE'.  The test
of the license can be found at https://opensource.org/licenses/MIT . See also
'LICENSE' or 'LICENSE.txt' for more information.

ioFileObject.h -- Definition for io file provider
\*****************************************************************************/
#ifndef IOTOOLS_IOFILEOBJECT_H
#define IOTOOLS_IOFILEOBJECT_H
#pragma once

#ifdef _WINDOWS
#include <Windows.h>
#endif // _WINDOWS

#include "ioObject.h"

class EXPORT ioFileObject : public ioObject
{
public:
	ioFileObject();
	ioFileObject(const char* filename);
	ioFileObject(const wchar_t* filename);
	ioFileObject(const ioFileObject& rhs);
	ioFileObject(ioFileObject&& rhs) { swap(rhs); }
	ioFileObject& operator= (const ioFileObject& rhs) { swap(ioFileObject{rhs}); return *this; }
	ioFileObject& operator= (ioFileObject&& rhs) { swap(rhs); return *this; }
	virtual ~ioFileObject() { Flush(); Close(); }

	void swap(ioFileObject& rhs);
	void swap(ioFileObject&& rhs) { swap(rhs); }

	virtual operator bool();

	virtual uint64_t Read(char* out, uint64_t amount, uint64_t offset = 0);
	virtual uint64_t Write(const char* data, uint64_t amount, uint64_t offset = 0);
	virtual ioFuture ReadAsync(char* out, uint64_t amount, uint64_t offset = 0);
	virtual ioFuture WriteAsync(const char* data, uint64_t amount, uint64_t offset = 0);

	virtual void Open(const char* filename);
	virtual void Open(const wchar_t* filename);
	virtual void Flush();
	virtual void Close();

protected:
#ifdef _WINDOWS
	HANDLE m_file = INVALID_HANDLE_VALUE;
#endif
};

inline void swap(ioFileObject& lhs, ioFileObject& rhs) { lhs.swap(rhs); }

#endif // IOTOOLS_IOFILEOBJECT_H
