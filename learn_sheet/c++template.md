# c++template

[TOC]

模板可以作为元函数的输入参数
```c++
template<template<typename> class T,typename U>
struct Fun_{
    using type = typename T<U>::type;
};

template<template<typename>class T,typename U>
using Fun = typename Fun_<T,U>::type;

Fun<std::remove_reference,int &> h=3;
```

模板作为元函数的输入
```c++
template<bool AddOrRemoveRef> struct Fun_;
template<> struct Fun_<true>{
    template<typename T> using type = std::add_lvalue_reference_t<T>;
};

template<> struct Fun_<false>{
    template<typename T> using type = std::remove_reference_t<T>;
};

template<typename T>
using Res_ = Fun_<false>;

Res_<int>::type a=3;
```

C++11中引入了变长参数模板(variadic template)使用它可以很容易地实现我们需要的容器
```c++
template<int...Vals>struct IntContainer;
template<bool...Vals>struct BoolContainer;
template<template<typename>class ...T> struct TemplateCont;
template<template<typename ...>class...T>struct TemplateCont2;
```

顺序执行的代码
```c++
template<typename T>
struct RemoveReferenceConst{
    private:
        using inter_type = typename std::remove_reference<T>::type;
    public:
        using type=typename std::remove_const<inter_type>::type;
}

template<typename T>
using RemoveReferenceConst_t=typename RemoveReferenceConst<T>::type;

RemoveReferenceConst_t<int const &> a=3;
```
使用std::conditional与std::conditional_t实现分支
```c++
namespace std{
    template<bool B,typename T,typename F>
    struct conditional{
        using type = T;
    }
};

template<typename T,typename F>
struct conditional_t<false,T,F>{
    using type = F;
};

template<bool B,typename T,typename F>
using conditional_t=typename conditional<B,T,F>::type;

```

逻辑行为是：
- 如果B为true，则返回T
- 如果B为false，则返回F

使用(部分)特化实现分支
```c++
struct A;struct B;
template<typename T>
struct Fun_{
    constexpr static size_t value = 0;
};

template<>
struct Fun_<A>{
    constexpr static size_t value =1;
};

template<>
struct Fun_<B>{
    constexpr static size_t value =2;
};

constexpr size_t h = Fun_<B>::value;
```

```c++
struct A; 
struct B;

template <typename T>
constexpr size_t Fun = 0;

template <>
constexpr size_t Fun<A> = 1;

template <>
constexpr size_t Fun<B> = 2;

constexpr size_t h = Fun<B>;
```
<strong>使用特化来实现分支时,有一点需要注意:在非完全特化的类模板中引入完全特化的分支代码是非法的</strong>

为了解决这个问题,使用部分特化来代替完全特化,将上面的代码修改如下:
```c++
template<typename TW>
struct Wrapper{
    template<typename T,typename TDummy=void>
    struct Fun_{
        constexpr static size_t value = 0;
    };
    template<typename Dummy>
    struct Fun_<int,Dummy>{
        constexpr static size_t value = 5;
    };
};
```

这里使用std::enable_if与std::enable_if_t来实现分支
```c++
namespace std{
    template<bool B,typename T=void>
    struct enable_if{};
    template<class T>
    struct enable_if<true,T>{using type=T};
    template<bool B,class T = void>
    using enable_if_t = typename enable_if<B,T>::type;
}
```
```c++
template <bool IsFeedbackOut, typename T,
    std::enable_if_t<IsFeedbackOut>* = nullptr>
auto FeedbackOut_(T&&) { /* ... */ }

template <bool IsFeedbackOut, typename T,
    std::enable_if_t<!IsFeedbackOut>* = nullptr>
auto FeedbackOut_(T&&) { /* ... */ }
```

这里引入了一个分支,当IsFeedbackOut为真时,std::enable_if_t<IsFeedbackOut>* ::type是有意义的

c++具有SFINAE(Substitution Failure Is Not An Error)机制,即当模板参数不满足时,编译器不会报错,而是会退化到下一个最匹配的模板,因此,在模板参数不满足时,可以用std::enable_if来实现分支,从而避免编译错误
```c++
template<bool Check,std::enable_if_t<check>*=nullptr>
auto fun(){
    return (int)0;
}

template<bool Check,std::enable_if_t<!check>*=nullptr>
auto fun(){
    return (double)0;
}
template<bool check>
auto wrap2(){
    return fun<check>();
}
```
奇特的递归模板式(CRTP)
派生类会把本身作为模版参数传递给基类
```c++
template<typename T>
class Base;

template<typename T>
class Derived:public Base<Derived<T>>;
```

<strong>函数模板以及类的静态函数无法被声明为虚函数</strong>,此时借助CRTP可以达到类似虚函数的效果

```c++
template <typename T>
struct Base
{
    template <typename TI>
    void Fun(const TI& input){
        D* ptr = static_cast<D*>(this);
        ptr -> Impl(input);
    }
};

struct Derive : public Base<Derive>
{
    template <typename TI>
    void Impl(const TI& input)
    {
        std::cout << "Derive::Impl(" << input << ")" << std::endl;  
    }
};

int main()
{
    Derive d;
    d.Fun(10);
    return 0;
}
```

```c++
template <typename T>
struct Base
{
    static void Fun()
    {
        D::Impl();
    }
};

struct Derive : public Base<Derive>
{
    static void Impl(){
        std::cout<<"Implementation from derive class"<<std::endl;
    }
};

int main()
{
    Derive::Fun();
    return 0;
}
```

## 异类词典与policy模板

异类词典的实现

```c++
template<typename...TParameters>
struct VarTypeDict
{
    template<typename...TTypes>
    struct Values{
        public:
            template<typename TTaq,typename TVal>
            auto Set(TVal &&) &&;
            template<typename TTaq>
            const auto& Get() const;
    };

    public:
        static auto Create(){
        using namespace NSVarTypeDict;
        using type = typename Create_<sizeof...(TParameters),Values>::type;
        return type{};
        }
};
```

Create()会根据VarTypeDict传入的模板参数构造一个类型type,之后返回这个类型所对应的对象
Create返回的对象实际上是Values<TTypes...>的实例,Values是位于VarTypeDict内部的一个模板,它提供了Set与Get函数
```c++
std::cerr << fun(FParams::Create().set<B>(2.4f).set<A>("hello").get<A>() << std::endl;);
```
第1行的Create相当于构造了Values<TTypes...>类型的变量,后面几个Set则相当于向Values<TTypes...>中传入数据

VarTypeDict<A,B,C>::Create().set<A>(10).set<B>("hello").set<C>(true)
创建了一个VarTypeDict<A,B,C>::Values<int,std::string,bool>类型的对象,并调用了它的set函数,将传入的参数分别绑定到A,B,C的类型上

Create函数的实现,Create函数是整个模块中首个对外的接口,但这个接口在实现时有一个问题

Create函数调用之处用这个占位符类型填充TTypes,在之后的Set中,再来修改这个类型为实际的类型
```c++
VarTypeDict<A,B>::Create().Set<A>(true).Set<B>(2.4f);
```
```c++
namespace NSVarTypeDict
{
    template<size_t N,template<typename...>class TCont,typename...T>
    struct Create_{
        using type = typename Create_<N-1,TCont,NullParameter,T...>::type;
    };
    
    template<template<typename...>class TCont,typename...T>
    struct Create_<0,TCont,T...>{
        using type = TCont<T...>;
    };
}

template<typename...TParameters>
struct VarTypeDict
{
    static auto Create(){
        using namespace NSVarTypeDict;
        using type = typename Create_<sizeof...(TParameters),Values>::type;
        return type{};
    }
};
```

函数的主体逻辑实际上位于名字空间NSVarTypeDict里面的Create_中
而Create内部调用了元函数Create_，获取它的返回结果(类型),使用该类型构造一个对象并返回

Create_本身实现了一个循环逻辑,它包含了两个部分:
1. 原始模板:
   * N表示还需要构造的元素数目
   * TCont是容器类型,用于存储最终的结果(值是类型的数组)
   * T是已经生成的类型序列
  在内部会构造一个NullParameter的类型并放到类型数组中,将N减1,之后进行下一次迭代
  Create的另一个特化表示N=0的情形，也就是循环终止的情形,此时系统直接返回TCont<T..>这个类型数组
  Create函数内部调用了Create_函数,传入TParameter的大小,同时传入数组容器Values,以保存类型计算的结果
  这里有两点需要注意:首先,Values与Create均定义于VarTypeDict内部,因此在Create中使用Values时,无需指定其外围类VarTypeDict

Values主体框架
```c++
template <typename...TParameters>
struct VarTypeDict
{
    template<typename...TTypes>
    struct Values{
        Values()=default;
        Values(std::shared_ptr<void>(&&input)[sizeof...(TTypes)])
        {
            for(size_t i = 0;i<sizeof...(TTypes);++i)
            {
                m_tuple[i]=std::move(input[i]);
            }
        }

        public:
            template<typename TTaq,typename TVal>
            auto Set(TVal &&val) &&
            {
                using namespace NSVarTypeDict;
                constexpr static size_t TaqPos = Tag2ID<TTaq,TParameters...>;

                using rawVal = std::decay_t<TVal>;
                rawVal*tmp = new rawVal(std::forward<TVal>(val));
                m_tuple[TagPos]= std::shared_ptr<void>(tmp,[](void* ptr){
                    rawVal * nptr = static_cast<rawVal*>(ptr);
                    delete nptr;
                });
                using new_type = NewTupleType<rawVal,TagPos,Values<>,TTypes...>;
                return new_type{std::move(m_tuple)};
            }
            template <typename TTaq>
            auto & Get() const;
        private:
        std::shared_ptr<void>m_tuple[sizeof...(TTypes)];
    };
}
```
Values::Set 是函数模板，它接收两个模板参数，分别表示了键（TTag）与值的类型（TVal）。根据 C++ 中函数模板的自动推导规则，将 TVal 作为第二个模板参数，这样在调用该函数时，只需提供 TTag 的模板实参（编译器可以推导出第二个实参的类型信息）。也即，假定 x 是一个 Values 类型的对象，那么：

x.Set<A>(true);

调用时，TTag 将为 A，TVal 将自动推导为类型 bool。

Values::Set 同样调用了几个位于 NSVarTypeDict 中的元函数来实现内部逻辑。对于传入的参数，它的处理流程如下。

1. 调用 NSVarTypeDict::Tag2ID 获取 TTag 在 TParameters 中的位置，保存于 TagPos 中（第 22 行）。
2. 调用 std::decay 对 TVal 进行处理，用于去除 TVal 中包含的 const、引用等修饰符。之后使用这个新类型在堆中构造一个输入参数的复本，并将该复本放置到 m_tuple 相应的位置上（第 24–30 行）。
3. 因为传入了新的参数，所以更新地修改 Values 中的 TType 类型，调用 NSVarTypeDict::NewTupleType 获取新的类型，并使用这个新的类型构造新的对象并返回（第 32–33 行）。

NewTupleType 逻辑分析
NewTupleType调用了NewTupleType_来实现其逻辑,而NewTupleType_声明如下(与Create_类似)这个函数也是依次对数组中的每个元素进行处理
```c++
template<typename TVal,size_t N,size_t M,typename TProcessedTypes,typename...TRemainTypes>
struct NewTupleType_;
```
其中TVal为替换的目标数据类型，N表示目标类型在类型数组中的位置；TProcessedTypes为一个数组容器,其中包含了已经完成扫描的部分;而TRemainTypes中包含了还需要进行扫描替换的部分；M是一个辅助变量，表示已经扫描的类型个数

除了上述声明,NewTupleType_一共提供了两个特化版本，它们共同组成了一个循环处理的逻辑。第一个特化版本扫描数组的前半部分,如下:
```c++
template<typename TVal,size_t N,size_t M,template<typename...>class TCont,typename TCurType,
typename...TRemainTypes>
struct NewTupleType_<TVal,N,M,TCont<TModifiedTypes...>,TCurType,TRemainTypes...>
{
    using type = typename NewTupleType_<TVal,N,M+1,
    TCont<TModifiedTypes...,TCurType>,TRemainTypes...>::type;
};
```

它描述的是N！= M的情况,该特化使用TCont<TModifiedTypees...>来表示已经完成替换扫描的类型;使用TCurType与TRemainTypes一起表示未替换扫描的类型(其中TCurType表示当前处理的类型)因为N！=M,所以只需要简单地将TCurType放入TCont容器中,继续处理下一个类型
如果N=M，那么编译器将采用下一个特化:
```c++
template<typename TVal,size_t N,template<typename...>class TCont,typename...TModifiedTypes,typename TCurType,typename...TRemainTypes>
struct NewTupleType_<TVal,N,N,TCont<TModifiedTypes...>,TCurType,TRemainTypes...>
{
    using type = TCont<TModifiedTypes...,TVal,TRemainTypes...>;
};
```
NewTupleType调用NewTupleType_来实现逻辑只是一个外壳
```c++
template<typename TVal,size_t TagPos,typename TCont,typename...TRemainTypes>
using NewTupleType = typename NewTupleType_<TVal,TagPos,0,TCont,TRemainTypes...>::type;
```

使用std::tuple作为缓冲
```c++
std::tuple<TType...>m_tuple;
```

### policy模板

```c++
template <typename TAccuType> struct Accumulator{/**/};
```
上式中,TAccuType表示采用的"累积"策略,在Accumulator内部,可以根据这个参数的值选择适当的处理逻辑

```c++
template <typename TAccuType,bool DoAve,typename ValueType> struct Accumulate{/**/};
```
每个policy都表现为键值对,其中键与值都是编译期常量,每个policy都有其取值集合
```c++
template <typename TAccuType=Add,bool DoAve = false,typename ValueType = double>
struct Accumulate{/****/};

//或者
Accumulate<Add,false,double>
```
C++不直接支持具名的模板参数,上述语句不符合c++标准会导致编译错误

policy对象
每个policy对象都属于某个policy,它们之间的关系就像c++中的对象与类那样,policy对象是编译期的常量,其中包含了键与值的全部信息

本书中定义的policy对象以大写字母P开头.使用者可以根据其名称,一目了然地明确该对象所描述的policy含义,对于支持policy对象的模板,可以非常容易改变其默认行为

```c++
Accumulator<PDoubleValueType>
Accumulator<PDoubleValueType,PAve>
Accumulator<PDoubleValueType,PAve,PIntValueType>
```
policy对象的构造与使用是分离的.我们需要首先构造出某个policy对象(比如PAve),并在随后声明Accumulator的实例时使用该对象(比如Accumulator<PAve>)

构造policy对象模板来表示保存计算结果的类型
```c++
PValueTypeIs<typename T>
```
用户可以按照如下方式来使用该模板
```c++
//等价于Accumulator<PDoubleValueType>
Accumulator<PValueTypeIs<double>>
//等价于Accumulator<PDoubleValueType,PAve>
Accumulator<PValueTypeIs<double>,PAve>
```
使用policy对象模板,就将构造policy对象的时机移到了policy的使用之处,这样就无需为使用policy而提前准备大量的policy对象了
有了policy对象之后,使用该对象的函数模板与类模板被称为policy模板

定义policy与policy对象(模板)
policy分组

为了描述policy对象所属的组以及互斥性，我们为其引入了两个属性:major class主要类别表示其所属的组,而minor class次要类别描述了互斥信息,如果两个policy对象的major class与minor class 均相同,那么二者是互斥的,不能被同时使用

因为policy对象将会参与到元函数的计算过程中,而c++中操作名字空间的元编程方法并不成熟;

以下是一个简单的policy组的示例
```c++
struct AccPolicy
{
    struct AccuTypeCate
    {
        struct Add;
        struct Mul;
    };
    using Accu = AccuTypeCate::Add;
    static constexpr bool IsAve = false;
    struct ValueTypeCate;
    using Value = float;
};
```
Values::Set 是函数模板，它接收两个模板参数，分别表示了键（TTag）与值的类型（TVal）。根据 C++ 中函数模板的自动推导规则，将 TVal 作为第二个模板参数，这样在调用该函数时，只需提供 TTag 的模板实参（编译器可以推导出第二个实参的类型信息）。也即，假定 x 是一个 Values 类型的对象，那么：

x.Set<A>(true);


调用时，TTag 将为 A，TVal 将自动推导为类型 bool。

Values::Set 同样调用了几个位于 NSVarTypeDict 中的元函数来实现内部逻辑。对于传入的参数，它的处理流程如下。

1. 调用 NSVarTypeDict::Tag2ID 获取 TTag 在 TParameters 中的位置，保存在 TagPos 中（第 22 行）。

2. 调用 std::decay 对 TVal 进行处理，用于去除 TVal 中包含的 const、引用等修饰符。之后使用这个新类型在堆中构造一个输入参数的复本，并将该复本放置到 m_tuple 相应的位置上（第 24–30 行）。

3. 因为传入了新的参数，所以更新地修改 Values 中的 TType 类型，调用 NSVarTypeDict::NewTupleType 获取新的类型，并使用这个新的类型构造新的对象并返回（第 32–33 行）。

* 对于类型policy,其minor class 为键名字加上TypeCate后缀
* 对于值policy,其minor class 为键名字加上ValueCate后缀

本书引入了4个宏:
* TypePolicyObj用于定义类型policy对象
* ValuePolicyObj用于定义数值policy对象
* TypePolicyTemplate用于定义类型policy对象模板
* ValuePolicyTemplate用于定义数值policy对象模板

使用policy
在定义了policy之后,就可以使用
```c++
template<typename...TPolicies>
struct Accumulator
{
    using TPoliCont = PolicyContainer<TPolicies...>;
    using TPolicyRes = PolicySelect<AccPolicy,TPoliCont>;
    using ValueType = typename TPolicyRes::Value;
    static ValueType = typename TPolicyRes::IsAve;
    static constexpr bool is_ave = TPolicyRes::IsAve;
    using AccuType = typename TPolicyRes::Accu;

    public:
    template<typename TIn>
    static auto Eval(const TIn& in)
    {
        if constexpr(std::is_same<AccuType,AccuPolicy::AccuTypeCate::Add>::value)
        {
            ValueType count =0;
            ValueType res =0;
            for(const auto& x:in)
            {
                res+=x;
                count++
            }
            if constexpr(is_ave)
            {
                return res/count;
            }
            else
            {
                return res;
            }
        }
        else if constexpr(std::is_same<AccuType,AccuPolicy::AccuTypeCate::Mul>::value)
        {
            ValueType res =1;
            ValueType count =0;
            for(const auto& x:in)
            {
                res*=x;
                count++
            }
            if constexpr(is_ave)
            {
                return pow(res,1.0/count);
            }
            else
            {
                return res;
            }
        }
        else
        {
            static_assert(DependencyFalse<AccuType>);
        }
    }
};

int main()
{
    TypePolicyObj(PAddAccu, AccPolicy, Accu, Add);
    TypePolicyObj(PMulAccu, AccPolicy, Accu, Mul);
    ValuePolicyObj(PAve, AccPolicy, IsAve, true);
    ValuePolicyObj(PNoAve, AccPolicy, IsAve, false);
    TypePolicyTemplate(PValueTypeIs, AccPolicy, Value);
    ValuePolicyTemplate(PAvePolicyIs, AccPolicy, IsAve);

}
```

Accumulator是一个接收policy的类模板,它提供了静态函数Eval来计算累积结果

背景知识:支配与虚继承

在了解了支配与继承的关系后,policy对象之所以能改变默认的policy值,实际上是因为它继承了定义的policy类,并在其自身定义中改变了原来的policy值,即形成了支配关系
比如,在给定AccPolicy的基础上,可以这样定义PMulAccu
```c++
struct AccPolicy{
    struct AccuTypeCate{
        struct Add;
        struct Mul;
    };
    using Accu = AccuTypeCate::Add;
}

struct PMulAccu : virtual public AccPolicy{
    using MajorClass = AccPolicy;
    using MinorClass = AccPolicy::AccuTypeCate;
    using Accu = AccuTypeCate::Mul;
};
```
一个policy模板可以接受多个policy对象,而policy模板的行为则是由这些policy对象共同决定的。基于policy模板所接收到的全部policy对象,可以通过元编程的手段构造policy的支配层次结构

policy选择元函数
整个policy模板的对外接口就是policy选择元函数:PolicySelect
```c++
template<typename..TPolicies>
struct Accumulator{
    using TPoliCont = PolicyContainer<TPolicies...>;
    using TPolicyRes = PolicySelect<AccPolicy,TPoliCont>;
    using ValueType = typename TPolicyRes::Value;
    static constexpr bool is_ave = TPolicyRes::IsAve;
    using AccuType = typename TPolicyRes::Accu;
}
```

PolicySelect仅仅是元函数NSPolicySelect::Selector_的封装
```c++
template <typename TMajorClass,typename TPolicyContainer>
using PolicySelect = typename Selector_<TMajorClass,TPolicyContainer>::type;
```
Selector_的定义如下:
```c++
template <typename TMajorClass,typename TPolicyContainer>
struct Selector_;

template <typename TMajorClass,typename...TPolicyContainer>
struct Selector_<TMajorClass,PolicyContainer<TPolicis...>>
{
    using TMF = typename MajorFilter_<PolicyContainer<>,TMajorClass,TPolicies...>::type;
    static_assert(MinorCheck_<TMF>::value,"No policy found");
    using type = std::conditional_t<IsArrayEmpty<TMF>,TMajorClass,PolicySelRes<TMF>>;
};
```
- 调用 MajorFilter 元函数对数组进行过滤，生成新的 PolicyContainer 数组——确保该数组中的所有元素的 major class 均为 TMajorClass（6～8 行）。
- 调用 MinorCheck 元函数检测上一步生成的数组，确保其中的元素不会冲突——即不存在相同 minor class 的 policy 对象（10～11 行）。
- 构造最终的返回类型（13～15 行）。


存储空间的分配与维护

Allocator类模板
```c++
template <typename TDevice>
struct Allocator;
```
它接收一个参数,取值为DeviceTags中定义的某个设备类型,通过特化可以引入不同的Allocator实例
采用设备相关的逻辑进行存储空间的分配与释放

```c++  
template<>
struct Allocator<DeviceTags::CPU>
{
    template<typename TElem>
    static std::shared_ptr<TElem>Allocate(size_t p_elemSize){
        return std::shared_ptr<TElem>(new TElem[p_elemSize],[](TElem* ptr){delete[] ptr;});
    }
};
```

ContinuousMemory类模板
```c++
template <typename TElem,typename TDevice>
class ContinuousMemory{
    static_assert(std::if_same<RemConstRef<TElem>,TElem>::value);
    using ElementType = TElem;
public:
    explicit ContinuousMemory(size_t p_size)
    : m_mem(Allocator<TDevice>::Allocate<ElementType>(p_size)),m_memStart(my_mem.get()){}
    auto RawMemory() const { return m_memStart; }

    const std::shared_ptr<ElementType> SharedPtr() const {
        return m_mem;
    }

    bool operator==(const ContinuousMemory& val) const {
        return (m_mem == val.m_mem) && (m_memStart == val.m_memStart);
    }

    bool operator!=(const ContinuousMemory& val) const {
        return !(*this == val);
    }

    size_t UseCount() const {
        return m_mem.use_count();
    }

    private:
        std::shared_ptr<ElementType> m_mem;
        ElementType* m_memStart;
}
```
为标量引入专门的类模板
```c++
template <typename TElem, typename TDevice = DeviceTags::CPU>
struct Scalar;

template <typename TElem, typename TDevice>
constexpr bool IsScalar<Scalar<TElem, TDevice>> = true;
```


```c++
template <typename TElem, typename TDevice = DeviceTags::CPU>
class Scalar
{
public:
    using ElementType = TElem;
    using DeviceType = TDevice;

public:
    Scalar(ElementType elem = ElementType())
        : m_elem(elem) {}

    auto Value() { return m_elem; }
    auto Value() const { return m_elem; }

    // 乗値相关接口
    bool operator==(const Scalar& val) const;

    template <typename TOtherType>
    bool operator==(const TOtherType&) const;

    template <typename TData>
    bool operator!=(const TData& val) const;

    auto EvalRegister() const;

private:
    ElementType m_elem;
};
```
为标量引入主体类型
```c++
template <typename TElem, typename TDevice>
struct PrincipalDataType_<CategoryTags::Scalar, TElem, TDevice>
{
    using type = Scalar<TElem, TDevice>;
};
```
矩阵
```c++
template<typename TElem, typename TDevice>
class Matrix;

template<typename TElem, typename TDevice>
constexpr bool IsMatrix<Matrix<TElem, TDevice>> = true;

```
```c++
template <typename TElem>
class Matrix<TElem, DeviceTags::CPU>
{
public:
    using ElementType = TElem;
    using DeviceType = DeviceTags::CPU;

public:
    Matrix(size_t p_rowNum = 0, size_t p_colNum = 0);

    // 维度相关接口
    size_t RowNum() const { return m_rowNum; }
    size_t ColNum() const { return m_colNum; }

    // 读写访问接口

    void SetValue(size_t p_rowId, size_t p_colId, ElementType val);
    const auto operator () (size_t p_rowId, size_t p_colId) const;
    bool AvailableForWrite() const;

    // 子矩阵接口
    Matrix SubMatrix(size_t p_rowB, size_t p_rowE,
                     size_t p_colB, size_t p_colE) const;

    // 求值相关接口
    // ...

private:
    Matrix(std::shared_ptr<ElementType> p_mem, ElementType* p_memStart,
           size_t p_rowNum, size_t p_colNum, size_t p_rowLen);

private:
    ContinuousMemory<ElementType, DeviceType> m_mem;
    size_t m_rowNum;
    size_t m_colNum;
    size_t m_rowLen;
};
```
```c++
const auto operator () (size_t p_rowId, size_t p_colId) const {
    assert((p_rowId < m_rowNum) && (p_colId < m_colNum));
    return (m_mem.RawMemory())[p_rowId * m_rowLen + p_colId];
}

bool AvailableForWrite() const {
    return m_mem.UseCount() == 1;
}

void SetValue(size_t p_rowId, size_t p_colId, ElementType val) {
    assert(AvailableForWrite());
    assert((p_rowId < m_rowNum) && (p_colId < m_colNum));
    (m_mem.RawMemory())[p_rowId * m_rowLen + p_colId] = val;
}
```
子矩阵接口
```c++
Matrix SubMatrix(size_t p_rowB, size_t p_rowE,
                 size_t p_colB, size_t p_colE) const
{
    assert((p_rowB < m_rowNum) && (p_colB < m_colNum));
    assert((p_rowE <= m_rowNum) && (p_colE <= m_colNum));

    TElem* pos = m_mem.RawMemory() + p_rowB * m_rowLen + p_colB;
    return Matrix(m_mem.SharedPtr(),
                  pos,
                  p_rowE - p_rowB,
                  p_colE - p_colB,
                  m_rowLen);
}
```

特殊矩阵:平凡矩阵,全零矩阵与独热矩阵
平凡矩阵:矩阵中每个元素取值均相同
```c++
template <typename TElem,typename TDevice,typename TScalar>
class TrivalMatrix
{
    public:
    using ElementType = TElem;
    using DeviceType = TDevice;

    public:
    TrivalMatrix(size_t p_rowNum,size_t p_colNum,TScalar p_val);
    // 维度相关接口
    size_t RowNum() const;
    size_t ColNum() const;
    // 读写访问接口
    auto ElementValue() const;
    //求相关接口
    private:
    TScalar m_val;
    size_t m_rowNum;
    size_t m_colNum;
    //求值结果缓存
    EvalBuffer<Matrix<ElementType,DeviceType>>m_evalBuf;
};
```

在实现了平凡矩阵之后,需要特化IsMatrix将其与MetaNN中的矩阵标签关联起来
```c++
template <typename TElem,typename TDevice>
constexpr bool IsMatrix<TrivalMatrix<TElem,TDevice,int>> = true;
```

直接构造TrivalMatrix对象是比较复杂的,这里提供一个函数用来简化构造过程
```c++
template <typename TElem,typename TDevice,typename TVal>
auto MakeTrivalMatrix(size_t rowNum,size_t colNum,TVal&& m_val)
{
    using RawVal = RemConstRef<TVal>;
    if constexpr(IsScalar<RawVal>)
    {
        //检测RawVal::DeviceType与TDevice相同或者同为CPU
        return TrivalMatrix<TElem,TDevice,RawVal>(rowNum,colNum,m_val);
    }
    else
    {
        TElem tmpElem = static_cast<TElem>(m_val);
        Scalar<TElem,DeviceTags::CPU> scalar(std::move(tmpElem));
        return TrivalMatrix<TElem,DeviceTags::CPU>(rowNum,colNum,std::move(scalar));
    }

}
```

Batch 模板
```c++
template<typename TElement,typename TDevice,typename TCategory>
class Batch;
//标量列表
template<typename TElement,typename TDevice>
class Batch<TElement,TDevice,CategoryTags::Scalar>;
//矩阵列表
template<typename TElement,typename TDevice>
class Batch<TElement,TDevice,CategoryTags::Matrix>;
```

引入元函数特化将Batch类模板与metaNN的标签体系关联起来
```c++
template <typename TElement,typename TDevice>
constexpr bool IsBatchMatrix<Batch<TElement,TDevice,CategoryTags::Matrix>> = true;

template <typename TElement,typename TDevice>
constexpr bool IsBatchScalar<Batch<TElement,TDevice,CategoryTags::Scalar>> = true;
```

以矩阵列表为例讨论Batch类模板实现细节
```c++
template <typename TElement,typaname TDevice>
class Batch<TElement,TDevice,CategoryTags::Matrix>
{
    public:
    using ElementType = TElement;
    using DeviceType = TDevice;
    
    friend struct LowerAccessImpl<Batch<TElement,TDevice,CategoryTags::Matrix>>;

    public:
    Batch(size_t p_batchNum=0,size_t p_rowNum=0,size_t p_colNum=0);

    //维度相关接口
    size_t RowNum() const{return m_rowNum;}
    size_t ColNum() const{return m_colNum;}
    size_t BatchNum() const {return m_batchNum;}

    //求值相关接口
    // 读写访问接口
    bool AvailableForWrite() const;
    void SetValue(size_t p_batchId,size_t p_rowId,size_t p_collId,ElementType val);
    const auto operator[](size_t p_rowB,size_t p_rowE,size_t p_colB,size_t p_colE) const;
    
    //子矩阵列表接口
    auto SubBatchMatrix(size_t p_rowB,size_t p_rowE,size_t p_colB,size_t p_colE) const;

    private:
    ContinuousMemory<ElementType,DeviceType> m_mem;
    size_t m_rowNum;
    size_t m_colNum;
    size_t m_batchNum;
    size_t m_rowLen;
    size_t m_rawMatrixSize;
};
```

Array模板
Batch与Array的关系：
```c++
template<typename TData>
class Array;
```

与Batch不同,Array只接收一个模板参数,即其中包含的元素类型(比如矩阵类型或者标量类型)
Array可以根据该类型所提供的接口来推导出相应的计算单元,计算设备等信息

```c++
template<typename TData>
constexpr bool IsBatchMatrix<Array<TData>> = IsMatrix<TData>;

template<typename TData>
constexpr bool IsBatchScalar<Array<TData>> = IsScalar<TData>;
```

根据矩阵列表和标量列表提供不同的接口
```c++
template<typename TData,typename TDataCate>
class ArrayImp;

template<typename TData>
class ArrayImp<TData,CategoryTags::Matrix>
{

};

template<typename TData>
class ArrayImp<TData,CategoryTags::Scalar>{

};

template<typename TData>
class Array:public ArrayImp<TData,DataCategory<TData>>
{
    public:
    using ElementType = typename TData::ElementType;
    using DeviceType = typename TData::DeviceType;
    using ArrayImp<TData,DataCategory<TData>>::ArrayImp;
};
```

ArrayImp类模板
用CategoryTags::Matrix特化ArrayImp
```c++
template<typename TData>
class ArrayImp<TData,CategoryTags::Matrix>
{
    public:
    using ElementType = typename TData::ElementType;
    using DeviceType = typename TData::DeviceType;

    ArrayImp(size_t rowNum=0,size_t colNum=0);
    template<typename TIterator,std::enable_if_t<IsIterator<TIterator>>*=nullptr>
    ArrayImp(TIterator p_begin,TIterator p_end);

    public:
    size_t RowNum() const {return m_rowNum;}
    size_t ColNum() const {return m_colNum;}
    size_t BatchNum() const {return m_buffer->size();}

    bool AvailableForWrite() const{
        return (!m_evalBuf.IsEvaluated())&&(m_buffer.use_count()==1);
    }

    protected:
    size_t m_rowNum;
    size_t m_colNum;
    std::shared_ptr<std::vector<TData>>m_buffer;
    EvalBuffer<Batch<ElementType,DeviceType,CategoryTags::Matrix>>m_evalBuf;
};
```

IsIterator 元函数
```c++
template<typename T>
struct IsIterator_
{
    template <typename R>
    static std::true_type Test(typename std::iterator_traits<R>::iterator_category*);

    template <typename R>
    static std::false_type Test(...);

    static constexpr bool value = decltype(Test<T>(nullptr))::value;
};

template<typename T>
constexpr bool IsIterator = IsIterator_<T>::value;
```

Array对象的构造
```c++
Array<Matrix<int,DeviceTags::CPU>> check(10,20);
```
如果想基于一个矩阵数组或vector来构造矩阵列表
```c++
vector<Matrix<int,DeviceTags::CPU>>vec;
vec.push_back();
Array<Matrix<int,DeviceTags::CPU>>check(vec.begin(),vec.end());
```
构造列表时,系统会检测以确保每个矩阵的行列数是相同的,因此vec中的每个矩阵的行列数必须相同

上面的代码在构造check时,需要指定矩阵的具体类型,metaNN提供了MakeArray函数来简化调用的方式,从而使得用户免于显式提供类型声明
```c++
template <typename TIterator>
auto MakeArray(TIterator beg,TIterator end)
{
    using TData = typename std::iterator_traits<TIterator>::value_type;
    using RawData = RemConstRef<TData>;
    return Array<RawData>(beg,end);
}
```

```c++
template <typename TData,typename TDataCate>
class DuplicateImp;

template <typename TData>
class Duplicate:public DuplicateImp<TData,DataCategory<TData>>
{
    public:
    using ElementType = typename TData::ElementType;
    using DeviceType = typename TData::DeviceType;
    using DuplicateImp<TData,DataCategory<TData>>::DuplicateImp;
};

template <typename TData>
constexpr bool IsBatchMatrix<Duplicate<TData>>=IsMatrix<TData>;
template <typename TData>
constexpr bool IsBatchScalar<Duplicate<TData>>=IsScalar<TData>;
```

DuplicateImp 类模板
DuplicateImp模板定义如下
```c++
template <typename TData,typename TDataCate>
class DuplcateImp;

template <typename TData>
class DuplicateImp<TData,CategoryTags::Scalar>{

};

template <typename TData>
class DuplicateImp<TData,CategoryTags::Matrix>
{
    public:
    using ElementType = typename TData::ElementType;
    using DeviceType = typename TData::DeviceType;

    DuplicateImp(TData data,size_t batch_num):m_data(std::move(data)),m_batchNum(batch_num)
    {
        assert(m_batchNum!=0);
    }

    public:
    size_t RowNum() const {return m_data.RowNum();}
    size_t ColNum() const {return m_data.ColNum();}
    size_t BatchNum() const {return m_batchNum;}

    const TData & Element() const{return m_data;}
    //求值相关接口
    protected:
    TData m_data;
    size_t m_batchNum;
    EvalBuffer<Batch<ElementType,DeviceType,CategoryTags::Matrix>> m_evalBuf;

};
```

Duplicate 对象的构造
```c++
template <typename TData>
auto MakeDuplicate(size_t batchNum,TData && data)
{
    using RawDataType = RemConstRef<TData>;
    return Duplicate<RawDataType>(std::forward<TData>(data),batchNum);
}

template <typename TData>
auto MakeDuplicate(size_t batchNum,TData && data)
{
    using RawDataType = RemConstRef<TData>;
    return Duplicate<RawDataType>(std::forward<data>,batchNum);
}

template<typename TData,typename...TParams>
auto MakeDuplicate(size_t batchNum,TParams&&...data){
    using RawDataType = RemConstRef<TData>;
    RawDataType tmp(std::forward<TParams>(data)...);
    return Duplicate<RawDataType>(std::move(tmp),batchNum);
}
```

## 运算与表达式模板

```c++
template <typename T1,typename T2>
class Add
{
    public:
    Add(T1 A,T2 B):m_a(std::move(A)),m_b(std::move(B)){}
    size_t RowNum() const
    {
        assert(m_a.RowNum()==m_b.RowNum());
        return m_a.RowNum();
    }
    private:
    T1 m_a;
    T2 m_b;
};
```
```c++
struct UnaryOpTags {
    struct Sigmoid;
    struct Tanh;
    struct Transpose;
    // ...
};  

struct BinaryOpTags {
    struct Add;
    struct Substract;
    struct ElementMul;
    // ...
};

struct TernaryOpTags {
    struct Interpolate;
    // ...
};
```
辅助类模板OperElementType_/OperDeviceType_
```c++

template <typename TOpTag, typename TOp1, typename... TOperands>
struct OperElementType_ {
    using type = typename TOp1::ElementType;
};

template <typename TOpTag, typename TOp1, typename... TOperands>
struct OperDeviceType_ {
    using type = typename TOp1::DeviceType;
};

```
假定对于某个特殊的运算MyOper,包含了两个输入参数,其结果的计算单元与计算设备类型要根据第二个输入参数来确定,那么我们就可以引入如下的特化:
```c++
template <typename TOp1,typename TOp2>
struct OperElementType_<MyOper,TOp1,TOp2>{
    using type = typename TOp2::ElementType;
};

template <typename TOp1,typename TOp2>
struct OperDeviceType_<MyOper,TOp1,TOp2>{
    using type = typename TOp2::DeviceType;
};
```     

辅助类模板OperCateCal
基于运算标签与输入参数的类别,推断出运算结果的类别
```c++
template <typename TOpTag,typename THead,typename...TRemain>
using OperCateCal = typename CateInduce_<TOpTag,Data2Cate<THead,TRemain...>>::type;
OperCateCal<BinaryOpTags::Add,Matrix<float,DeviceTags::CPU>,TrivalMatrix<float,DeviceTags::CPU>>
```

OperCateCal要对运算所需要的每个输入参数类型进行下面的转换:  
```c++
template <typename TCateCont,typename...TData>
struct Data2Cate_{
    using type = TCateCont;
};

template <typename...TProcessed,typename TCur,typename...TRemain>
struct Data2Cate_<std::tuple<TProcessed...>,TCur,TRemain...>{
    using tmp1=DataCategory<TCur>;
    using tmp2=std::tuple<TProcessed...,tmp1>;
    using type = typename Data2Cate_<tmp2,TRemain...>::type;
};

template <typename THead,typename...TRemain>
using Data2Cate = typename Data2Cate_<std::tuple<>,THead,TRemain...>::type;
```
在获取了输入类型所对应的类别标签后,OperCateCal调用CateInduce_来推断输出结果的类别
```c++
template <typename TOpTag,typename TCateContainer>
struct CateInduce_;

template <typename TOpTag,typename...TCates>
struct CateInduce_<TOpTag,std::tuple<TCates...>>
{
    using type = typename OperCategory_<TOpTag,TCates...>::type;
};

template <typename TOpTag,typename THeadCate,typename...TRemainCate>
struct OperCategory_
{
    static_assert(SameCate_<THeadCate,TRemainCate...>::value,"Input types must have the same category");
    using type = THeadCate;
};
```
其中SameCate_是一个辅助元函数,用于断言传入的类别信息THeadCate,TRemainCate...元素一致性,在此基础上返回THeadCate,即输入参数的类别
考虑矩阵相加的情形,此时OperCategory_模板将会被实例化为如下的样子:
```c++
struct OperCategory_<BinaryOpTags::Add,CategoryTags::Matrix,CategoryTags::Matrix>
{
    static_assert(SameCate_<CategoryTags::Matrix,CategoryTags::Matrix>::value,"Input types must have the same category");
    using type = CategoryTags::Matrix;
}
```

辅助类模板OperOrganizer
提供尺寸相关的接口,其声明如下:
```c++
template <typename TOpTag,typename TCate>
class OperOrganizer;
```

OperOrganizer针对标量的特化版本如下:
```c++
template <typename TOgTag>
class OperOrganizer<TOgTag,CategoryTags::Scalar>
{
    public:
    template <typename THead,typename...TRemain>
    Operation(const THead&,const TRemain&...){}
};
```

将构造函数模板化,使OperOrganizer作为运算模板的基类使用,引入这个特化就可以保证所有运算模板都可以派生自相应的OperOrganizer基类,而无需关注运算结果是否为标量

需要说明的是,所有OperOrganizer特化版本中都包含了一个模板构造函数,接收运算的输入参数并由此计算出运算结果的尺寸信息

针对标量列表的特化
相比标量的版本来说,OperOrganizer标量列表的版本则提供标量列表所需要支持的尺寸逻辑

```c++
template <typename TOpTag>
class OperOrganizer<TOpTag,CategoryTags::BatchScalar>
{
    private:
    template <typename THead,typename...TRemain>
    bool SameDim(const THead&,const TRemain&...)
    {
        return true;
    }

    template <typename THead,typename...TRemain>
    bool SameDim(const THead& head,const TCur& cur,const TRemain&...rem){
        const bool tmp = (head.BatchNum()==cur.BatchNum());
        return tmp&&SameDim(cur,rem...);
    }
    public:
    template <typename THead,typename...TRemain>
    Operation(const THead& head,const TRemain&...rem)
    {
        assert(SameDim(head,rem...));
    }
    size_t BatchNum() const{
        return m_batchNum;
    }
    private:
    size_t m_batchNum;
};
```
辅助类模板OperSeq
OperSeq是最后一个辅助模板,用来封装求值相关的逻辑

```c++
template <typename TOpTag, typename TOp1, typename... TOperands>
struct OperElementType_ {
    using type = typename TOp1::ElementType;
};

template <typename TOpTag, typename TOp1, typename... TOperands>
struct OperDeviceType_ {
    using type = typename TOp1::DeviceType;
};
```
运算模板的类别标签
```c++
template <typename TOpTag, typename TData>
constexpr bool IsScalar<UnaryOp<TOpTag, TData>> =
    std::is_same<OperCateCal<TOpTag, TData>,
    CategoryTags::Scalar>::value;

template <typename TOpTag, typename TData>
constexpr bool IsMatrix<UnaryOp<TOpTag, TData>> = 
    std::is_same<OperCateCal<TOpTag, TData>, 
    CategoryTags::Matrix>::value;

template <typename TOpTag, typename TData>
constexpr bool IsBatchScalar<UnaryOp<TOpTag, TData>> =
    std::is_same<OperCateCal<TOpTag, TData>, 
    CategoryTags::BatchScalar>::value;

template <typename TOpTag, typename TData>
constexpr bool IsBatchMatrix<UnaryOp<TOpTag, TData>> =
    std::is_same<OperCateCal<TOpTag, TData>, 
    CategoryTags::BatchMatrix>::value;
```

UnaryOp的定义
```c++
template <typename TOpTag,typenameTData>
class UnaryOp : public OperOrganizer<TOpTag,OperCateCal<TOpTag,TData>>
{
    static_assert(std::is_same<RemConstRef<TData>,TData>::value,"UnaryOp only accept non-const reference");
    public:
    using ElementType = typename OperElementType_<TOpTag,TData>::type;
    using DeviceType = typename OperDeviceType_<TOpTag,TData>::type;

    public: 
    UnaryOP(TData data):OperOrganizer<TOpTag,Cate>(data),m_data(std::move(data)){}

    private:    
    TData m_data;
};
```

### 运算实现实例
函数接口
```c++
template<typename IP,std::enable_if_t<OperSigmoid_<IP>::valid>*=nullptr>
auto Sigmoid(const IP && p_m)
{
    return OperSigmoid_<IP>::Eval(std::forward<IP>(p_m));
}
```
上面的enable_if_t元函数,只有在OperSigmoid_<IP>::valid为真时,所对应的代码才会编译,否则将触发系统的SFINAE机制,被编译器拒绝

OperSigmoid_模板
```c++
template <typename TP>
struct OperSigmoid_
{
    private:
    using rawM = RemConstRef<TP>;
    public:
    static constexpr bool valid = IsMatrix<rawM>||IsBatchMatrix<rawM>;
    public:
    static auto Eval(TP && p_m)
    {
        using ResType = UnaryOp<UnaryOpTags::Sigmoid,rawM>;
        return ResType(std::forward<TP>(p_m));
    }
};
```

```c++
template <typename TP1,typename TP2,std::enable_if_t<OperAdd_<TP1,TP2>::valid>*=nullptr>
auto operator + (TP1&& p_m1,TP2&& p_m2)
{
    using Cate1 = DataCategory<TP1>;
    using Cate2 = DataCategory<TP2>;
    return OperAdd_<TP1,TP2>::template_Eval<Cate1,Cate2>(std::forward<TP1>(p_m1),std::forward<TP2>(p_m2));
};
```

OperAdd_实现框架
```c++
template <typename TP1,typename TP2>
struct OperAdd_
{
    private:
    using rawM1 = RemConstRef<TP1>;
    using rawM2 = RemConstRef<TP2>;
    public:
    static constexpr bool valid=(IsMatrix<rawM1>&&IsMatrix<rawM2>||(IsMatrix<rawM1>&&IsBatchMatrix<rawM2>)||(IsBatchMatrix<rawM1>&&IsMatrix<rawM2>));

    template <typename X1,typename X2>
    constexpr static bool Imp = std::is_same<X1,X2>::value;
    using CT = CategoryTags;
    template <typename T1,typename T2,std::enable_if_t<Imp<T1,T2>>*=nullptr>
    static auto Eval(TP1&& p_m1,TP2&& p_m2);
    template <typename T1,typename T2,std::enable_if_t<Imp<CT::Matrix,T1>>*=nullptr>,std::enable_if_t<Imp<CT::Matrix,T2>>*=nullptr>
    static auto Eval(TP1&& p_m1,TP2&& p_m2);
};
```
OperAdd_::Eval的实现

```c++
template <typename T1,typename T2,std::enable_if_t<Imp<T1,T2>>*=nullptr>
static auto Eval(TP1&& p_m1,TP2&& p_m2)
{
    static_assert(std::is_same<typename rawM1::DeviceType,typename rawM2::DeviceType>::value,"Input matrices must have the same device type");
    static_assert(std::is_same<typename rawM1::ElementType,typename rawM2::ElementType>::value,"Input matrices must have the same element type");
    using ResType = BinaryOp<BinaryOpTags::Add,rawM1,rawM2>;
    return ResType(std::forward<TP1>(p_m1),std::forward<TP2>(p_m2));
}
```

由于需要二者数据类型相同,对于矩阵和标量的相加,不能在构造BinaryOp时直接传入矩阵与标量,因为这样会造成运算输入参数的标签不同
运算模板在确定计算结果的类别标签时，采用的默认方式是首先确保其操作数的类别标签相同,并在此基础上返回第一个操作数的类别标签,如果运算输入类别标签不同,就无法利用这个默认行为

对于第一个操作数是标量,第二个操作数是矩阵的情况,可以根据第二个操作数的信息构造出第一个操作数所对应的平凡矩阵,之后以这个平凡矩阵作为加法的操作数即可

加法支持交换率,因此在有了矩阵与标量相加的版本后,可以很容易实现标量与矩阵相加的版本:
```c++
template <typename T1,typename T2,std::enable_if_t<Imp<CT::Matrix,T1>>*=nullptr>,std::enable_if_t<Imp<CT::Scalar,T2>>*=nullptr>
static auto Eval(TP1&& p_m1,TP2&& p_m2)
{
    return OperAdd_<TP2,TP1>::template_Eval<T2,T1>(std::forward<TP2>(p_m2),std::forward<TP1>(p_m1));
}
```

转置运算
转置是一元矩阵运算,它接收NxM的矩阵,返回MxN的矩阵,其实现如下:

与求和,Sigmoid等运算相比,转置运算的最大区别就是要修改OperOrganizer的默认行为:其输入,输出矩阵的行列数不再相同,我们需要引入OperOrganizer的特化来描述这种变化:
```c++
template<>
class OperOrganizer<UnaryOpTags::Transpose,CategoryTags::Matrix>
{
    public:
    template <typename TData>:m_rowNum(data.ColNum()),m_colNum(data.RowNum()){}
    size_t RowNum() const{return m_rowNum;}
    size_t ColNum() const{return m_colNum;}

    private:
    size_t m_rowNum;
    size_t m_colNum;
};

template<>
class OperOrganizer<UnaryOpTags::Transpose,categoryTags::BatchMatrix>:public OperOrganizer<UnaryOpTags::Transpose,CategoryTags::Matrix>
{
public:
    template <typename TData>
    OperOrganizer(const TData& data):BaseType(data),m_batchNum(data.BatchNum()){}
    size_t BatchNum() const{return m_batchNum;}
private:
    size_t m_batchNum;
};
```

折叠运算
折叠操作是一种典型的输入与输出类别不同的运算,它的输入是矩阵列表,将列表中的矩阵求和,生成结果矩阵并输出
不满足OperCategory_的默认行为——输入输出的类别标签相同,在实现该操作时,我们需要特化OperCategory_;

```c++
template <>
struct OperCategory_<UnaryOpTags::Collapse,CategoryTags::BatchMatrix>
{
    using type = CategoryTags::Matrix;
};
```
----

## 基本层

初始化模块
```c++
auto initializer 
    = MakeInitializer<float,
        PInitializerIs<struct Gauss1>,
        PWeightInitializerIs<struct Gauss2>>()
        .SetFiller<Gauss1>(GaussianFiller(0, 1.5))
        .SetFiller<Gauss2>(GaussianFiller(0, 3.3));

Matrix<float, CPU> mat;
initializer.SetMatrix("name1", mat);

map<string, Matrix<float, DeviceTags::CPU>> loader;
layer.Init(initializer, loader);
```
MakeArrayInitializer是整个初始化模块的调用入口,从之前的使用方式不难看出,它实际上使用了policy模板来设置
初始化器的标签,使用异类词典来设置标签对应的初始化器
```c++
struct InitPolicy {
    using MajorClass = InitPolicy;

    struct OverallTypeCate;
    struct WeightTypeCate;
    struct BiasTypeCate;

    using Overall = void;
    using Weight = void;
    using Bias = void;

    // ...
};

TypePolicyTemplate(PInitializerIs, InitPolicy, Overall);
TypePolicyTemplate(PWeightInitializerIs, InitPolicy, Weight);
TypePolicyTemplate(PBiasInitializerIs, InitPolicy, Bias);
```

PInitializerIs用于设置默认初始化器,PWeightInitializerIs用于设置权重初始化器,PBiasInitializerIs用于设置偏置初始化器

```c++
template <typename TElem, typename... TPolicies>
auto MakeInitializer() {
    using npType = FillerTags2NamedParams<TPolicies...>;
    using FilDictType = RemConstRef<decltype(npType::Create())>;
    return ParamInitializer<TElem,
        PolicyContainer<TPolicies...>,
        FilDictType>
        (npType::Create());
}
```

ParamInitializer模板类用于初始化矩阵,其模板参数为矩阵元素类型,策略容器,以及填充字典

```c++
template <typename TElem, typename TPolicyCont, typename TFillers>
class ParamInitializer {
public:
    using PolicyCont = TPolicyCont;

    ParamInitializer(TFillers&& filler)
        : m_filler(std::move(filler)) {}

    // 初始化器的设置与获取接口
    template <typename TTag, typename TVal>
    auto SetFiller(TVal&& val) &&;

    template <typename TTag>
    auto GetFiller();

    // 参数矩阵的设置与获取接口
    template <typename TElem2, typename TDevice2>
    void SetMatrix(const std::string& name,
                   const Matrix<TElem2, TDevice2>& param);

    template <typename TElem2, typename TDevice2>
    void GetMatrix(const std::string& name,
                   Matrix<TElem2, TDevice2>& res) const;

    bool IsMatrixExist(const std::string& name) const;

private:
    TFillers m_filler;
    std::map<std::string, Matrix<TElem, DeviceTags::CPU>> m_params;
};

```

初始化器类模板

初始化器在 MetaNN 中表示为类模板。目前，MetaNN 实现了如下几个初始化器的类模板：

- **ConstantFiller**  
  将矩阵的内容初始化为某个常量；
  
- **GaussianFiller**  
  使用正态分布初始化矩阵参数；
  
- **UniformFiller**  
  使用均匀分布初始化矩阵参数；
  
- **VarScaleFiller**  
  实现了 TensorFlow 中的 `variance_scaling_filler`，并由此构造出了 `XavierFiller` 与 `MSRAFiller`。


AddLayer模块

AddLayer是一个两输入,一输出的层,因此我们首先定义输入容器
```c++
using AddLayerInput = NamedParams<struct AddLayerIn1,struct AddLayerIn2>;
```
AddLayer类模板定义如下:
```c++
template <typename TPolicies>
class AddLayer
{
    static_assert(IsPolicyContainer<TPolicies>::value, "PolicyContainer required");
    using CurLayerPolicy = PlainPolicy<TPolicies>;
    public:
    static constexpr bool IsFeedbackOutput = PolicySelect<FeedbackOutput, CurLayerPolicy>::IsFeedbackOutput;
    static constexpr bool IsUpdate = false;
    using InputType = AddLayerInput;
    using OutputType = LayerIO;
    public:
    template <typename TIn>
    auto FeedForward(const TIn & p_in)
    {
        auto in1 = p_in.Get<AddLayerIn1>();
        auto in2 = p_in.Get<AddLayerIn2>();
        using rawType1 = std::decay_t<decltype(in1)>;
        using rawType2 = std::decay_t<decltype(in2)>;
        static_assert(IsMatrix<rawType1>::value && IsMatrix<rawType2>::value, "Input must be matrix");
        static_assert(std::is_same<typename rawType1::ElementType, typename rawType2::ElementType>::value, "Input must have the same element type");
        return OutputType::Create().Set<LayerIO>(in1 + in2);

    }
    template <typename TGrad>
auto FeedBackward(TGrad&& p_grad)
{
    if constexpr (IsFeedbackOutput)
    {
        auto res = p_grad.template Get<LayerIO>();
        return AddLayerInput::Create()
            .template Set<AddLayerIn1>(res)
            .template Set<AddLayerIn2>(res);
    }
    else
    {
        return AddLayerInput::Create();
    }
}

};
```

记录中间结果
```c++
using ElementMulLayerInput =
NamedParams<struct MulLayerIn1, struct MulLayerIn2>;
template <typename TPolicies>
class ElementMulLayer
{
    using CurLayerPolicy = PlainPolicy<TPolicies>;
    public:
    static constexpr bool IsFeedbackOutput = ...;
    static constexpr bool IsUpdate = false;
    using InputType = ElementMulLayerInput;
    using OutputType = LayerIO;

    public:
    template <typename TIn>
    auto FeedBackward(const TIn & p_in);
    void NeutralInvariant();
    private:
    using BatchMode = PolicySelect<InputPolicy,CurLayerPolicy>::BatchMode;
    using ElementType = typename PolicySelect<OperandPolicy,CurLayerPolicy>::Element
    using DeviceType = typename PolicySelect<OperandPolicy,CurLayerPolicy>::Device;
    using DataType = LayerInternalBuf<IsFeedbackOutput, BatchMode, ElementType, DeviceType,CategoryTags::Matrix,CategoryTags::BatchMatrix>;
    DataType m_data1;
    DataType m_data2;
};
```











