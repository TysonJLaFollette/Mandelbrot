#ifndef ThreadSafeQueue
#define ThreadSafeQueue
#include <mutex>
#include <queue>

template<typename T>
class TSQ
{
public:
	void enqueue(T t){
		std::lock_guard<std::mutex> l(m);
		q.push(t);
		//std::cout << q.size() << " now in queue.\n";
	}
	
	bool try_dequeue(T& res){
		std::lock_guard<std::mutex> l(m);
		if (q.empty()){return false;}
		res = q.front();
		q.pop();
		//std::cout << q.size() << " left queued.\n";
		return true;
	}

private:
std::mutex m;
std::queue<T> q;
};

#endif
