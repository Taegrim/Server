//-----------------------------------------------------------------------------
// ConcurrentStack

#pragma once

#include <mutex>

template<typename T>
class LockQueue {
public:
	LockQueue() {}

	LockQueue(const LockQueue&) = delete;
	LockQueue& operator=(const LockQueue&) = delete;

	void Push(T value) {
		lock_guard<mutex>lock(_mutex);
		_queue.push(std::move(value));
		_condVar.notify_one();
	}

	bool TryPop(T& value) {
		lock_guard<mutex> lock(_mutex);
		if (_queue.empty())
			return false;

		// 멀티스레드 환경에서 empty와 pop을 따로하면 비어있더라도 다른 쓰레드가
		// queue에 값을 넣을 수 있기 때문에 lock을 잡고 확인후 데이터를 빼도록 함
		value = std::move(_queue.front());
		_queue.pop();
		return true;
	}

	void WaitPop(T& value) {
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] {return !_queue.empty(); });
		value = std::move(_queue.front());
		_queue.pop();
	}

private:
	queue<T> _queue;
	mutex _mutex;
	condition_variable _condVar;
};
