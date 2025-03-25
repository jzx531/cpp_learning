#ifndef IFTHENELSE_H
#define IFTHENELSE_H

template<bool COND,typename TrueType,typename FalseType>
struct ifThenElseT{
    using Type = TrueType;
};

template<typename TrueType,typename FalseType>
struct ifThenElseT<false,TrueType,FalseType>{
    using Type = FalseType;
};

template<bool COND,typename TrueType,typename FalseType>
using ifThenElse = typename ifThenElseT<COND,TrueType,FalseType>::Type;


//valuelist
template<typename T,bool cond,T truevalue,T falsevalue>
struct ifThenElseVT{
    static constexpr T value = truevalue;
};

template<typename T,T truevalue,T falsevalue>
struct ifThenElseVT<T,false,truevalue,falsevalue>{
    static constexpr T value = falsevalue;
};

template<typename T,bool cond,T truevalue,T falsevalue>
constexpr T ifThenElseV = ifThenElseVT<T,cond,truevalue,falsevalue>::value;


#endif

 
