// ------------------------------------------------------------------------------------------------------------------------
// CPU 파이프라인
#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include "save.h"

// 가시성, 코드 재배치
int32 x = 0;
int32 y = 0;
int32 r1 = 0;
int32 r2 = 0;

volatile bool ready;

void Thread_1()
{
	while (!ready)
		;

	y = 1;	// Store y
	r1 = x;	// Load x
}

void Thread_2()
{
	while (!ready)
		;

	x = 1;	// Store x
	r2 = y;	// Load y
}

int main()
{
	int32 count = 0;

	while (true) {
		ready = false;
		count++;

		x = y = r1 = r2 = 0;

		thread t1(Thread_1);
		thread t2(Thread_2);

		ready = true;

		t1.join();
		t2.join();

		if (r1 == 0 && r2 == 0)
			break;
	}

	cout << count << " 번만에 빠져나옴 " << endl;

	// 가시성 문제
	// 멀티 쓰레드에선 코어마다 고유의 캐시를 가지고 있음
	// 따라서 공유 데이터를 사용할 때 그 값이 각각의 캐시에 저장될 수 있고 사용되기 때문에
	// 한 쓰레드가 공유 데이터의 값을 변경하여도 다른 쓰레드에서 캐시에 저장된 변경전의 데이터를 사용할 수 있다

	// 코드 재배치
	// 컴파일러/CPU 는 판단하에 작성한 코드의 위치를 수정할 수 있음
	// -> 단일 쓰레드에서 로직의 결과물이 같다는 보장이 있으면 재배치를 함

	//save("GameServer.cpp");
}