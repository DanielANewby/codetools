/*****************************************************************************\
Copyright (c) 2016 Daniel Newby

Part of the codetools library: https://github.com/DanielANewby/codetools

The following code is licensed under The MIT License (MIT).  You should have
received a copy of the license with the software as 'MIT-LICENSE'.  The test
of the license can be found at https://opensource.org/licenses/MIT . See also
'LICENSE' or 'LICENSE.txt' for more information.

ioObject.h -- Definition for io base class
\*****************************************************************************/
#ifndef IOTOOLS_IOOBJECT_H
#define IOTOOLS_IOOBJECT_H
#pragma once

#include <cstdint>
#include <future>

#ifdef EXPORT
#undef EXPORT
#endif // EXPORT

#ifdef IOTOOLS_EXPORTS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif // IOTOOLS_EXPORTS

struct ioResult
{
	unsigned error;
	unsigned warning;
	uint64_t length;
	const char* data;
};

typedef std::promise<ioResult> ioPromise;
typedef std::future<ioResult> ioFuture;

class EXPORT ioObject
{
protected:
	ioObject() {};

public:
	ioObject(const ioObject&) = delete;
	ioObject(ioObject&&) = delete;
	ioObject& operator=(const ioObject&) = delete;
	ioObject& operator=(ioObject&&) = delete;
	virtual ~ioObject() = default;

	virtual operator bool() = 0;

	virtual uint64_t Read(char* out, uint64_t amount, uint64_t offset = 0) = 0;
	virtual uint64_t Write(const char* data, uint64_t amount, uint64_t offset = 0) = 0;
	virtual ioFuture ReadAsync(char* out, uint64_t amount, uint64_t offset = 0) = 0;
	virtual ioFuture WriteAsync(const char* data, uint64_t amount, uint64_t offset = 0) = 0;
};

#endif // IOTOOLS_IOOBJECT_H
