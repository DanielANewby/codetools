#include "containers/ring_buffer.h"

int main()
{
	using codetools::ring_buffer;

	ring_buffer<int> buffer1 { 5, 4, 3, 2, 1 };
	ring_buffer<int> buffer2 { 10, {1,2,3,4,5} };
	ring_buffer<int> buffer3(10, {3,4,5,1,2});
	ring_buffer<int> buffer4(5);
}