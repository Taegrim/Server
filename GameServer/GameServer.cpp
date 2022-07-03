#include "pch.h"
#include <iostream>
#include "CorePch.h"

#include <thread>

void HelloThread()
{
    cout << "Hello Thread" << endl;
}

int main()
{
    //// System Call (OS 커널에 요청)
    //cout << "Hello World" << endl;


    // 쓰레드의 생성은 OS에 요청하여 OS가 처리해야 함
    std::thread t(HelloThread);
    // thread 로 HelloThread를 실행 시 독립적으로 실행 됨

    cout << "Hello Main" << endl;

    t.join();

}
