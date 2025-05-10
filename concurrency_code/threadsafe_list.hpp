#ifndef THREADSAFE_LIST_HPP
#define THREADSAFE_LIST_HPP

#include <iostream>
#include "mingw.shared_mutex.h"
#include "mingw.thread.h"
#include "mingw.condition_variable.h"
#include "mingw.mutex.h"
#include <hash_map>
#include <list>
#include <utility>

template<typename T>
class threadsafe_list
{
    struct node
    {
        std::mutex m;
        std::shared_ptr<T> data;
        std::unique_ptr<node> next;
        node():next(){}
        node(T const& value):data(std::make_shared<T>(value)){}
    };
    node head;
    public: 
        threadsafe_list(){}
        ~threadsafe_list(){
            remove_if([](node const&){return true;});
        }
        threadsafe_list(threadsafe_list const& other)=delele;
        threadsafe_list& operator=(threadsafe_list const& other)=delete;
        template<typename Function>
        void for_each(Function f)
        {
            node* current=&head;
            std::unique_lock<std::mutex> lk(head.m);
            while(node* const next=current->next.get())
            {
                std::unique_lock<std::mutex> next_lk(next->m);
                lk.unlock();
                f(*next->data);
                current=next;
                lk = std::move(next_lk);
            }
        }

        template<typename Predicate>
        std::shared_ptr<T> find_first_if(Predicate p)
        {
            node* current =&head;
            std::unique_lock<std::mutex> lk(head.m);
            while(node* const next=current->next.get())
            {
                std::unique_lock<std::mutex> next_lk(next->m);
                lk.unlock();
                if(p(*next->data))
                {
                    return next->data;
                }
                current=next;
                lk = std::move(next_lk);
            }
            return std::shared_ptr<T>();
        }

        template<typename Predicate>
        bool remove_if(Predicate p)
        {
            node* current=&head;
            std::unique_lock<std::mutex> lk(head.m);
            while(node* const next= current->next.get())
            {
                std::unique_lock<std::mutex> next_lk(next->m);
                if(p(*next->data))
                {
                    std::unique_lock<std::mutex> next_lk(next->m);
                    if(p(*next->data))
                    {
                        std::unique_ptr<node> old_next = std::move(current->next);
                        current->next = std::move(next->next);
                        next_lk.unlock();
                    }
                    else 
                    {
                        lk.unlock();
                        current = next;
                        lk = std::move(next_lk);
                    }
                }
            }
        }
};

#endif // THREADSAFE_LIST_HPP
