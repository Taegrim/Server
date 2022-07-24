#pragma once

#include <mutex>

template<typename T>
class LockStack {
public:
	LockStack() {}

	LockStack(const LockStack&) = delete;
	LockStack& operator=(const LockStack&) = delete;

	void Push(T value) {
		lock_guard<mutex> lock(_mutex);
		_stack.push(std::move(value));
		_condVar.notify_one();
	}

	bool TryPop(T& value) {
		lock_guard<mutex> lock(_mutex);
		if (_stack.empty())
			return false;
		
		// 멀티스레드 환경에서 empty와 pop을 따로하면 비어있더라도 다른 쓰레드가
		// stack에 값을 넣을 수 있기 때문에 lock을 잡고 확인후 데이터를 빼도록 함
		value = std::move(_stack.top());
		_stack.pop();
		return true;
	}

	void WaitPop(T& value) {
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] {return !_stack.emptry(); });
		value = std::move(_stack.top());
		_stack.pop();
	}

private:
	stack<T> _stack;
	mutex _mutex;
	condition_variable _condVar;
};
