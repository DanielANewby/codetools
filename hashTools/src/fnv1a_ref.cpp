/*****************************************************************************\
FNV-1a hash reference implementation.

Part of the codetools library: https://github.com/DanielANewby/codetools

Derived from http://www.isthe.com/chongo/tech/comp/fnv/

FNV is in the public domain, see
http://www.isthe.com/chongo/tech/comp/fnv/#public_domain

Some slight modifications because C++11.
\*****************************************************************************/

#include "hashTools.h"

BEGIN_HASHTOOLS_NS

namespace 
{
	constexpr uint32_t FNV_32_PRIME = 0x01000193;
	constexpr uint64_t FNV_64_PRIME = 0x100000001b3ULL;
}

EXPORT uint32_t fnv1a_32(const void* key, size_t length, uint32_t initialValue) noexcept
{
	const unsigned char *bp = (const unsigned char *)key;
	const unsigned char *be = bp + length;
	while (bp < be) 
	{
		initialValue ^= (uint32_t)*bp++;
		initialValue *= FNV_32_PRIME;
	}
	return initialValue;
}

EXPORT uint64_t fnv1a_64(const void* key, size_t length, uint64_t initialValue) noexcept
{
	const unsigned char *bp = (const unsigned char *)key;
	const unsigned char *be = bp + length;
	while (bp < be)
	{
		initialValue ^= (uint64_t)*bp++;
		initialValue *= FNV_64_PRIME;
	}
	return initialValue;
}

END_HASHTOOLS_NS
