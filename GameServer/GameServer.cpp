// ------------------------------------------------------------------------------------------------------------------------
// Event - Lock의 구현 이론
// 
#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include "save.h"
#include <windows.h>

mutex m;
queue<int32> q;
HANDLE handle;

void Producer()
{
	while (true) {
		{
			unique_lock<mutex> lock(m);
			q.push(100);
		}

		::SetEvent(handle);
		// 이벤트의 상태를 Signal로 바꿈
		this_thread::sleep_for(100ms);
	}
}

void Consumer()
{
	while (true) {
		::WaitForSingleObject(handle, INFINITE);
		// handle 에 해당하는 이벤트가 발생할 때까지 INFINITE 만큼 대기함
		// Event을 Auto로 해두었기 때문에 Non-Signal 로 자동으로 변경함
		// ::ResetEvent(handle);

		unique_lock<mutex> lock(m);
		if (!q.empty()) {
			int32 data = q.front();
			q.pop();
			cout << data << endl;
		}
	}
}

int main()
{

	// 커널 오브젝트
	// Usage Count
	// Signal / Non-Signal (켜짐/꺼짐) - bool
	// Auto / Manual - bool
	// 보안속성, 수동(true)/자동(flase) 리셋, 초기상태, 이름
	handle = ::CreateEvent(NULL, false, false, NULL);

	thread t1(Producer);
	thread t2(Consumer);

	t1.join();
	t2.join();

	::CloseHandle(handle);

	//save("GameServer.cpp");
}