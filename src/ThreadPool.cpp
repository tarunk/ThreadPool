#include "ThreadPool.h"
#include <pthread.h>
#include <cpuid.h>
#include <iostream>
#include <sstream>

void ThreadPool::workerThread() {
	while (!done) {
        std::function<void(int id)>* _f = nullptr;
		if (!this->workQueue.empty() && this->workQueue.pop(_f)) {
            std::unique_ptr<std::function<void(int id)>> func(_f);
            std::function<void(int)> f;
            if  (nullptr != _f) {
                f = *_f;
                f(0);
            }
        } else {
			std::this_thread::yield();
		}
	}
}

ThreadPool::ThreadPool(unsigned count, int queueSZ) : done(false), threadCount(count), workQueue(queueSZ), threads(vector<std::thread>(count)) {
	std::cout << "Launching [" << threadCount << "] threads " << std::endl;
    threads.clear();

	try {
		for (unsigned i = 0; i < threadCount; ++i) {

			std::thread t = std::thread(&ThreadPool::workerThread, this);
			cpu_set_t cpuset;
			CPU_ZERO(&cpuset);
			CPU_SET(i, &cpuset);
			int rc = pthread_setaffinity_np(t.native_handle(), sizeof(cpu_set_t), &cpuset);

			if (rc != 0) {
				std::cerr << "Error calling pthread_setaffinity_np: " << rc << endl;;
			}
			threads.push_back(std::move(t));
		}
	}
	catch (...) {
        std::cerr << "Exception while launching thread. " << std::endl;
		done = true;
		throw;
	}
}


ThreadPool::~ThreadPool() {
	done = true;
}


void ThreadPool::join() {
    for (unsigned i = 0; i < threadCount; ++i) {
        threads[i].join();
    }
}

