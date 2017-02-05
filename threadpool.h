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
	using func = std::function<void()>; //defines "func" as a shorthand keyword for "std::function<void(void)>".
	//typedef std::function<void(void)> func; //same as above.
	
	ThreadPool(int n):pool(n),queue(),hasItem(),itemMutex(),jobMutex(),shouldContinue(true){
		numTasksGiven = 0;
		numTasksFinished = 0;
		for (auto& t : pool) {
			t = std::thread([=](){this->run();});
		}
	}
	~ThreadPool(){
		for (auto& t : pool) {
			t.join();
		}
	}
	void stop(){
		while (numTasksFinished < numTasksGiven) {
			std::cout << "Stop called. Waiting for threads to finish.\n";
			std::unique_lock<std::mutex> l(jobMutex);
			caughtUp.wait(l);
		}
		//std::cout << "Stopped.\n";
		shouldContinue = false; 
		hasItem.notify_all();
		}
	void post(func task){
		queue.enqueue(task); 
		//std::cout << "Notifying thread of task."; 
		hasItem.notify_one();
		std::lock_guard<std::mutex> l(taskFinishedMutex);
		numTasksGiven +=1;
		std::cout << "Tasks given: " << numTasksGiven << "\n";
		}
	void run(){
		//std::cout << "Thread running.\n";
		while(shouldContinue){//while job is not finished.
			func task;
			while(!queue.try_dequeue(task)){ //while no tasks are on queue, wait.
				//std::cout << "Thread waiting.\n";
				std::unique_lock<std::mutex> l(itemMutex);
				hasItem.wait(l);
				if(!shouldContinue){return;} //if awoken with all tasks finished, end.
			}
			//std::cout << "Running function from thread.\n";
			task();// if task was on queue, do it.
			std::cout << "Thread finished.\n";
			std::lock_guard<std::mutex> l(taskFinishedMutex);
			numTasksFinished += 1;
			std::cout << "Tasks finished: " << numTasksFinished << "\n";
			if (numTasksFinished >= numTasksGiven) {
				caughtUp.notify_one();
			}
		}
	}
	int getNumThreads(){ return pool.size();}
private:
	std::vector<std::thread> pool;
	TSQ<func> queue;
	std::condition_variable hasItem;
	std::condition_variable caughtUp;
	int numTasksGiven;
	int numTasksFinished;
	std::mutex itemMutex;
	std::mutex jobMutex;
	std::mutex taskFinishedMutex;
	std::mutex taskAddedMutex;
	std::atomic<bool> shouldContinue;
};

#endif
