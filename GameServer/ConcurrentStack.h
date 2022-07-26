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
		
		// 멀티스레드 환경에서 empty와 pop을 따로하면 비어있더라도 다른 쓰레드가
		// stack에 값을 넣을 수 있기 때문에 lock을 잡고 확인후 데이터를 빼도록 함
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

	// 1) head 읽기
	// 2) head->next 읽기
	// 3) head = head->next
	// 4) data 추출해서 반환
	// 5) 추출한 노드를 삭제

	// 한 쓰레드에서 2,3 수행중에 다른 쓰레드가 해당 노드를 삭제하면 이상한 메모리를 참조할 수 있음
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
		// popCount가 1이면 나만 사용하는 것
		// 기존의 head는 next로 옮겼고 oldHead로 처리하는 것이기 때문에
		// 다른 쓰레드가 끼어들더라도 문제가 발생하지 않음
		if (_popCount == 1) {

			// 삭제 예약된 다른 데이터들 또한 삭제
			Node* node = _pendingList.exchange(nullptr);

			if (--_popCount == 0) {
				// 끼어든 쓰레드가 없음 -> 예약된 데이터 삭제
				DeleteNodes(node);
			}
			else if (node) {
				// 다른 쓰레드가 끼어들었으니 다시 갖다 놓아야 함
				ChainPendingNodeList(node);
			}

			// 내 데이터 삭제
			delete oldHead;
		}
		else {
			// 다른 쓰레드가 Pop을 실행했다면 삭제하지 않고 삭제 예약을 함
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

	atomic<uint32> _popCount = 0; // Pop을 실행중인 쓰레드 개수

	atomic<Node*> _pendingList; // 삭제되어야 할 노드들 (첫번째 노드)
};
