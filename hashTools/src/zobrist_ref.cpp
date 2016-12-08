/*****************************************************************************\
Zobrist hash reference implementation.

Part of the codetools library: https://github.com/DanielANewby/codetools

The following hash algorithm is derived from source provided by Bob Jenkins, at
http://burtleburtle.net/bob/hash/doobs.html

According to Mr. Jenkins this is public domain.

Some slight modifications because C++11.
\*****************************************************************************/

#include "hashTools.h"

BEGIN_HASHTOOLS_NS

EXPORT uint32_t zobrist_hash_32(char *key, uint32_t len, uint32_t maxlen, uint32_t* tab) noexcept
{
	if (len > maxlen)
		len = maxlen;
	uint32_t hash, i;
	for (hash = len, i = 0; i<len; ++i)
		hash ^= tab[i * 256 + key[i]];
	return hash;
}

END_HASHTOOLS_NS
