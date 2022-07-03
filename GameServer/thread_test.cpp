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
    // thread �� HelloThread�� ���� �� ���������� ���� ��
    // thread�� ȣ�Ⱑ�� Ÿ���� ȣ����

    cout << "Hello Main" << endl;


    //int32 count = t.hardware_concurrency(); // CPU �ھ� ������ ����� �˷���
                                              // ���ÿ� ���� ������ ������ ������ �ǹ���
    //auto id = t.get_id();                   // �����帶�� id�� �˷���, �����带 �����ϴµ� �̿�
                                              // ������ ��ü�� �Լ�ȣ���� �� �� id�� �Ҵ� ��
    //t.detach();                             // thread ��ü���� ���� �����带 �и�
                                              // �и� �� thread ��ü�� ����, ���� ������ �Ұ���
    //t.joinable();                           // �����尡 �����Ǿ� �ִ����� �Ǻ�
                                              // ������ ��ü�� ������ �Լ� ȣ���� ������ �� ���

    t.join();
    // join �Լ��� thread ��ü�� ������ �Լ��� ���������� ��ٸ����� ��

    int32 count = t.hardware_concurrency();
    cout << "�ھ� ���� : " << count << endl;


    // ���Ϳ� �����带 ��� �̿��� ��� ������ �Լ� ȣ���� �޸��ϴ� ��찡 ����
    // �ھ������ ���ķ� ����Ǳ� ������ ������� ���� ȣ������� �� �� ����
    vector<std::thread> v;

    for (int32 i = 0; i < 10; ++i)
        v.push_back(std::thread(HelloThread2, i));

    for (std::thread& th : v)
        if (th.joinable())
            th.join();

    cout << "Hello Main" << endl;
}
