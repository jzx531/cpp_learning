#ifndef TUPLEINSERTIONSORT_H
#define TUPLEINSERTIONSORT_H

#include "tuple.h"
#include "tuple_impl.h"
#include "ifThenElse.h"
#include "identity.h"

// 插入排序辅助
template<typename Tuple, typename Element, template<typename T, typename U> class Compare, bool = IsEmpty<Tuple>::value>
class InsertSortedT;

template<typename Tuple, typename Element, template<typename T, typename U> class Compare>
class InsertSortedT<Tuple, Element, Compare, false> {
    using NewTail =typename ifThenElse<Compare<Element, Front<Tuple>>::value,IdentityT<Tuple>,InsertSortedT<PopFront<Tuple>, Element, Compare>>::Type;
    using NewHead = ifThenElse<Compare<Element, Front<Tuple>>::value, Element, Front<Tuple>>;

public:
    using Type = PushFront<NewTail,NewHead>;
};

// 终止条件
template<typename Tuple, typename Element, template<typename T, typename U> class Compare>
class InsertSortedT<Tuple, Element, Compare, true> : public PushFrontT<Tuple, Element> {};


template<typename List, typename Element,
template<typename T, typename U> class Compare>
using InsertSorted = typename InsertSortedT<List, Element, Compare>::Type;


// 递归 InsertionSortT
template<typename Tuple, template<typename T, typename U> class Compare, bool = IsEmpty<Tuple>::value>
class InsertionSortT;


template<typename Tuple, template<typename T, typename U> class Compare>
using InsertionSort = typename InsertionSortT<Tuple, Compare>::Type;

// 递归排序
template<typename Tuple, template<typename T, typename U> class Compare>
class InsertionSortT<Tuple, Compare, false> : public InsertSortedT<InsertionSort<PopFront<Tuple>, Compare>, Front<Tuple>, Compare>
{};

// 终止条件
template<typename Tuple, template<typename T, typename U> class Compare>
class InsertionSortT<Tuple, Compare, true> {
public:
    using Type = Tuple;
};

// 修正 TupleInsertionSort 使其正确匹配参数
template<typename Tuple, template<typename T, typename U> class Compare>
auto TupleInsertionSort(Tuple& t) {
    return InsertionSort<Tuple, Compare>{};
}

// 比较器
template<typename T, typename U>
struct TupleSmaller {
    static constexpr bool value = sizeof(T) < sizeof(U);
};




#endif // TUPLEINSERTIONSORT_H

