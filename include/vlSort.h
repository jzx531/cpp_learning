// #ifndef VLSORT_H
// #define VLSORT_H

// #include "tuple.h"
// #include "copycounter.h"
// #include "indexlist.h"
// #include "tuple_impl.h"
// #include "Tupleinsertionsort.h"
// #include "ifThenElse.h"

// // 插入排序辅助模板
// template<typename List, unsigned Element, template<unsigned, unsigned> class Compare, bool = (List::length == 0)>
// struct InsertSortedVT;

// // 处理非空列表的插入逻辑
// template<typename List, unsigned Element, template<unsigned, unsigned> class Compare>
// struct InsertSortedVT<List, Element, Compare, false> {
//     static constexpr unsigned Head = FrontVT<unsigned, List>::value;
//     using NewTail = ifThenElse<
//         Compare<Element, Head>::value, 
//         List,  // 如果 Element 比 Head 小，则直接插入
//         typename InsertSortedVT<PopFrontV<unsigned, List>, Element, Compare>::type>;

//     using type = typename PushFrontVT<unsigned, 
//     ifThenElseV<unsigned, Compare<Element, Head>::value, Element, Head>, 
//         NewTail>::type;
// };

// // 处理空列表的终止条件
// template<typename List, unsigned Element, template<unsigned, unsigned> class Compare>
// struct InsertSortedVT<List, Element, Compare, true> {
//     using type = PushFrontV<unsigned, Element, List>;
// };

// // 插入排序主模板
// template<typename List, template<unsigned, unsigned> class Compare, bool=(List::length == 0)>
// struct InsertionSortVT;

// // 递归排序逻辑
// template<typename List, template<unsigned, unsigned> class Compare>
// struct InsertionSortVT<List, Compare, false> {
//     using type = typename InsertSortedVT<
//         typename InsertionSortVT<PopFrontV<unsigned, List>, Compare>::type,
//         FrontVT<unsigned, List>::value,
//         Compare>::type;
// };

// // 终止条件：空列表
// template<typename List, template<unsigned, unsigned> class Compare>
// struct InsertionSortVT<List, Compare, true> {
//     using type = List;
// };

// // 简化别名
// template<typename List, template<unsigned, unsigned> class Compare>
// using InsertionSortV = typename InsertionSortVT<List, Compare>::type;

// #endif

#ifndef VLSORT_H
#define VLSORT_H

#include "tuple.h"
#include "copycounter.h"
#include "indexlist.h"
#include "tuple_impl.h"
#include "Tupleinsertionsort.h"
#include "ifThenElse.h"

// 插入排序辅助模板
template<typename List, typename T, T Element, template<T, T> class Compare, bool = (List::length == 0)>
struct InsertSortedVT;

// 处理非空列表的插入逻辑
template<typename List, typename T, T Element, template<T, T> class Compare>
struct InsertSortedVT<List, T, Element, Compare, false> {
    static constexpr T Head = FrontVT<T, List>::value;
    using NewTail = ifThenElse<
        Compare<Element, Head>::value, 
        List,  // 如果 Element 比 Head 小，则直接插入
        typename InsertSortedVT<PopFrontV<T, List>, T, Element, Compare>::type>;

    using type = typename PushFrontVT<T, 
    ifThenElseV<T, Compare<Element, Head>::value, Element, Head>, 
        NewTail>::type;
};

// 处理空列表的终止条件
template<typename List, typename T, T Element, template<T, T> class Compare>
struct InsertSortedVT<List, T, Element, Compare, true> {
    using type = PushFrontV<T, Element, List>;
};

// 插入排序主模板
template<typename List, typename T, template<T, T> class Compare, bool=(List::length == 0)>
struct InsertionSortVT;

// 递归排序逻辑
template<typename List, typename T, template<T, T> class Compare>
struct InsertionSortVT<List, T, Compare, false> {
    using type = typename InsertSortedVT<
        typename InsertionSortVT<PopFrontV<T, List>, T, Compare>::type,
        T,
        FrontVT<T, List>::value,
        Compare>::type;
};

// 终止条件：空列表
template<typename List, typename T, template<T, T> class Compare>
struct InsertionSortVT<List, T, Compare, true> {
    using type = List;
};

// 简化别名
template<typename List, typename T, template<T, T> class Compare>
using InsertionSortV = typename InsertionSortVT<List, T, Compare>::type;

#endif
