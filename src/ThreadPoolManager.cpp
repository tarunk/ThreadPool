#include <iostream>
#include "ThreadPool.h"

using namespace std;

void testFunIntArg(int id, int n) {
    std::cout << "hello from " << n << ", function\n";
}

void testFunStringArg(int id, const std::string & s) {
    std::cout << __FUNCTION__ << ": " << s << '\n';
}

typedef struct TestObject {
    TestObject(int v) { 
        this->v = v; 
        std::cout << "TestObject ctor " << this->v << '\n'; 
    }
    TestObject(TestObject && c) { 
        this->v = c.v; std::cout<<"TestObject move ctor\n"; 
    }
    TestObject(const TestObject& c) { 
        this->v = c.v; 
        std::cout<<"TestObject copy ctor\n"; 
    }
    ~TestObject() { 
        std::cout << "TestObject dtor\n"; 
    }
    int v;
}TestObject;

void testFunObjectArg(int id, TestObject& t) {
    std::cout << __FUNCTION__ << ": " << t.v << endl;
}


int main() {
	std::cout << "Hello Thread Pool" << endl;
	unsigned const thread_count = std::thread::hardware_concurrency();
	ThreadPool tp(thread_count);
    tp.submit(&testFunIntArg, 10);
    tp.submit(&testFunStringArg, "Hello Test Fun String Arg");
    TestObject obj(120);
    tp.submit(testFunObjectArg, std::ref(obj));
    tp.submit(testFunObjectArg, obj);
    tp.submit(testFunObjectArg, std::move(obj));

    std::string ret1 = "result success";
    auto fun1 = tp.submit([ret1](int) {
        return ret1;
    });

    std::cout << "returned: " << fun1.get() << '\n';

    auto fun2 = tp.submit([](int) {
        throw std::exception();
    });

    try {
        fun2.get();
    } catch (std::exception& e) {
        std::cout << "caught exception\n";
    }

    tp.join();

	return 0;
}
