#ifndef THREADPOOL_ALGORITHM_HPP
#define THREADPOOL_ALGORITHM_HPP
#include "waiting_thread_pool.hpp"

template<typename Iterator,typename T>
T parallel_accumulate(Iterator first,Iterator last,T init)
{
    unsigned long const length=std::distance(first,last);
    if(!length) return init;
    unsigned long const block_size=25;
    unsigned long const num_blocks= (length+block_size-1)/block_size;
    thread_pool pool;
    std::vector<std::future<T>> futures(num_blocks-1);
    Iterator block_start=first;
for(unsigned long i=0;i<num_blocks-1;++i)
    {
        Iterator block_last=block_start;
        std::advance(block_last,block_size);
        futures[i]=pool.submit([=]{
            accumulate_block(block_start,block_last);
        });
        block_start=block_last;
    }
    T last_result=accumulate_block<Iterator,T>(block_start,last);
    T result = init;
    for(unsigned long i=0;i<num_blocks-1;++i)
    {
        result+=futures[i].get();
    }
    result+=last_result;
    return result;
}

template<typename T>
struct sorter
{
    thread_pool pool;
    std::list<T> do_sort(std::list<T>& chunk_data)
    {
        if(chunk_data.empty())
        {
            return chunk_data;
        }
        std::list<T> result;
        result.splice(result.begin(),chunk_data,chunk_data.begin());//将划分点提前存入result
        T const& partition_val=*result.begin();
        Typename std::list<T>::iterator divide_point=std::partition(chunk_data.begin(),chunk_data.end(),[&](T const& val){return val<partition_val;});
        std::list<T> new_lower_chunk;
        new_lower_chunk.splice(new_lower_chunk.end(),chunk_data,chunk_data.begin(),divide_point);
        std::future<std::list<T>> new_higher(do_sort(chunk_data));
        result.splice(result.end(),new_higher.get());
        while(new_lower.wait_for(std::chrono::milliseconds(0))==std::future_status::timeout)
        {
            pool.run_pending_task();
        }
        result.splice(result.begin(),new_lower.get());
        return result;
    }
};

template<typename T>
std::list<T> parallel_quick_sort(std::list<T> input)
{
    if(input.empty())
    {
        return input;
    }
    sorter<T> s;
    return s.do_sort(input);
}


#endif // THREADPOOL_ALGORITHM_HPP