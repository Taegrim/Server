// ------------------------------------------------------------------------------------------------------------------------
// Lock-Based Stack / Queue
#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <atomic>
#include <mutex>
#include <windows.h>
#include <future>
#include "ConcurrentQueue.h"
#include "ConcurrentStack.h"
#include "save.h"

LockQueue<int32> q;
LockStack<int32> s;

void Push()
{
	while (true) {
		int32 value = rand() % 100 + 1;
		q.Push(value);

		this_thread::sleep_for(10ms);
	}
}

void Pop()
{
	while (true) {
		int32 data = 0;
		//if(q.TryPop(OUT data))
		//	cout << data << endl;

		q.WaitPop(OUT data);
		cout << data << endl;
	}
}

int main()
{
	save("GameServer.cpp");

	thread t1(Push);
	thread t2(Pop);
	thread t3(Pop);

	t1.join();
	t2.join();

	// Stack, Queue에 자체적으로 lock을 구현하면 DeadLock 문제가 발생할 확률이 적어짐
	//  -> 따로 Lock을 잡고 풀 필요없이 Stack/Queue 에서 처리하기 때문에

	
}