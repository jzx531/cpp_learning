#include <queue>
#include <deque>
#include <mutex>
#include <shared_mutex>
#include <condition_variable>

template<typename T>
class BlockingQueue {
        std::deque<T> deque;
        std::mutex mtx;
        std::condition_variable fullWait;
        std::condition_variable emptyWait;
        int capacity;
    public:
        BlockingQueue(int capacity) : capacity(capacity) {}
        void put(const T& item){
            std::unique_lock<std::mutex> lock(this->mtx);
            try{
                while(deque.size() >= capacity) {
                    fullWait.wait(lock,[&](){return deque.size() < capacity;});
                }
                deque.push_back(item);
                emptyWait.notify_all();
            }catch(...){
                fullWait.notify_all();
                lock.unlock();
            }
        }

        T take(){
            std::unique_lock<std::mutex> lock(this->mtx);
            try{
                while(deque.empty()){
                    emptyWait.wait(lock,[&](){return !deque.empty();});
                }
                T item = deque.front();
                deque.pop_front();
                fullWait.notify_all();
                // lock.unlock();
                return item;
            }catch(...){
                emptyWait.notify_all();
                lock.unlock();
            }
            
        }

};

