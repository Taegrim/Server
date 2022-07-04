// ------------------------------------------------------------------------------------------------------------------------
// Spin Lock - Lock의 구현 이론
// 
#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include "save.h"

class SpinLock 
{
public:
	void lock() {
		// CAS (Compare-And-Swap) 
		bool expected = false;
		bool desired = true;

		while (!locked.compare_exchange_strong(expected, desired))
			expected = false;
	}

	void unlock() {
		locked.store(false);
	}

private:
	atomic<bool> locked = false;
};

int32 sum = 0;
SpinLock spinLock;

void Add()
{
	for (int32 i = 0; i < 10'0000; ++i) {
		lock_guard<SpinLock>guard(spinLock);
		sum++;
	}
}

void Sub()
{
	for (int32 i = 0; i < 10'0000; ++i) {
		lock_guard<SpinLock>guard(spinLock);
		sum--;
	}
}

int main()
{
	thread t1(Add);
	thread t2(Sub);

	t1.join();
	t2.join();

	cout << sum << endl;

	//save("GameServer.cpp");
}