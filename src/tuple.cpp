#include "tuple.h"
#include "copycounter.h"
#include "indexlist.h"
#include "tuple_impl.h"
#include "Tupleinsertionsort.h"
#include "tuplesort.h"
#include "vlSort.h"
#include <iterator>
template<typename T, std::size_t... Indices>
void func(T (&arr)[sizeof...(Indices)], std::index_sequence<Indices...>) {
    ((std::cout << arr[Indices] << " "), ...);
}

template<unsigned A, unsigned B>
struct Ascending {
    static constexpr bool value = A < B;
};

template<int A, int B>
struct Ascendingint {
    static constexpr bool value = A < B;
};

// 测试代码
int main() {
    Tuple<int, double, char> t(1, 2.2, 'a');
    // auto t2 = makeTuple(1, 2.2, 'a');
    // t2.printTuple();
    t.printTuple();

    Tuple<CopyCounter<0>, CopyCounter<1>, CopyCounter<2>> copies;
    // auto reversed = reverse(copies);
    auto reversed =makeTuple (get<2>(copies));
    // auto reversed = makeTuple(get<2>(copies), get<1>(copies), get<0>(copies));
    std::cout<<"0:\t"<<CopyCounter<0>::numCopies<<std::endl;
    std::cout<<"1:\t"<<CopyCounter<1>::numCopies<<std::endl;
    std::cout<<"2:\t"<<CopyCounter<2>::numCopies<<std::endl;
    
    using MyList = ValueList<int, 1, 2, 3>;
    MyList l1;
    l1.print_values(); // Output: 1 2 3

    using NewList = PushFrontV<int, 0, MyList>;
    NewList l2;
    l2.print_values(); // Output: 0 1 2 3

    auto t3 = reverse_tuple(t);
    t3.printTuple(); // Output: (1, 2.2, 'a')

    ValueList<unsigned,0,2> ml2;
    auto t4 = select(t,ml2);
    t4.printTuple(); // Output: (1, 'a')    

    auto t5 = splat<1,2>(t);
    t5.printTuple(); // Output: (1, 2.2)
    
    auto t6= swapTupleByIndicies<1,2>(t);
    t.printType(); // Output: Tuple<int, double, char>
    // auto t6 = push_back(t, t.getHead());
    t6.printTuple(); // Output: (1, 2.2, 'a', 1, 2.2, 'a')
    t6.printType(); // Output: Tuple<int, double, char, int, double, char>
    
    
    using ST = InsertionSort<Tuple<int, char,short, double>, TupleSmaller>;
    std::cout<<std::is_same<ST, Tuple<char, short, int, double>>::value<<std::endl;
    
    Tuple<int, char, short, double> tt(1, 'a', 2, 3.3);
    auto t7 = TupleSort<TupleSmaller>(tt);
    t7.printTuple(); // Output: (1, 2.2, 'a')

    auto vlt = ValueList<unsigned ,1,5,2,3,4>();
    vlt.print_values(); // Output: 1 5 2 3 4
    using list = ValueList<int, 1,5,2,3,4>;
    using vlts = InsertionSortV<list,int,Ascendingint>;
    vlts vv;
    vv.print_values(); // Output: 1 2 3 4 5
    return 0;
}