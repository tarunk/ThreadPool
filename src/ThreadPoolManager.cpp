#include <iostream>
#include "ThreadPool.h"

using namespace std;

void first(int id, int n) {
    std::cout << "hello from " << n << ", function\n";
}

int main() {
	std::cout << "Hello Thread Pool" << endl;
	unsigned const thread_count = std::thread::hardware_concurrency();
	ThreadPool tp(1);
    tp.submit(&first, 10);
    tp.join();

	return 0;
}
