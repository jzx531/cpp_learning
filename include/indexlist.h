#ifndef INDEXLIST_H
#define INDEXLIST_H
#include <utility>

template<typename T,T... v>
struct ValueList{
    static constexpr T values[] = {v...};  
    // #define length  (sizeof(values)/sizeof(T))
    static constexpr int length = sizeof...(v);
    void print_values(){
        for(int i=0;i<length;i++){
            std::cout << values[i] << " ";
        }
        std::cout << '\n';
    }
};

template<typename T,typename ValueList>
struct IsEmptyV{
    static constexpr bool value = false;
};

template<typename T>
struct IsEmptyV<T,ValueList<T>> {
    static constexpr bool value = true;
};

template<typename T, T NewVal, typename V>
struct PushFrontVT; // Primary template

// Specialization of PushFrontV
template<typename T, T NewVal, T... v>
struct PushFrontVT<T, NewVal, ValueList<T, v...>> {
    using type = ValueList<T, NewVal, v...>;
};

template<typename T, T NewVal, typename V>
using PushFrontV = typename PushFrontVT<T, NewVal, V>::type;



template<typename T, T NewVal, typename V>
struct PushBackV; // Primary template

// Specialization of PushFrontV
template<typename T, T NewVal, T... v>
struct PushBackV<T, NewVal, ValueList<T, v...>> {
    using type = ValueList<T, v..., NewVal>;
};
//pop front
template<typename T, typename V>
struct PopFrontVT; // Primary template

// Specialization for non-empty ValueList
template<typename T, T Head, T... Tail>
struct PopFrontVT<T, ValueList<T, Head, Tail...>> {
    using type = ValueList<T, Tail...>;
};

// Specialization for empty ValueList (optional, depending on use case)
template<typename T>
struct PopFrontVT<T, ValueList<T>> {
    using type = ValueList<T>;  // 返回空的 ValueList
};

template<typename T, typename ValueList>
using PopFrontV = typename PopFrontVT<T, ValueList>::type;


template<typename T, typename List>
struct FrontVT; 

template<typename T, T Head, T... Tail>
struct FrontVT<T, ValueList<T, Head, Tail...>> {
    static constexpr T value = Head;
};

template<typename T, typename List>
using FrontV = FrontVT<T, List>::value;


// 交换 i 和 j 索引的元素
template<typename T, std::size_t i, std::size_t j, typename List, std::size_t... Indices>
auto swapValuesHelper(List, std::index_sequence<Indices...>) {
    return ValueList<T, (Indices == i ? List::values[j] :
                         Indices == j ? List::values[i] :
                                        List::values[Indices])...>{};
}

// swapValues 主函数
template<typename T, std::size_t i, std::size_t j, T... v>
auto swapValues(ValueList<T, v...>) {
    return swapValuesHelper<T, i, j>(ValueList<T, v...>(), std::make_index_sequence<sizeof...(v)>());
}

template<typename T, typename List, std::size_t... Indices>
auto reverseValuesHelper(List, std::index_sequence<Indices...>) {
    // std::cout << "reverseValuesHelper: " <<List::length<< std::endl;
    return ValueList<T, Indices...>{};
}

// reverseValues 主函数
template<typename T, T... v>
auto reverseValues(ValueList<T, v...>) {
    return reverseValuesHelper<T>(ValueList<T, v...>(), std::make_index_sequence<sizeof...(v)>());
}

// MakeIndexList(N) returns a ValueList<unsigned, 0, 1, 2,..., N-1>
// template<unsigned N,typename Result = ValueList<unsigned>>
// struct MakeIndexListT:MakeIndexListT< N-1,typename PushFrontV<unsigned, N - 1, Result>::type>{};
template<unsigned N,typename Result = ValueList<unsigned>>
struct MakeIndexListT:MakeIndexListT< N-1,typename PushBackV<unsigned, N - 1, Result>::type>{};
// Base case
template<typename Result>
struct MakeIndexListT<0, Result>{
    using type = Result;
};

template<unsigned N>
using MakeIndexList = typename MakeIndexListT<N>::type;


#endif


