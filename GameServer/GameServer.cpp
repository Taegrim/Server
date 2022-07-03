// Atomic
// 공유 데이터 사용 시 발생하는 문제점들을 알아볼 것

#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include "save.h"

// atomic : All - Or - Nothing
// 예시 : DB
// A라는 유저 인벤에서 집행검 빼고
// B라는 유저 인벤에서 집행검을 추가
// 이 경우 한 경우만 일어나면 안되기 때문에 atomic 하게 일어나야함

// atomic을 이용하면 쓰레드간 경합이 발생하면 한 쓰레드가 데이터에 접근하면
// 다른 쓰레드는 잠시 대기하도록 함 (대기하도록 하는 주체는 CPU)
// -> 잠시 대기하도록 하기 때문에 실행 시간이 늦어지는 문제가 있음

atomic<int32> sum = 0;

void Add()
{
    for (int32 i = 0; i < 100'0000; ++i)
        sum.fetch_add(1);
        //sum++
}

void Sub()
{
    for (int32 i = 0; i < 100'0000; ++i)
        sum.fetch_add(-1);
        //--sum;
}

int main()
{
    Add();
    Sub();
    cout << sum << endl;

    std::thread t1(Add);
    std::thread t2(Sub);

    t1.join();
    t2.join();
    cout << sum << endl;

    save("GameServer.cpp");
}