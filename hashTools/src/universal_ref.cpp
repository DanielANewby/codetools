/*****************************************************************************\
Universal hashing reference implementation.

Part of the codetools library: https://github.com/DanielANewby/codetools

The following hash algorithm is derived from source provided by Bob Jenkins, at
http://burtleburtle.net/bob/hash/doobs.html

According to Mr. Jenkins this is public domain.

Some slight modifications because C++11.
\*****************************************************************************/

#include "hashTools.h"

BEGIN_HASHTOOLS_NS

EXPORT uint32_t universal_hash_32(const void *key, uint32_t len, uint32_t maxlen, uint32_t* tab) noexcept
{
	uint32_t hash, i;
	if (len > maxlen)
		len = maxlen;
	const char* message = (const char*)key;
	for (hash = len, i = 0; i<(len << 3); i += 8)
	{
		register char k = message[i >> 3];
		if (k & 0x01) hash ^= tab[i + 0];
		if (k & 0x02) hash ^= tab[i + 1];
		if (k & 0x04) hash ^= tab[i + 2];
		if (k & 0x08) hash ^= tab[i + 3];
		if (k & 0x10) hash ^= tab[i + 4];
		if (k & 0x20) hash ^= tab[i + 5];
		if (k & 0x40) hash ^= tab[i + 6];
		if (k & 0x80) hash ^= tab[i + 7];
	}
	return hash;
}

END_HASHTOOLS_NS
