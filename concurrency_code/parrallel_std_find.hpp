#ifndef PARRALLEL_STD_FIND_HPP
#define PARRALLEL_STD_FIND_HPP
#include <iostream>
#include "mingw.shared_mutex.h"
#include "mingw.thread.h"
#include "mingw.condition_variable.h"
#include "mingw.mutex.h"
#include "mingw.future.h"
#include <list>
#include <atomic>
#include "thread_safe_stack.hpp"
#include <memory>
#include <vector>
#include <algorithm>

template<typename Iterator,typename MatchType>
Iterator parallel_find(Iterator begin,Iterator end,MatchType match)
{
    struct find_element
    {
        void operator()(Iterator& begin,Iterator& end,MatchType& match,std::promise<Iterator>*result,std::atomic<bool>*done_flag)
        {
            try{
                for(;begin!=end &&!done_flag->load();++begin)
                {
                    if(*begin==match)
                    {
                        result->set_value(begin);
                        done_flag->store(true);
                        return;
                    }
                    
                }
            }
            catch(...)
            {
                try{
                    result->set_exception(std::current_exception());
                    done_flag->store(true);
                }
                catch(...)
                {
                    //ignore any exception thrown by set_exception
                }
            
            }
        }
    };
    
}

#endif