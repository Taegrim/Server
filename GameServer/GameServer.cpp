// ------------------------------------------------------------------------------------------------------------------------
// Future
// mutex, condition_variable 까지 가지 않고 단순한 작업을 처리할 수 있음
// 일회성 이벤트에 유용함
#include "pch.h"
#include <iostream>
#include "CorePch.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <future>
#include "save.h"

int64 Caculate()
{
	int64 sum = 0;

	for (int32 i = 0; i < 100'000; ++i)
		sum += i;

	return sum;
}

void PromiseWorker(std::promise<string>&& promise)
{
	promise.set_value("Secret Message");
}

void TaskWorker(std::packaged_task<int64(void)>&& task)
{
	task();
}

int main()
{
	// std::future
	{
		std::future<int64> future = std::async(std::launch::async, Caculate);
		// Caculate를 위한 전용 쓰레드를 만듦

		int64 sum = future.get();
		cout << sum << endl;
	}

	// std::promise  (future를 세팅할 수 있는 객체)
	{
		// 미래(std::future)에 결과물을 반환해줄거라 약속(std::promise) 하는 것
		std::promise<string> promise;
		std::future<string> future = promise.get_future();
		// future 추출 

		thread t(PromiseWorker, std::move(promise));
		// future는 main쓰레드, promise는 Worker쓰레드가 소유함

		string message = future.get();
		cout << message << endl;
		// 데이터 추출 시 future 는 empty 상태가 됨 -> 한번만 호출할 수 있음

		t.join();
	}

	// std::packaged_task
	{
		std::packaged_task<int64(void)> task(Caculate);
		std::future<int64> future = task.get_future();

		std::thread t(TaskWorker, std::move(task));
		// 이미 존재하고 있는 쓰레드에 일감을 떠넘겨 주는 것(여러개 넘길 수 있음)

		int64 sum = future.get();
		cout << sum << endl;
		
		t.join();
	}

	// 1) async
	// 원하는 함수를 비동기적으로 실행
	// 2) promise
	// 결과물을 promise를 통해 future로 받아줌
	// 3) packkaged_task
	// 원하는 함수의 실행 결과를 packaged_task를 통해 future로 받아줌

	//save("GameServer.cpp");
}