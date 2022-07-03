#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>

void HelloThread()
{
    cout << "Hello Thread " << endl;
}

void HelloThread2(int32 num)
{
    cout << num << endl;
}

int main()
{
    std::thread t(HelloThread);
    // thread 로 HelloThread를 실행 시 독립적으로 실행 됨
    // thread는 호출가능 타입을 호출함

    cout << "Hello Main" << endl;


    //int32 count = t.hardware_concurrency(); // CPU 코어 개수가 몇개인지 알려줌
                                              // 동시에 실행 가능한 쓰레드 개수를 의미함
    //auto id = t.get_id();                   // 쓰레드마다 id를 알려줌, 쓰레드를 구분하는데 이용
                                              // 쓰레드 객체에 함수호출이 될 때 id가 할당 됨
    //t.detach();                             // thread 객체에서 실제 쓰레드를 분리
                                              // 분리 시 thread 객체의 상태, 정보 추출이 불가능
    //t.joinable();                           // 쓰레드가 연동되어 있는지를 판별
                                              // 쓰레드 객체의 생성과 함수 호출을 따로할 때 사용

    t.join();
    // join 함수로 thread 객체가 실행한 함수가 끝날때까지 기다리도록 함

    int32 count = t.hardware_concurrency();
    cout << "코어 개수 : " << count << endl;


    // 벡터에 쓰레드를 담아 이용할 경우 생성과 함수 호출을 달리하는 경우가 생김
    // ★쓰레드는 병렬로 실행되기 때문에 어느것이 먼저 호출될지는 알 수 없음
    vector<std::thread> v;

    for (int32 i = 0; i < 10; ++i)
        v.push_back(std::thread(HelloThread2, i));

    for (std::thread& th : v)
        if (th.joinable())
            th.join();

    cout << "Hello Main" << endl;
}
