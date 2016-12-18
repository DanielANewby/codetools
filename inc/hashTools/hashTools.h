#ifndef CODETOOLS_HASHTOOLS_H
#define CODETOOLS_HASHTOOLS_H
#pragma once

#include <cstdint>

#ifdef EXPORT
#undef EXPORT
#endif // EXPORT

#ifdef HASHTOOLS_EXPORTS
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __declspec(dllimport)
#endif // HASHTOOLS_EXPORTS

#define BEGIN_HASHTOOLS_NS namespace codetools { namespace hashtools {
#define END_HASHTOOLS_NS }}

BEGIN_HASHTOOLS_NS

	//////////////////////////////////////////////////////////////////////////////
	// Miscellaneous hashes
	//////////////////////////////////////////////////////////////////////////////
	EXPORT uint32_t additive_hash_32(const uint8_t* key, uint32_t len, uint32_t prime) noexcept;
	EXPORT uint32_t rotating_hash_32(const uint8_t* key, uint32_t len, uint32_t prime) noexcept;
	// table is a pointer to uint32_t[maxlen * 8]
	EXPORT uint32_t universal_hash_32(const void *key, uint32_t len, uint32_t maxlen, uint32_t* table) noexcept;
	// table is a naked pointer to uint32_t[maxlen][256]
	EXPORT uint32_t zobrist_hash_32(char *key, uint32_t len, uint32_t maxlen, uint32_t* table) noexcept;

	//////////////////////////////////////////////////////////////////////////////
	// Hseih hash
	//////////////////////////////////////////////////////////////////////////////
	EXPORT uint32_t hseih_32(const void* message, size_t size, uint32_t initialVal) noexcept;

	//////////////////////////////////////////////////////////////////////////////
	// FNV hashes
	//////////////////////////////////////////////////////////////////////////////
	EXPORT uint32_t fnv1a_32(const void* key, size_t length, uint32_t initialValue) noexcept;
	EXPORT uint64_t fnv1a_64(const void* key, size_t length, uint64_t initialValue) noexcept;

	//////////////////////////////////////////////////////////////////////////////
	// Jenkins hashes
	//////////////////////////////////////////////////////////////////////////////
	EXPORT uint32_t jenkins_oaat_32(const uint8_t* key, uint32_t len, uint32_t mask = 0xFFFFFFFF) noexcept;
	
	// Lookup 2
	EXPORT uint32_t jenkins_lookup2_32(const uint8_t* key, uint32_t len, uint32_t initialValue = 0) noexcept;
	EXPORT uint32_t jenkins_lookup2_32(const uint32_t* key, uint32_t len, uint32_t initialValue = 0) noexcept;
	template <class T> uint32_t jenkins_lookup2_32(const T* key, uint32_t len, uint32_t initVal = 0) noexcept
	{
		if (len & 0x3)
			return jenkins_lookup2_32((const uint8_t*)key, len, initVal);
		return jenkins_lookup2_32((const uint32_t*)key, len / 4, initVal);
	}

	// Lookup 3
	EXPORT uint32_t jenkins_lookup3_32(const void* key, size_t length, uint32_t initialValue = 0) noexcept;
	EXPORT uint64_t jenkins_lookup3_64(const void* key, size_t length, uint64_t initialValue = 0) noexcept;

	constexpr uint32_t jenkins_hashsize(const uint32_t bits) noexcept { return 1 << bits; }
	constexpr uint32_t jenkins_hashmask(const uint32_t bits) noexcept { return jenkins_hashsize(bits) - 1; }
	constexpr uint64_t jenkins_hashsize_64(const uint64_t bits) noexcept { return 1ull << bits; }
	constexpr uint64_t jenkins_hashmask_64(const uint64_t bits) noexcept { return jenkins_hashsize_64(bits) - 1; }

	// Spooky hash
	EXPORT uint32_t spooky_32(const void* message, size_t length, uint32_t seed = 0) noexcept;
	EXPORT uint64_t spooky_64(const void* message, size_t length, uint64_t seed = 0) noexcept;
	EXPORT uint64_t spooky_128(const void* message, size_t length, uint64_t seed = 0, uint64_t* pSeedHigh = 0) noexcept;

	//////////////////////////////////////////////////////////////////////////////
	// Cryptographic hashes
	//////////////////////////////////////////////////////////////////////////////
	EXPORT void md5_ref(const void* message, size_t length, unsigned char* digest) noexcept;
	EXPORT uint32_t sha1_ref(const void* message, size_t length) noexcept;

END_HASHTOOLS_NS

#endif // CODETOOLS_HASHTOOLS_H
