#ifndef TUPLESORT_H
#define TUPLESORT_H

#include "tuple.h"
#include "copycounter.h"
#include "indexlist.h"
#include "tuple_impl.h"
#include "Tupleinsertionsort.h"
#include "vlSort.h"

template<typename T,T v>
struct CTValue{
    static constexpr T value = v;
};

template<unsigned N, typename Tuple>
struct NthElementT;

template<typename Head, typename... Tail>
struct NthElementT<0, Tuple<Head, Tail...>> {
    using type = Head;
};

template<unsigned N, typename Head, typename... Tail>
struct NthElementT<N, Tuple<Head, Tail...>> {
    using type = typename NthElementT<N - 1, Tuple<Tail...>>::type;
};

template<typename Tuple, unsigned N>
using NthElement = typename NthElementT<N, Tuple>::type;

// template<typename Tuple, template<typename T, typename U> class Compare>
// class SortNthElementT {
// public:
//     template<typename T, typename U>
//     class Apply;

//     template<unsigned M, unsigned N>
//     class Apply<CTValue<unsigned, M>, CTValue<unsigned, N>>
//         : public Compare<NthElement<Tuple, M>, NthElement<Tuple, N>> {};
// };

template<typename Tuple, template<typename T, typename U> class Compare>
class SortNthElementT {
public:
    template<unsigned M, unsigned N>
    struct Apply : Compare<NthElement<Tuple, M>, NthElement<Tuple, N>> {};
};


template<template<typename T, typename U> class Compare, typename... Elements>
auto TupleSort(Tuple<Elements...>& t) {
    using SortedIndexList = InsertionSortV<
        MakeIndexList<sizeof...(Elements)>,
        unsigned,
        SortNthElementT<Tuple<Elements...>, Compare>::template Apply
    >;
    
    return select(t, SortedIndexList{});
}


#endif