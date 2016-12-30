#ifndef CODETOOLS_ITERATORS_H
#define CODETOOLS_ITERATORS_H
#pragma once

#include <iterator>
#include <utility>

namespace codetools 
{
	// Random access Core type must support:
	// default ctor, copy ctor, move ctor
	// assignment (lval & rval)
	// equality comparison (==)
	// inequality less-than comparison (<)
	// compound addition-assignment (+=)
	// distance (subtraction) operation (-)
	// dereference (unary *)

	template <class Core, class ValueType>
	struct const_random_access_iterator : std::iterator<std::random_access_iterator_tag, ValueType>
	{
		using value_type = ValueType;
		using reference = const value_type&;
		using pointer = const value_type*;
		using const_core_reference = const Core&;
		using const_core_pointer = const Core*;
		using core_rvalue = Core&&;
		using iterator_type = const_random_access_iterator;
		using iterator_reference = iterator_type&;
		using const_reference_iterator = const iterator_type&;
		using rvalue_reference_iterator = iterator_type&&;

		const_random_access_iterator() noexcept {}
		const_random_access_iterator(const_reference_iterator rhs) noexcept : core(rhs.core) {}
		const_random_access_iterator(rvalue_reference_iterator rhs) noexcept : core(std::move(rhs.core)) {}
		const_random_access_iterator(const_core_reference rhs) noexcept : core(rhs) {}
		const_random_access_iterator(const_core_pointer rhs) noexcept : const_random_access_iterator(*rhs) {}
		const_random_access_iterator(core_rvalue rhs) noexcept : core(std::move(rhs)) {}
		template <class ... Args>
		const_random_access_iterator(Args... args) noexcept : core(std::forward<Args>(args)...) {}
		~const_random_access_iterator() noexcept {}

		iterator_reference operator=(const_reference_iterator rhs) { core = rhs.core; return *this; }
		iterator_reference operator=(rvalue_reference_iterator rhs) { core = std::move(rhs.core); }

		bool operator==(const_reference_iterator rhs) const { return core == rhs.core; }
		bool operator!=(const_reference_iterator rhs) const { return !(core == rhs.core); }

		bool operator<(const_reference_iterator rhs) const { return core < rhs.core; }
		bool operator>(const_reference_iterator rhs) const { return rhs < *this; }
		bool operator<=(const_reference_iterator rhs) const { return !(rhs < *this); }
		bool operator>=(const_reference_iterator rhs) const { return !(*this < rhs); }

		iterator_reference operator+=(difference_type dist) { core += dist; return *this; }
		iterator_reference operator-=(difference_type dist) { core += -dist; return *this; }

		iterator_type operator+(difference_type dist) const { return iterator_type(core) += dist; }
		iterator_type operator-(difference_type dist) const { return iterator_type(core) -= dist; }
		difference_type operator-(const_reference_iterator rhs) const { return core - rhs.core; }
		
		iterator_reference operator++() { return (*this += 1); }
		iterator_reference operator--() { return (*this -= 1); }
		iterator_type operator++(int) { iterator_type tmp(*this); ++(*this); return tmp; }
		iterator_type operator--(int) { iterator_type tmp(*this); --(*this); return tmp; }

		reference operator*() const { return *core; }
		pointer operator->() const { return pointer(**this); }
		reference operator[](difference_type dist) const { return *(*this + dist); }

	protected:
		Core core;
	};

	template <class Core, class ValueType>
	typename const_random_access_iterator<Core, ValueType>::iterator_type
		operator+(
			typename const_random_access_iterator<Core, ValueType>::difference_type dist,
			const const_random_access_iterator<Core, ValueType>& rhs)
	{
		return rhs + dist;
	}

	template <class Core, class ValueType>
	struct random_access_iterator : const_random_access_iterator<Core, ValueType>
	{
		using value_type = ValueType;
		using reference = value_type&;
		using pointer = value_type*;
		using const_core_reference = const Core&;
		using const_core_pointer = const Core*;
		using core_rvalue = Core&&;
		using iterator_type = random_access_iterator;
		using iterator_reference = iterator_type&;
		using const_reference_iterator = const iterator_type&;
		using rvalue_reference_iterator = iterator_type&&;

		using base_type = const_random_access_iterator<Core, ValueType>;

		random_access_iterator() noexcept {}
		random_access_iterator(const_reference_iterator rhs) noexcept :base_type(rhs.core) {}
		random_access_iterator(rvalue_reference_iterator rhs) noexcept :base_type(std::move(rhs.core)) {}
		random_access_iterator(const_core_reference rhs) noexcept : base_type(rhs) {}
		random_access_iterator(const_core_pointer rhs) noexcept : base_type(*rhs) {}
		random_access_iterator(core_rvalue rhs) noexcept : base_type(std::move(rhs)) {}
		template <class ... Args>
		random_access_iterator(Args... args) noexcept : base_type(std::forward<Args>(args)...) {}
		~random_access_iterator() noexcept {}

		iterator_reference operator=(const_reference_iterator rhs) { core = rhs.core; return *this; }
		iterator_reference operator=(rvalue_reference_iterator rhs) { core = std::move(rhs.core); }

		bool operator==(const_reference_iterator rhs) const { return core == rhs.core; }
		bool operator!=(const_reference_iterator rhs) const { return !(core == rhs.core); }

		bool operator<(const_reference_iterator rhs) const { return core < rhs.core; }
		bool operator>(const_reference_iterator rhs) const { return rhs < *this; }
		bool operator<=(const_reference_iterator rhs) const { return !(rhs < *this); }
		bool operator>=(const_reference_iterator rhs) const { return !(*this < rhs); }

		iterator_reference operator+=(difference_type dist) { core += dist; return *this; }
		iterator_reference operator-=(difference_type dist) { core += -dist; return *this; }

		iterator_type operator+(difference_type dist) const { return iterator_type(core) += dist; }
		iterator_type operator-(difference_type dist) const { return iterator_type(core) -= dist; }
		difference_type operator-(const_reference_iterator rhs) const { return core - rhs.core; }

		iterator_reference operator++() { return (*this += 1); }
		iterator_reference operator--() { return (*this -= 1); }
		iterator_type operator++(int) { iterator_type tmp(*this); ++(*this); return tmp; }
		iterator_type operator--(int) { iterator_type tmp(*this); --(*this); return tmp; }

		reference operator*() const { return *core; }
		pointer operator->() const { return &(**this); }
		reference operator[](difference_type dist) const { return *(*this + dist); }
	};

	template <class Core, class ValueType>
	typename random_access_iterator<Core, ValueType>::iterator_type
		operator+(
			typename random_access_iterator<Core, ValueType>::difference_type dist,
			const random_access_iterator<Core, ValueType>& rhs)
	{
		return rhs + dist;
	}
}

#endif // CODETOOLS_ITERATORS_H
