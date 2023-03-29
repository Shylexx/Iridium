#include <iostream>

extern "C" {
	int mutvar(int);
}

int main() {
	std::cout << "Result of mutvar: " << mutvar(5) << std::endl;
	return 0;
}
