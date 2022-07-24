// ------------------------------------------------------------------------------------------------------------------------
// Thread Local Storage 
#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include "save.h"


thread_local int32 Threadid = 0;

void ThreadMain(int32 threadid)
{
	Threadid = threadid;

	while (true){
		cout << "Hi I am Thread" << Threadid << endl;
		this_thread::sleep_for(1s);
	}
}

int main()
{
	vector<thread> threads;

	for (int i = 0; i < 10; ++i) 
		threads.push_back(thread(ThreadMain, i + 1));
	
	for (thread& t : threads)
		t.join();
		

	// Heap, 데이터 영역에 있는 정보를 가져와서 쓰레드마다 각각 가지고 있는 
	// 쓰레드별 고유의 지역 공간(TLS)에 저장함
	// 스택은 전역 데이터를 담기에는 불안정하므로 TLS에 가져와서 처리
	// TLS에 정보를 가져오면 Heap, 데이터 영역에 접근하지 않아도 되므로 쓰레드 경합이 적게 발생함 
	// 쓰레드 별로 고유의 공간이므로 데이터의 변화가 있어도 TLS의 값은 변하지 않음

	//save("GameServer.cpp");
}