/*****************************************************************************\
Bob Jenkin's "one at a time" reference implementation.

Part of the codetools library: https://github.com/DanielANewby/codetools

The following hash algorithm is derived from source provided by Bob Jenkins, at
http://burtleburtle.net/bob/hash/doobs.html

According to Mr. Jenkins this is public domain.

Some slight modifications because C++11.
\*****************************************************************************/

#include "hashTools.h"

BEGIN_HASHTOOLS_NS

EXPORT uint32_t jenkins_oaat_32(const uint8_t* key, uint32_t len, uint32_t mask) noexcept
{
	uint32_t hash, i;
	for (hash = 0, i = 0; i<len; ++i)
	{
		hash += key[i];
		hash += (hash << 10);
		hash ^= (hash >> 6);
	}
	hash += (hash << 3);
	hash ^= (hash >> 11);
	hash += (hash << 15);
	return (hash & mask);
}

END_HASHTOOLS_NS
