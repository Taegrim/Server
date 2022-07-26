//-----------------------------------------------------------------------------
// ConcurrentStack

#pragma once

#include <mutex>

template<typename T>
class LockStack {
public:
	LockStack() {}

	LockStack(const LockStack&) = delete;
	LockStack& operator=(const LockStack&) = delete;

	void Push(T value) {
		lock_guard<mutex> lock(_mutex);
		_stack.push(std::move(value));
		_condVar.notify_one();
	}

	bool TryPop(T& value) {
		lock_guard<mutex> lock(_mutex);
		if (_stack.empty())
			return false;
		
		// ��Ƽ������ ȯ�濡�� empty�� pop�� �����ϸ� ����ִ��� �ٸ� �����尡
		// stack�� ���� ���� �� �ֱ� ������ lock�� ��� Ȯ���� �����͸� ������ ��
		value = std::move(_stack.top());
		_stack.pop();
		return true;
	}

	void WaitPop(T& value) {
		unique_lock<mutex> lock(_mutex);
		_condVar.wait(lock, [this] {return !_stack.emptry(); });
		value = std::move(_stack.top());
		_stack.pop();
	}

private:
	stack<T> _stack;
	mutex _mutex;
	condition_variable _condVar;
};

template<typename T>
class LockFreeStack {
	struct Node {
		Node(const T& value) : data(value), next(nullptr) {}

		T data;
		Node* next;
	};

public:
	void Push(const T& value) {
		Node* node = new Node(value);
		node->next = _head;

		while (!_head.compare_exchange_weak(node->next, node))
			;

	}

	// 1) head �б�
	// 2) head->next �б�
	// 3) head = head->next
	// 4) data �����ؼ� ��ȯ
	// 5) ������ ��带 ����

	// �� �����忡�� 2,3 �����߿� �ٸ� �����尡 �ش� ��带 �����ϸ� �̻��� �޸𸮸� ������ �� ����
	bool TryPop(T& value) {
		++_popCount;

		Node* oldHead = _head;
		
		while(oldHead && !_head.compare_exchange_weak(oldHead, oldHead->next))
			;

		if (!oldHead) {
			--_popCount;
			return false;
		}

		value = oldHead->data;
		
		TryDelete(oldHead);
		return true;
	}

	void TryDelete(Node* oldHead) {
		// popCount�� 1�̸� ���� ����ϴ� ��
		// ������ head�� next�� �Ű�� oldHead�� ó���ϴ� ���̱� ������
		// �ٸ� �����尡 �������� ������ �߻����� ����
		if (_popCount == 1) {

			// ���� ����� �ٸ� �����͵� ���� ����
			Node* node = _pendingList.exchange(nullptr);

			if (--_popCount == 0) {
				// ����� �����尡 ���� -> ����� ������ ����
				DeleteNodes(node);
			}
			else if (node) {
				// �ٸ� �����尡 ���������� �ٽ� ���� ���ƾ� ��
				ChainPendingNodeList(node);
			}

			// �� ������ ����
			delete oldHead;
		}
		else {
			// �ٸ� �����尡 Pop�� �����ߴٸ� �������� �ʰ� ���� ������ ��
			ChainPendingNodeList(oldHead);
			--_popCount;
		}
	}

	void ChainPendingNodeList(Node* first, Node* last) {
		last->next = _pendingList;

		while (!_pendingList.compare_exchange_weak(last->next, first))
			;
	}

	void ChainPendingNodeList(Node* node) {
		Node* last = node;
		while (last->next)
			last = last->next;

		ChainPendingNodeList(node, last);
	}

	void ChainPendingNode(Node* node) {
		ChainPendingNodeList(node, node);
	}

	static void DeleteNodes(Node* node) {
		while (node) {
			Node* next = node->next;
			delete node;
			node = next;
		}
	}

private:
	atomic<Node*> _head;

	atomic<uint32> _popCount = 0; // Pop�� �������� ������ ����

	atomic<Node*> _pendingList; // �����Ǿ�� �� ���� (ù��° ���)
};
