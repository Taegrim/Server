// ------------------------------------------------------------------------------------------------------------------------
// Dead Lock
// 
#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include "save.h"
#include "AccountManager.h"
#include "UserManager.h"

void Func1()
{
	for (int32 i = 0; i < 1000; ++i)
		UserManager::Instance()->ProcessSave();
}

void Func2()
{
	for (int32 i = 0; i < 1000; ++i)
		AccountManager::Instance()->ProcessLogin();
}

int main()
{
	std::thread t1(Func1);
	std::thread t2(Func2);

	t1.join();
	t2.join();

	cout << "Done" << endl;
	// mutex가 여러개일 때 번호를 지정하여 번호에 따라 순서를 맞추면 어느정도는 해결된다


	mutex m1;
	mutex m2;

	std::lock(m1, m2);
	// std::lock을 이용하면 내부적으로 m1, m2에 순서를 부여하여 lock을 함

	lock_guard<mutex> g1(m1, std::adopt_lock);
	lock_guard<mutex> g1(m2, std::adopt_lock);
	// adopt_lock : 이미 lock된 상태니까, 나중에 소멸될 때 풀어주도록 함

	// lock을 Graph 형식으로 구성하여 사이클이 발생하는지를 추적하여 해결할 수 있음

	//save("GameServer.cpp");
}