#ifndef TUPLE_IMPL_H
#define TUPLE_IMPL_H

#include "tuple.h"
#include "indexlist.h"
#include <utility>  
#include <array>

//按索引反转元组
template<typename...Elements,unsigned...Indices>
auto reverseImpl(Tuple<Elements...>& t,ValueList<unsigned,Indices...>const &)
{
    return makeTuple(getTupleElement<Indices>(t)...);
}

template <typename... Elements>
auto reverse_tuple(Tuple<Elements...>& t)
{
    return reverseImpl(t,MakeIndexList<sizeof...(Elements)>());
}

//打乱和选择
template<typename...Elements,unsigned...Indicies>
auto select(Tuple<Elements...>&t,ValueList<unsigned,Indicies...>const &)
{
    return makeTuple(getTupleElement<Indicies>(t)...);
}

//索引集复制
template<unsigned I,unsigned N,typename IndexList = ValueList<unsigned>>
class ReplicateIndexListT;

template<unsigned I,unsigned N,unsigned...Indices>
class ReplicateIndexListT<I,N,ValueList<unsigned,Indices...>>:public 
    ReplicateIndexListT<I,N-1,typename PushBackV<unsigned, I, ValueList<unsigned,Indices...>>::type>{};

template<unsigned I,unsigned...Indices>
class ReplicateIndexListT<I,0,ValueList<unsigned,Indices...>>{
    public:
        using type = ValueList<unsigned,Indices...>;
};

template<unsigned I,unsigned N>
using ReplicatedIndexList = typename ReplicateIndexListT<I,N>::type;

template<unsigned I,unsigned N,typename...Elements>
auto splat(Tuple<Elements...>& t)
{
    return select(t,ReplicatedIndexList<I,N>());
}


//合并tuple
template<typename...Elements,typename...NewElements>
auto mergeTuple(Tuple<Elements...>& t,Tuple<>&)
{
    return t;
}

template<typename...Elements,typename...NewElements>
auto mergeTuple(Tuple<Elements...>& t,Tuple<NewElements...>& newT)
{
    // unsigned int n = t.getLength();
    auto tmpvl = push_back(t,newT.getHead());
    auto newvl = newT.getTail();
    return mergeTuple(tmpvl,newvl);
}

//交换元祖元素
// 交换 tuple 中索引 i 和 j 的元素,存在类型未交换的情况
template<std::size_t i, std::size_t j, typename... Args,std::size_t... Indices>
auto swap_tuple_helper(Tuple<Args...>& tup, std::index_sequence<Indices...>) {
    return makeTuple(
        (Indices == i ? get<j>(tup) :
         Indices == j ? get<i>(tup) :
         get<Indices>(tup))...);
}

template<std::size_t i, std::size_t j, typename... Args>
auto swap_tuple_elements(Tuple<Args...>& tup) {
    return swap_tuple_helper<i, j>(tup, std::index_sequence_for<Args...>{});
}

//使用索引序列和select进行
//打乱和选择
template<unsigned i,unsigned j,typename...Elements,unsigned...Indicies>
auto swapTupleByIndicies(Tuple<Elements...>&t)
{
    MakeIndexList<sizeof...(Elements)> indicies;
    auto reverseindicies = reverseValues(indicies);
    auto indextmp = swapValues<unsigned, std::integral_constant<unsigned, i>::value, 
                                      std::integral_constant<unsigned, j>::value>(reverseindicies);
    // indextmp.print_values();
    return select(t,indextmp);
}


#include <iostream>
#include <array>
#include <utility>

// 递归展开获取元素，并推入新元组
template<size_t N,std::array<unsigned,N> arr,std::size_t... Indices, typename... Elements>
auto makeTupleFromArrayImpl(Tuple<Elements...>& t, std::index_sequence<Indices...>) {
    return makeTuple(getTupleElement<arr[Indices]>(t)...);
}

// 主函数：调用 `makeTupleFromArrayImpl`
template<size_t N,std::array<unsigned,N> arr, typename... Elements>
auto makeTupleFromArray(Tuple<Elements...>& t) {
    return makeTupleFromArrayImpl<N,arr>(t, std::make_index_sequence<N>{});
}



// 统一 `Compare` 接口
struct SmallerThan {
    template<typename T, typename U>
    struct apply {
        static constexpr bool value = (sizeof(T) < sizeof(U));
    };
};

struct Smaller {
    template<typename T, typename U>
    bool operator()(const T& t, const U& u) const {
        return sizeof(T) < sizeof(U);
    }
};



auto sortTuple(Tuple<>&t)
{
    return t;
}

template<typename T, typename U>
auto sortTuple(Tuple<T, U>& t);

template<typename T>
auto sortTuple(Tuple<T>& t)
{
    return t;
}

// template<typename Compare,typename... Elements>
// auto sortTuple(Tuple<Elements...>& t)
// {
//     int length = sizeof...(Elements);
//     for (int i = 0; i < length ; i++) {
//         for (int j = i + 1; j < length; j++) {
//             if(Compare(get<i>(t),get<j>(t))){
//                 auto tmp = swapTupleByIndicies<i,j>(t);
//                 return sortTuple<Compare>(tmp);
//             }
//         }
//     }
// }

// 处理多个元素的元组
// template<typename Compare, typename... Elements>
// auto sortTuple(Tuple<Elements...>& t) {
//     constexpr int length = sizeof...(Elements);

//     for (int i = 0; i < length - 1; i++) {
//         for (int j = i + 1; j < length; j++) {
//             // 运行时访问元组元素
//             auto& elem1 = getTupleElement<i>(t);
//             auto& elem2 = getTupleElement<j>(t);

//             // 使用 Compare 进行比较
//             if (Compare()(elem2, elem1)) {  // 这里 Compare 需要是可调用对象
//                 t = swapTupleByIndicies<i, j>(t);
//             }
//         }
//     }
//     return t;
// }



#endif


