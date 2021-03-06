/*****************************************************************************\
An additive hash reference implementation.

Part of the codetools library: https://github.com/DanielANewby/codetools

The following hash algorithm is derived from source provided by Bob Jenkins, at
http://burtleburtle.net/bob/hash/doobs.html

According to Mr. Jenkins this is public domain.

Some slight modifications because C++11.
\*****************************************************************************/

#include "hashTools.h"

#include <Windows.h>

BEGIN_HASHTOOLS_NS

EXPORT uint32_t additive_hash_32(const uint8_t* key, uint32_t len, uint32_t prime) noexcept
{
	uint32_t hash, i;
	for (hash = len, i = 0; i<len; ++i)
		hash += key[i];
	return (hash % prime);
}

END_HASHTOOLS_NS
