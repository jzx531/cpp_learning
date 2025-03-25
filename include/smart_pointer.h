#ifndef SMART_POINTER_H
#define SMART_POINTER_H

template<typename T>
class shared_ptr{
    public:
        struct counter{
            int count;
            counter():count(0){};
            void inc(){++count;};
            void dec(){--count;};
            int get() const{return count;};
        };
    public:
        typedef T value_type;
        shared_ptr(T* p = nullptr){
            ptr = p;
            counter = new struct counter();
            counter->inc();
        }
        shared_ptr(const shared_ptr& other)
        {
            if(ptr == other.ptr)
            {
                // counter->inc();
            }
            else{
                ptr = other.ptr;
                counter->dec();
                counter = other.counter;
                counter->inc();
            }
        }

        shared_ptr& operator=(const shared_ptr& other)
        {
            if(ptr == other.ptr)
            {
                // counter->inc();
            }
            else{
                ptr = other.ptr;
                counter->dec();
                counter = other.counter;
                counter->inc();
            }
            return *this;
        }

        ~shared_ptr(){
            counter->dec();
            if(counter->get() == 0)
            {
                delete ptr;
            }
        }

        T* get() const{
            return ptr;
        }

        void reset(T* p = nullptr){
          if(ptr!= p)
            {
                counter->dec();
                if(counter->get() == 0)
                {
                    delete ptr;
                }
                ptr = p;
                counter->count = p? 1 : 0;
            }
        }

        T& operator*() const{
            return *ptr;
        }

        T* operator->() const{
            return ptr;
        }

        int use_count() {
            return counter->get();
        }

    private:
        T* ptr;
        counter* counter;
};


#endif

