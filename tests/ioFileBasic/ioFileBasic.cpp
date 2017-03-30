#include "ioTools\ioObject.h"
#include "ioTools\ioFileObject.h"

#include <iostream>

int main()
{
	ioFileObject file { "ioTest.txt" };
	char buffers[2][512];
	ioFuture futures[2];
	ioResult result;
	unsigned n = 0, offset = 512;
	futures[n] = file.ReadAsync(buffers[n], 512, 0);
	do {
		unsigned next = (n + 1) & 1;
		futures[next] = file.ReadAsync(buffers[next], 512, offset);
		result = futures[n].get();
		if (!result.error)
			std::cout.write(result.data, result.length);
		else
			std::cout << "Error: " << result.error << ", " << result.warning << std::endl;
	} while (!result.error && !result.warning);

	std::cout << "Final error code: " << result.error << "\n";
	std::cout << "Final warning code: " << result.warning << std::endl;;
}
