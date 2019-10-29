#pragma once
#ifndef __THREAD_POOL__H
#define __THREAD_POOL__H
#include <atomic>
#include <vector>
#include <thread>
#include <boost/lockfree/queue.hpp>
#include <functional>
#include <future>
// thread pool to run user's functors with signature
//     ret func(int id, other_params)
// where id is the index of the thread that runs the functor
// ret is some return type
using namespace std;

class ThreadPool {
private:
	unsigned threadCount;
	atomic<bool> done;
	boost::lockfree::queue<std::function<void(int)> *> workQueue;
	std::vector<std::thread> threads;
public:
	ThreadPool(unsigned count, int queueSZ = 100);
	~ThreadPool();

    // run the user's function that excepts argument int - id of the running thread. returned value is templatized
    // operator returns std::future, where the user can get the result and rethrow the catched exceptions
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
    ThreadPool & operator=(const ThreadPool &) = delete;
    ThreadPool & operator=(ThreadPool &&) = delete;
private:
	void workerThread();
void taskThread();
};

#endif
