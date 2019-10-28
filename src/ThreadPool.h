#pragma once
#ifndef __THREAD_POOL__H
#define __THREAD_POOL__H
#include <atomic>
#include <vector>
#include <thread>
#include <boost/lockfree/queue.hpp>
#include <functional>
#include <future>

using namespace std;

class ThreadPool {
private:
	unsigned threadCount;
	atomic<bool> done;
	boost::lockfree::queue<std::function<void(int)> *> workQueue{100};
	std::vector<std::thread> threads;
public:
	ThreadPool(unsigned count);
	~ThreadPool();

    template<typename F, typename... Rest>
    auto submit(F && f, Rest&&... rest) ->std::future<decltype(f(0, rest...))> {
        auto pck = std::make_shared<std::packaged_task<decltype(f(0, rest...))(int)>>(
            std::bind(std::forward<F>(f), std::placeholders::_1, std::forward<Rest>(rest)...)
        );

        auto taskPtr = new std::function<void(int id)>([pck](int id) {
            (*pck)(id);
        });

        this->workQueue.push(taskPtr);
        return pck->get_future();
    }

    template<typename F>
    auto submit(F && f) ->std::future<decltype(f(0))> {
        auto pck = std::make_shared<std::packaged_task<decltype(f(0))(int)>>(std::forward<F>(f));

        auto _f = new std::function<void(int id)>([pck](int id) {
            (*pck)(id);
        });

        this->workQueue.push(_f);
        return pck->get_future();
    }


    void join();
    ThreadPool(const ThreadPool &) = delete;
    ThreadPool(ThreadPool &&) = delete;
private:
	void workerThread();
void taskThread();
};

#endif
