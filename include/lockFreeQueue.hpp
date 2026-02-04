#ifndef LOCK_FREE_QUEUE_HPP
#define LOCK_FREE_QUEUE_HPP

#include <atomic>
#include <memory>
 
template<typename T>
class LockFreeQueue {
private:
    struct Node {
        std::shared_ptr<T> data;
        std::atomic<Node*> next;
        Node() : next(nullptr) {}
        Node(T val) : data(std::make_shared<T>(std::move(val))), next(nullptr) {}
    };

    std::atomic<Node*> head;
    std::atomic<Node*> tail;

public:
    LockFreeQueue() {
        Node* dummy = new Node();
        head.store(dummy);
        tail.store(dummy);
    }

    ~LockFreeQueue() {
        while (Node* p = head.load()) {
            head.store(p->next);
            delete p;
        }
    }

    void push(T val) {
        Node* newNode = new Node(std::move(val));
        Node* oldTail = tail.load();
        while (true) {
            Node* next = oldTail->next.load();
            if (next == nullptr) {
                if (oldTail->next.compare_exchange_weak(next, newNode)) {
                    break;
                }
            } else {
                // 帮助其他线程完成 tail 更新
                tail.compare_exchange_weak(oldTail, next);
                oldTail = tail.load();
            }
        }
        tail.compare_exchange_weak(oldTail, newNode); // try to update tail
    }

    std::shared_ptr<T> pop() {
        Node* oldHead = head.load();
        while (true) {
            Node* next = oldHead->next.load();
            if (next == nullptr) {
                return nullptr; // empty
            }
            if (head.compare_exchange_weak(oldHead, next)) {
                std::shared_ptr<T> res = next->data;
                delete oldHead; // delete previous dummy
                return res;
            }
            // else retry
        }
    }
};

#endif // LOCK_FREE_QUEUE_HPP

