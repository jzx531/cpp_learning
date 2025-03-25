#ifndef TUPLE_H
#define TUPLE_H

#include <iostream>

// Tuple 定义
template<typename... Args>
class Tuple;

template<typename T, typename... Args>
class Tuple<T, Args...> {
private:
    T head;
    Tuple<Args...> tail;

public:
    using TailType = Tuple<Args...>;
    Tuple() : head(), tail() {}
    Tuple(T head, Tuple<Args...> tail) : head(head), tail(tail) {}
    Tuple(T head, Args... args) : head(head), tail(args...) {}
    Tuple(const Tuple<T, Args...>& t) : head(t.getHead()), tail(t.getTail()) {}
    T& getHead() { return head; }
    T getHeadNoRef() const { return head; }
    const T& getHead() const { return head; }

    Tuple<Args...>& getTail() { return tail; }
    Tuple<Args...> getTailNoRef() const { return tail; }
    const Tuple<Args...>& getTail() const { return tail; }

    void printTuple() const {
        std::cout << head << " ";
        tail.printTuple();
    }

    int getLength() const {
        return 1 + tail.getLength();
    }

    void printType() const {
        std::cout << typeid(T).name() << " ";
        tail.printType();
    }
};

template<>
class Tuple<> {
public:
    void printTuple() const {
        std::cout << '\n';
    }
    int  getLength() const {
        return 0;
    }
    void printType() const {
        std::cout << '\n';
    }
};

bool operator==(const Tuple<>&, const Tuple<>&) {
    return true;
}

template<typename T, typename... Args>
bool operator==(const Tuple<T, Args...>& t1, const Tuple<T, Args...>& t2) {
    return t1.getHead() == t2.getHead() && t1.getTail() == t2.getTail();
}

template<typename T, typename... Args>
bool operator!=(const Tuple<T, Args...>& t1, const Tuple<T, Args...>& t2) {
    return!(t1 == t2);
}


template<typename... Args>
constexpr bool is_empty(Tuple<Args...>) {
    return false;
}

// Corrected function template specialization for an empty Tuple
template<>
constexpr bool is_empty(Tuple<>) {
    return true;
}

// 判断是否为空的 Tuple
template<typename Tuple>
struct IsEmpty {
    static constexpr bool value = false;
};

template<>
struct IsEmpty<Tuple<>> {
    static constexpr bool value = true;
};



template<typename Tuple>
class FrontT;

// template<>
// class FrontT<Tuple<>> {
//     public:
//         using Type = void;
// };

template<typename T>
class FrontT<Tuple<T>> {
    public:
        using Type = T;
};

template<typename T, typename... Args>
class FrontT<Tuple<T, Args...>> {
public:
    using Type = T;
};

template <typename Tuple>
using Front = typename FrontT<Tuple>::Type;

// PushFront
template<typename T, typename Tuple>
struct PushFrontT;

template<typename T, typename... Tail>
struct PushFrontT<Tuple<Tail...>,T> {
    using Type = Tuple<T,Tail...>;
};

template<typename T>
struct PushFrontT<Tuple<>,T> {
    using Type = Tuple<T>;
};

template<typename Tuple,typename T>
using PushFront = typename PushFrontT<Tuple,T>::Type;

// makeTuple 辅助函数
// template<typename... Args>
// Tuple<Args...> makeTuple(Args... args) {
//     return Tuple<Args...>(args...);
// }

template<typename... Types>
auto makeTuple(Types&&... args) {
    return Tuple<std::decay_t<Types>...>(std::forward<Types>(args)...);
}

// template<typename... Types>
// auto makeTuple(Types&... args) {
//     return Tuple<std::decay_t<Types>...>(args...);
// }

template<typename... Types, typename V>
Tuple<V,Types...> push_front(V& v, Tuple<Types...>& t) {
  return Tuple<V,Types...>(v,t);
}

template<typename V>
Tuple<V> push_back(Tuple<>& t,V& v) {
    return Tuple<V>(v);
}

template<typename Head, typename... Types, typename V>
Tuple<Head,Types..., V> push_back(Tuple<Head,Types...>& t, V& v) {
  return Tuple<Head,Types..., V>(t.getHead(), push_back(t.getTail(), v));
}

//反转

// PushBackT元函数：用于类型推导，将类型V添加到Tuple末尾
template<typename Tup, typename V>
struct PushBackT;

template<typename... Types, typename V>
struct PushBackT<Tuple<Types...>, V> {
    using Type = Tuple<Types..., V>;
};

// Reverse元函数：递归计算反转后的Tuple类型
template<typename T>
struct Reverse;

template<>
struct Reverse<Tuple<>> {
    using Type = Tuple<>;
};

template<typename Head, typename... Tail>
struct Reverse<Tuple<Head, Tail...>> {
private:
    using ReversedTail = typename Reverse<Tuple<Tail...>>::Type;
public:
    using Type = typename PushBackT<ReversedTail, Head>::Type;
};

template<typename... Args>
using  ReverseT = typename Reverse<Args...>::type;

Tuple<> reverse(const Tuple<>&t) {
    return t;
}

// recursive case
template<typename Head, typename... Tail>
Reverse<Tuple<Head, Tail...>>::Type reverse(Tuple<Head, Tail...> & t)
{
  auto reversed_tail = reverse(t.getTail()); // 递归反转尾部
  return push_back(reversed_tail, t.getHead()); // 将当前头部追加到反转后的尾部
}

// popback
template<typename T, typename... Args>
class PopBackT;

template<typename T, typename... Args>
class PopBackT<Tuple<T,Args...>>{
    public:
        using Reversed = typename Reverse<Tuple<T,Args...>>::Type;
        using ReversedTail = Reversed::TailType;
        using Type = Reverse<ReversedTail>::Type;
};

template<typename T, typename V>
class PopBackT<Tuple<T, V>> {
public:
    using Type = Tuple<T>;
};


template<typename T>
class PopBackT<Tuple<T>>{
    public:
        using Type = Tuple<>;
};

Tuple<> popBack(Tuple<>& t)
{
    return t;
}


template<typename T,typename... Args>
typename PopBackT<Tuple<T,Args...>>::Type popBack(Tuple<T,Args...>& t)
{
    using ReversedTuple = typename Reverse<Tuple<T,Args...>>::Type;
    ReversedTuple reversed = reverse(t);  // 反转整个 Tuple
    auto tail = reversed.getTail(); // 这里返回的是 Tuple<Args..., T>
    return reverse(tail); // 再次反转，确保返回类型正确
}


//popfront
template<typename Head, typename... Tail>
class PopFrontT;
template<typename Head, typename... Tail>
struct PopFrontT<Tuple<Head, Tail...>> {
    using Type = Tuple<Tail...>;
};

template<typename Tuple>
using PopFront = typename PopFrontT<Tuple>::Type;


Tuple<> popFront(Tuple<>& t)
{
    return t;
}

template<typename T, typename... Args>
typename PopFrontT<Tuple<T, Args...>>::Type popFront(Tuple<T, Args...>& t)
{
    return t.getTail();
}


// tie 定义
template<typename... Args>
class tie;

template<typename T, typename... Rest>
class tie<T, Rest...> {
private:
    T& head;
    tie<Rest...> tail;

public:
    tie(T& head, Rest&... rest) : head(head), tail(rest...) {}

    tie(Tuple<T, Rest...>& t) : head(t.getHead()), tail(t.getTail()) {}
    void operator=(const Tuple<T, Rest...>& t) {
        head = t.getHead();
        tail = t.getTail();
    }
};

template<>
class tie<> {
public:
    tie() {}  // 默认构造函数
    void operator=(const Tuple<>&) {}
};

// make_tie 辅助函数，用于模板参数推导
template<typename... Args>
tie<Args...> make_tie(Args&... args) {
    return tie<Args...>(args...);
}

// get
template<unsigned N>
struct TupleGet{
    template<typename Head, typename... Tail>
    static auto& apply(Tuple<Head, Tail...>& t) {
        return TupleGet<N-1>::apply(t.getTail());
    }
};

template<>
struct TupleGet<0>{
    template<typename Head, typename... Tail>
    static Head& apply(Tuple<Head, Tail...>& t) {
        return t.getHead();
    }
};

template<unsigned N,typename... Args>
auto& getTupleElement(Tuple<Args...>& t){
    return TupleGet<N>::apply(t);
}

#define get getTupleElement

#endif // TUPLE_H


