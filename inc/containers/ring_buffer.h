#ifndef CODETOOLS_RING_BUFFER_H
#define CODETOOLS_RING_BUFFER_H
#pragma once

#include <cstdint>
#include <initializer_list>
#include <iterator>

#include <new>

#include "iterators.h"

namespace codetools 
{
	const size_t default_ring_buffer_size = 10;

	template <class MyContainer>
	struct ring_buffer_iterator_core {
		using container_type = MyContainer;
		using container_pointer = container_type*;
		using reference = typename container_type::reference;
		using difference_type = typename container_type::difference_type;

		using my_type = ring_buffer_iterator_core;
		ring_buffer_iterator_core() noexcept :container{0}, logical_index{k_logical_end} {}
		ring_buffer_iterator_core(container_pointer pc, size_t index) noexcept :
			container{pc}, logical_index{index}
		{
			if (logical_index >= container->size())
				logical_index = k_logical_end;
		}
		ring_buffer_iterator_core(const MyContainer* pc, size_t index) noexcept :
			ring_buffer_iterator_core(const_cast<container_pointer>(pc), index)
		{}
		ring_buffer_iterator_core(const my_type& rhs) noexcept :
			ring_buffer_iterator_core(rhs.container, rhs.logical_index)
		{}
		ring_buffer_iterator_core(my_type&& rhs) = default;
		~ring_buffer_iterator_core() {}

		my_type& operator=(const my_type& rhs) noexcept { 
			container = rhs.container;
			logical_index = rhs.logical_index;
			if (logical_index > container->size())
				logical_index = k_logical_end;
			return *this;
		}
		my_type& operator=(my_type&& rhs) { return (*this = rhs); }

		bool operator==(const my_type& rhs) const { 
			return same_container(rhs) && (rhs.logical_index == logical_index);
		}

		bool operator<(const my_type& rhs) const {
			return same_container(rhs) && (logical_index < rhs.logical_index);
		}

		my_type& operator+=(const difference_type dist) {
			if (logical_index == k_logical_end)
				logical_index = container->size() + dist;
			else
				logical_index += dist;
			if (logical_index >= container->size())
				logical_index = k_logical_end;
			return *this;
		}

		difference_type operator-(const my_type& rhs) const {
			auto li = (logical_index == k_logical_end) ? container->size() : logical_index;
			auto ri = (rhs.logical_index == k_logical_end) ? container->size() : rhs.logical_index;
			return li - ri;
		}

		reference operator*() const { return container->at(logical_index); }

	private:
		bool same_container(const my_type& rhs) const { return rhs.container == container; }

		container_pointer container;
		size_t logical_index;

		static const size_t k_logical_end = size_t(-1);
	};

	template <class T>
	class ring_buffer
	{
	public:
		typedef T value_type;
		typedef T& reference;
		typedef T&& rvalue_reference;
		typedef const T& const_reference;
		typedef T* pointer;
		typedef const T* const_pointer;

		using size_t = std::size_t;
		using difference_type = std::ptrdiff_t;
		using rai_core = ring_buffer_iterator_core<ring_buffer<T>>;

		using const_iterator = codetools::const_random_access_iterator<rai_core, value_type>;
		using iterator = codetools::random_access_iterator<rai_core, value_type>;
		using const_reverse_iterator = std::reverse_iterator<const_iterator>;
		using reverse_iterator = std::reverse_iterator<iterator>;

		// Constructors/dtor
		ring_buffer(size_t capacity = default_ring_buffer_size) noexcept;
		ring_buffer(std::initializer_list<T>);
		ring_buffer(size_t capacity, std::initializer_list<T>);
		template <class InputIterator>
		ring_buffer(size_t capacity, InputIterator start, InputIterator end);
		ring_buffer(const ring_buffer<T>&);
		ring_buffer(ring_buffer<T>&&) noexcept;
		~ring_buffer();

		// Assignment
		ring_buffer<T>& operator=(const ring_buffer<T>& rhs);
		ring_buffer<T>& operator=(ring_buffer<T>&& rhs) noexcept;
		ring_buffer<T>& operator=(std::initializer_list<T> il);

		// Size and capacity
		size_t capacity() const noexcept { return m_capacity; }
		size_t size() const noexcept { return m_size; }
		bool empty() const noexcept { return m_size == 0; }
		void reserve(size_t requested);
		void resize(size_t requested);
		void shrink_to_fit();
	
		// Element access

		reference front() { return *begin(); }
		const_reference front() const { return *cbegin(); }

		reference back() { return *rbegin(); }
		const_reference back() const { return *crbegin(); }

		reference at(size_t pos);
		const_reference at(size_t pos) const;

		reference operator[](size_t pos) { return at(pos); }
		const_reference operator[](size_t pos) const { return at(pos); }

		// Modifying functions

		void push_front(const_reference elem);
		void push_front(rvalue_reference elem);

		void push_back(const_reference elem);
		void push_back(rvalue_reference elem);

		void pop_front(size_t count = 1);

		void pop_back(size_t count = 1);

		iterator insert(const_iterator pos, const_reference elem);
		iterator insert(const_iterator pos, rvalue_reference elem);
		iterator insert(const_iterator pos, std::initializer_list<T> elems);
		template <class InputIterator>	
		typename std::enable_if<is_iterator<InputIterator>::value, iterator>::type
		insert(const_iterator pos, InputIterator first, InputIterator last)
		{
			return insert_impl(pos, first, last);
		}

		iterator erase(const const_iterator& pos);
		iterator erase(const const_iterator& first, const const_iterator& last);

		void clear();

		void swap(ring_buffer<T>& rhs) noexcept
		{
			using std::swap;
			swap(m_size, rhs.m_size);
			swap(m_capacity, rhs.m_capacity);
			swap(m_head, rhs.m_head);
			swap(m_buffer, rhs.m_buffer);
		}

		void swap(ring_buffer<T>&& rhs) noexcept
		{
			swap(rhs);
		}
		
		// Iteration

		// Forward iteration
		iterator begin() noexcept { return iterator(this, 0); }
		iterator end() noexcept { return iterator(this, m_capacity); }
		const_iterator begin() const noexcept { return const_iterator(this, 0); }
		const_iterator end() const noexcept { return const_iterator(this, m_capacity); }
		const_iterator cbegin() const noexcept { return begin(); }
		const_iterator cend() const noexcept { return end(); }

		// Reverse iteration
		reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
		reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
		const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
		const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }
		const_reverse_iterator crbegin() const noexcept { return const_reverse_iterator(end()); }
		const_reverse_iterator crend() const noexcept { return const_reverse_iterator(begin()); }

		size_t _Head() const noexcept { return m_head; };

	private:
		size_t logical_to_real_index(size_t logical_index)
		{
			return (m_head + logical_index) % m_capacity;
		}

		void dispose() {
			clear();
			delete [] ((char*)m_buffer);
			m_buffer = nullptr;
		}

		template <class ForwardIterator>
		iterator insert_impl(const_iterator pos, ForwardIterator first, ForwardIterator last)
		{
			ring_buffer<T> other { m_capacity, first, last };
			const_iterator from { pos };
			while (other.size() < m_capacity && from != end())
				other.push_back(*from++);
			const_reverse_iterator rev { pos };
			auto ins_head = 0;
			while (other.size() < m_capacity && rev != rend())
			{
				other.push_front(*rev++);
				++ins_head;
			}
			swap(other);
			return begin() + ins_head;
		}

		size_t m_size;
		size_t m_capacity;
		size_t m_head;

		T* m_buffer;
	};

	template <class T>
	ring_buffer<T>::ring_buffer(size_t capacity = default_ring_buffer_size) noexcept:
		m_size(0),
		m_capacity(capacity),
		m_head(0),
		m_buffer(nullptr)
	{
		try 
		{
			if (m_capacity)
				m_buffer = (T*)(new char[sizeof(T) * m_capacity]);
		}
		catch (...)
		{
			m_buffer = nullptr;
			m_capacity = 0;
		}
	}

	template <class T>
	ring_buffer<T>::ring_buffer(std::initializer_list<T> il):
		ring_buffer(il.size(), il.begin(), il.end()) 
	{}

	template <class T>
	ring_buffer<T>::ring_buffer(size_t capacity, std::initializer_list<T> il):
		ring_buffer((capacity < il.size()) ? il.size() : capacity, il.begin(), il.end())
	{}

	template <class T>
		template <class InputIterator>
	ring_buffer<T>::ring_buffer(size_t capacity, InputIterator start, InputIterator end) :
		ring_buffer(capacity)
	{
		while (start != end)
			push_back(*start++);
	}

	template <class T>
	ring_buffer<T>::ring_buffer(const ring_buffer<T>& rhs):
		ring_buffer(rhs.m_capacity, rhs.begin(), rhs.end())
	{}

	template <class T>
	ring_buffer<T>::ring_buffer(ring_buffer<T>&& rhs) noexcept :
		ring_buffer()
	{
		swap(rhs);
	}

	template <class T>
	ring_buffer<T>::~ring_buffer()
	{
		dispose();
	}

	template <class T>
	ring_buffer<T>& ring_buffer<T>::operator=(const ring_buffer<T>& rhs)
	{
		swap(ring_buffer<T>(rhs));
		return *this;
	}

	template <class T>
	ring_buffer<T>& ring_buffer<T>::operator=(ring_buffer<T>&& rhs) noexcept
	{
		swap(rhs);
		return *this;
	}

	template <class T>
	ring_buffer<T>& ring_buffer<T>::operator=(std::initializer_list<T> il)
	{
		swap(ring_buffer<T>(il));
		return *this;
	}

	template <class T>
	void ring_buffer<T>::reserve(size_t requested)
	{
		if (m_capacity < requested)
			swap(ring_buffer<T>(requested, begin(), end()));
	}
	
	template <class T>
	void ring_buffer<T>::resize(size_t requested)
	{
		reserve(requested);
		while (requested > m_size)
			new (&m_buffer[logical_to_real_index(m_size++)]) T;
		if (requested < m_size)
			pop_back(m_size - requested);
	}

	template <class T>
	void ring_buffer<T>::shrink_to_fit()
	{
		if (m_size < m_capacity)
			swap(ring_buffer<T>(m_size, begin(), end()));
	}

	template <class T>
	typename ring_buffer<T>::reference 
	ring_buffer<T>::at(size_t pos)
	{
		if (pos >= m_size)
			throw std::out_of_range("Index beyond end");
		size_t real_index = logical_to_real_index(pos);
		return m_buffer[real_index];
	}

	template <class T>
	typename ring_buffer<T>::const_reference
	ring_buffer<T>::at(size_t pos) const
	{
		if (pos >= m_size)
			throw std::out_of_range("Index beyond end");
		size_t real_index = logical_to_real_index(pos);
		return m_buffer[real_index];
	}

	template <class T>
	void ring_buffer<T>::push_front(const_reference elem)
	{
		if (m_size == m_capacity) 
			pop_back();
		if (m_head == 0)
			m_head = m_capacity;
		new (&m_buffer[--m_head]) T(elem);
		++m_size;
	}

	template <class T>
	void ring_buffer<T>::push_front(rvalue_reference elem)
	{
		if (m_size == m_capacity) 
			pop_back();
		if (m_head == 0)
			m_head = m_capacity;
		new (&m_buffer[--m_head]) T(elem);
		++m_size;
	}

	template <class T>
	void ring_buffer<T>::push_back(const_reference elem)
	{
		if (m_size == m_capacity) 
			pop_front();
		size_t pos = logical_to_real_index(m_size++);
		new (&m_buffer[pos]) T(elem);
	}

	template <class T>
	void ring_buffer<T>::push_back(rvalue_reference elem)
	{
		if (m_size == m_capacity) 
			pop_front();
		size_t pos = logical_to_real_index(m_size++);
		new (&m_buffer[pos]) T(std::move(elem));
	}

	template <class T>
	void ring_buffer<T>::pop_front(size_t count = 1)
	{
		if (count > m_size)
			count = m_size;
		for (size_t n = 0; n < count; ++n)
			m_buffer[logical_to_real_index(n)].~T();
		m_head = logical_to_real_index(count);
		m_size -= count;
	}

	template <class T>
	void ring_buffer<T>::pop_back(size_t count = 1)
	{
		if (count > m_size)
			count = m_size;
		size_t newSize = m_size - count;
		for (size_t n = newSize; n < m_size; ++n)
			m_buffer[logical_to_real_index(n)].~T();
		m_size -= count;
	}

	template <class T>
	typename ring_buffer<T>::iterator 
	ring_buffer<T>::insert(
		typename ring_buffer<T>::const_iterator pos,
		typename ring_buffer<T>::const_reference elem)
	{
		if (m_size == m_capacity)
			pop_front();
		iterator iter = end();
		iterator ins = iterator(this, pos - begin());
		new (&m_buffer[logical_to_real_index(m_size++)]) T(std::move(*--iter));
		for ( ; pos < iter; --iter)
			*iter = std::move(*(iter - 1));
		*ins = elem;
		return ins;
	}

	template <class T>
	typename ring_buffer<T>::iterator
	ring_buffer<T>::insert(const_iterator pos, rvalue_reference elem)
	{
		if (m_size == m_capacity)
			pop_front();
		iterator iter = end();
		iterator ins = iterator(this, pos - begin());
		m_buffer[logical_to_real_index(m_size++)] = std::move(*--iter);
		for (; pos < iter; --iter)
			*iter = std::move(*(iter - 1));
		*ins = std::move(elem);
		return ins;
	}

	template <class T>
	typename ring_buffer<T>::iterator
	ring_buffer<T>::insert(const_iterator pos, std::initializer_list<T> elems)
	{
		return insert(pos, elems.begin(), elems.end());
	}

	template <class T>
	typename ring_buffer<T>::iterator
	ring_buffer<T>::erase(typename const ring_buffer<T>::const_iterator& pos)
	{
		return erase(pos, pos + 1);
	}

	template <class T>
	typename ring_buffer<T>::iterator
	ring_buffer<T>::erase(
		typename const ring_buffer<T>::const_iterator& first,
		typename const ring_buffer<T>::const_iterator& last)
	{
		difference_type first_index = first - begin();
		if (first < last)
		{
			const_iterator copy_from { this, last - begin() };
			for (iterator copy_to { this, first_index }; copy_to != end(); ++copy_to, ++copy_from)
			{
				if (copy_from < end())
					*copy_to = std::move(*copy_from);
				else
					(*copy_to).~T();
			}
			m_size -= last - first;
		}
		return iterator { this, first_index };
	}

	template <class T>
	void ring_buffer<T>::clear()
	{
		erase(cbegin(), cend());
		m_head = 0;
	}
}

#endif // CODETOOLS_RING_BUFFER_H
