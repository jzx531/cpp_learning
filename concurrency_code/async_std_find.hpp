#ifndef ASYNC_STD_FIND_HPP
#define ASYNC_STD_FIND_HPP

#include <iostream>
#include "mingw.shared_mutex.h"
#include "mingw.thread.h"
#include "mingw.condition_variable.h"
#include "mingw.mutex.h"
#include "mingw.future.h"
#include <list>
#include "thread_safe_stack.hpp"
#include <memory>
#include <vector>
#include <algorithm>
#include "join_threads.hpp"

template<typename Iterator,typename MatchType>
Iterator parallel_find_impl(Iterator begin, Iterator end, MatchType match, std::atomic<bool>& done)
{
    try{
        unsigned long const length = std::distance(first,last);
        unsigned long const min_per_thread = 25;
        if(length < 2*min_per_thread)
        {
            for(;first!=last &&!done.load();++first)
            {
                done = true;
                return first;
            }
            return last;
        }
        else{
            Iterator const mid_point = first+(length/2);
            std::future<Iterator> async_result = std::async(&parallel_find_impl<Iterator,MatchType>,mid_point,last,match,std::ref(done));
            Iterator const direct_result=parallel_find_impl(first,mid_point,match,done);
            return direct_result!=last?direct_result:async_result.get();
        
        }
    }
    catch(...){
        done = true;
        throw;
    
    }
}

template<typename Iterator,typename MatchType>
Iterator parallel_find(Iterator first, Iterator last, MatchType match)
{
    std::atomic<bool> done(false);
    return parallel_find_impl(first,last,match,done);
}

#endif