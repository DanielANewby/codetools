#include "containers/ring_buffer.h"

#include <vector>
#include <iostream>

using codetools::ring_buffer;

template<class T>
ring_buffer<T> make_buffer(typename ring_buffer<T>::size_t size)
{
	return ring_buffer<T>(size);
}

template<class T, class U>
bool compare_container_elements(T& lhs, U& rhs)
{
	lhs::iterator lIter = lhs.begin();
	rhs::iterator rIter = rhs.begin();
	for (; lIter = lhs.end(); ++lIter, ++rIter)
	{
		if (rIter == rhs.end())
			return false;
		if (*lIter != *rIter)
			return false;
	}
	return rIter == rhs.end();
}

template <class T>
std::ostream& operator<<(std::ostream& o, ring_buffer<T>& rhs)
{
	using iterator = ring_buffer<T>::iterator;
	o << rhs.size() << "/" << rhs.capacity() << "(" << rhs._Head() << ") {";
	iterator i, e = rhs.end() - 1;
	for (i = rhs.begin(); i != e; ++i)
		o << *i << ", ";
	o << rhs.back() << "}";
	return o;
}

void ringBufferSmokeTest()
{
	std::vector<int> vec{ 1,2,3,4,5 };
	{
		ring_buffer<int> buffer0;
		ring_buffer<int> buffer1{ 5, 4, 3, 2, 1 };
		ring_buffer<int> buffer2{ 10,{ 1,2,3,4,5 } };
		ring_buffer<int> buffer3(10, { 3,4,5,1,2 });
		ring_buffer<int> buffer4(5);
		ring_buffer<int> buffer5{ 10, vec.begin(), vec.end() };
		ring_buffer<int> copy{ buffer5 };
		ring_buffer<int> buffer_move{ std::move(make_buffer<int>(10)) };
	}

	ring_buffer<int> source {10, { 1, 2, 3, 4, 5 }};
	ring_buffer<int> assign;
	ring_buffer<int> move_copy;
	assign = source;
	move_copy = std::move(assign);
	assign = { 5, 4, 3, 2, 1 };

	std::cout << source << std::endl;
	std::cout << move_copy << std::endl;
	std::cout << assign << std::endl;
	assign.erase(assign.begin(), assign.begin() + 2);
	assign.clear();
	std::cout << "Empty: " << (assign.empty() ? "True" : "False") << std::endl;

	assign = { 5, 4, 3, 2, 1 };
	std::cout << "Before reserve: " << assign << std::endl;
	assign.reserve(20);
	std::cout << "After reserve:  " << assign << std::endl;
	std::cout << assign << std::endl;
	std::cout << "Before resize: " << assign << std::endl;
	assign.resize(10);
	std::cout << "After resize: " << assign << std::endl;
	assign.resize(3);
	std::cout << "After resize(3): " << assign << std::endl;
	assign.shrink_to_fit();
	std::cout << "After shrink: " << assign << std::endl;
	std::cout << "Front: " << assign.front() << " Back: " << assign.back() << std::endl;
	for (decltype(assign.size()) size = assign.size(), n = 0; n < size; ++n)
		std::cout << n << ": " << assign[n] << ", ";
	std::cout << std::endl;

	assign = { 1, 2, 3, 4, 5 };
	std::cout << "Start: " << assign << std::endl;
	assign.push_front(6);
	std::cout << "Push_front: " << assign << std::endl;
	assign.pop_front(2);
	std::cout << "Pop_front(2): " << assign << std::endl;
	assign.push_back(7);
	assign.push_back(8);
	std::cout << "Push_back {1,2}: " << assign << std::endl;
	assign.pop_back();
	std::cout << "Pop_back(): " << assign << std::endl;
	assign.pop_back(2);
	std::cout << "Pop_back(2): " << assign << std::endl;

	int i = 7;
	assign = { 1, 2, 3, 4, 5 };
	assign.reserve(10);
	using iterator = ring_buffer<int>::iterator;
	iterator ins = assign.insert(assign.cbegin() + 2, 6); 
	// Range should be   { 1, 2, 6, 3, 4, 5 }
	// Storage should be { 1, 2, 6, 3, 4, 5 }
	std::cout << assign << std::endl;
	ins = assign.insert(++ins, i); 
	// Range should be   { 1, 2, 6, 7, 3, 4, 5 }
	// Storage should be { 1, 2, 6, 7, 3, 4, 5 }
	std::cout << assign << std::endl;
	assign.insert(++ins, { 8, 9, 10, 11 }); 
	// Range should be   { 2, 6, 7, 8, 9, 10, 11, 3, 4, 5 }
	// Storage should be { 5, 2, 6, 7, 8, 9, 10, 11, 3, 4 }
	std::cout << assign << std::endl;
	vec = { 11, 12, 13 };
	assign.insert(assign.end() - 1, vec.begin(), vec.end());
	// Range should be   { 8, 9, 10, 11, 3, 4, 11, 12, 13, 5 }
	// Storage should be { 11, 12, 13, 5, 8, 9, 10, 11, 3, 4 }
	std::cout << assign << std::endl;
}
