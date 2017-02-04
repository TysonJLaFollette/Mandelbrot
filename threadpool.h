#ifndef THREADPOOL
#define THREADPOOL
#include <thread>
#include <mutex>
#include "threadsafequeue.h"
#include <condition_variable>
#include <atomic>
#include <functional>
class ThreadPool
{
public:
	using func = std::function<void(void)>; //defines "func" as a shorthand keyword for "std::function<void(void)>".
	//typedef std::function<void(void)> func; //same as above.
	ThreadPool(int n):pool(n),queue(),hasItem(),itemMutex(),shouldContinue(true){
		for (auto& t:pool) {
			t = std::thread([=](){this->run()})
		}
	}
	
	void stop(){shouldContinue = false; hasItem.notify_all();}
	void post(func f){queue.enqueue(f); hasItem.notify();}
	void run(){
		while(shouldContinue){
			func f;
			while(!queue.try_dequeue(f)){
				std::unique_lock<std::mutex> l(itemMutex);
				hasItem.wait(l);
				if(!shouldContinue){return;}
			}
			f();
		}
	}
private:
	std::vector<std::thread> pool;
	TSQ<func> queue;
	std::condition_variable hasItem;
	std::mutex itemMutex;
	std::atomic<boo> shouldContinue;
}

#endif
