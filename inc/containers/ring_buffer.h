#ifndef CODETOOLS_RING_BUFFER_H
#define CODETOOLS_RING_BUFFER_H
#pragma once

#include <cstdint>
#include <initializer_list>

namespace codetools 
{
	template <class T>
	class ring_buffer
	{
	public:
		typedef T value_type;
		typedef T& reference;
		typedef const T& const_reference;
		typedef T* pointer;
		typedef const T* const_pointer;

		using size_t = std::size_t;

		ring_buffer();
		ring_buffer(size_t size, const T& initVal = T{});
		ring_buffer(std::initializer_list<T>);
		ring_buffer(size_t capacity, std::initializer_list<T>);
		~ring_buffer();

		size_t capacity();
		size_t size();
		void reserve(size_t requested);
		void resize(size_t requested);
		void shrink();
	
		reference front();
		const_reference front() const;
		reference back();
		const_reference back() const;
		reference at();
		const_reference at() const;
	private:
		size_t m_size;
		size_t m_capacity;

		T* m_buffer;
	};
}

#endif // CODETOOLS_RING_BUFFER_H
