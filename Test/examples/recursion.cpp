#include <cstdlib>
#include <iostream>

extern "C" {
	int fib(int);
}

int main(int argc, char* argv[]) {
	if(argc < 2) {
		std::cout << "provide fibonacci index to find as an argument" << std::endl;
		return 1;
	}

	std::cout << "Fibonacci number " << argv[1] << " is " << fib(std::atoi(argv[1])) << std::endl;
	return 0;
}
