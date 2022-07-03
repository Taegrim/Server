// ------------------------------------------------------------------------------------------------------------------------
// Lock (mutex)
// 공유 데이터 처리 시 해결 방법
#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <mutex>
#include "save.h"

vector<int32> v;
mutex m;
// Mutual Exclusive (상호 배타적)
// atomic 처럼 딜레이가 있으므로 동작이 느림

// 유의점 1. 재귀적으로 lock 사용시 recursive_mutex 사용 (일반적인 mutex는 여러번 lock 불가능)
// 유의점 2. lock 이용 시 unlock을 반드시 해야함 (예외 처리를 할 경우 unlock이 걸리는지에 유의)
//           -> 일일히 unlock을 하면 복잡함, 빠지는 부분이 있을 수 있음 
//           -> RAII 방식으로 처리
// 유의점 3. lock을 거는 범위에 따라 실행이 완전히 달라질 수 있음

// RAII (Resource Aquisition Is Initialization)

template<typename T>
class LockGuard {
public:
	LockGuard(T& m) {
		_mutex = &m;
		_mutex->lock();
	}

	~LockGuard() { _mutex->unlock(); }

private:
	T* _mutex;
};
// 객체 생성에 대한 부담이 있으나 lock, unlock을 수동으로 하는것보다 안정적임

void Push()
{
	// for문 전에 lock을 걸면 Push함수가 끝나야 다른쓰레드가 접근할 수 있어짐
	for (int32 i = 0; i < 1'0000; ++i) {
		//LockGuard<std::mutex> lockGuard(m);
		//std::lock_guard<std::mutex> lockGuard(m);		// 표준 lock_guard

		std::unique_lock<std::mutex> uniqueLock(m, std::defer_lock);
		// std::defer_lock 으로 바로 잠그지 않고 lock() 호출 시 잠그도록 할 수 있음

		uniqueLock.lock();

		if (5000 == i)
			break;

		v.push_back(i);
	}
}

int main()
{
	std::thread t1(Push);
	std::thread t2(Push);
	t1.join();
	t2.join();

	cout << v.size() << endl;

	//save("GameServer.cpp");
}