#pragma once
#include <iostream>

template<typename T>
class winnerTree
{
    public:
    virtual ~winnerTree(){};
    virtual void initialize(T *thePlayer, int theNumberOfPlayers)=0;
    virtual int winner()const =0;//返回最后的索引
    virtual void rePlay(int thePlayer,T score) =0;//重播树
};

template<typename T>
class completeWinnerTree:public winnerTree<T>
{
    public:
        completeWinnerTree(T *thePlayer, int theNumerOfPlayers) {
            //构造函数，将赢者树初始化为空，然后生成赢者树
            this->tree = nullptr;
            initialize(thePlayer, theNumerOfPlayers);
        }
        ~completeWinnerTree() {
            //析构函数
            if (tree) {
                delete[] tree;
            }
            tree = nullptr;
        }
    
        //用数组thePlayer[1:numberOfPlayers]生成赢者树
        void initialize(T *thePlayer, int theNumberOfPlayers)override;
        //返回最终赢者的索引
        int winner()const override { return this->tree[0]; }
        //返回竞赛树某个节点的赢者
        int winner(int i) const{
            return (i < this->numberOfPlayers) ? this->tree[i] : 0;
        }
        //在参赛者thePLayer的分数变化后重赛
        void rePlay(int thePlayer,T score);
        //输出赢者树中的一些信息
        void output()const;
        template<typename U>
        friend std::ostream& operator<<(std::ostream& os, const completeWinnerTree<U>& tree);
    protected:
        void play(); 
        int lowExt;         //最底层外部节点个数
        int offset;         //offset=2*s-1（s为最底层最左端的内部节点）
        int *tree;          //赢者树
        int numberOfPlayers;//竞赛选手的数量
        T *player;          //保存竞赛选手
};

template<typename T>
void  completeWinnerTree<T>::initialize(T *thePlayer, int theNumberOfPlayers)
{
    int n = theNumberOfPlayers;//竞赛者的数量
    if(n<2) throw std::invalid_argument("theNumberOfPlayers must be greater than 1");
    this ->player = thePlayer;
    this ->numberOfPlayers = n;
    delete[] tree;
    // this ->tree = new int[n-1];
    //计算最底层外部节点个数
    int s = 1;
    while (s *2< n) {
        s <<= 1;
    }
    this->lowExt = (n-s)*2;

    //生成赢者树
    this ->offset = 2*s-1;
    
    this->play();
}
template<typename T>
void completeWinnerTree<T>::play() {
    T *winner = new T[this->offset+this->lowExt];
    if(this->tree != nullptr){ delete[] this->tree; }
    this->tree = new int[this->offset+this->lowExt];
    // std::cout<<"offset="<<offset<<'\n';
    // std::cout<<"lowExt="<<lowExt<<'\n';
    for (int i =0; i<this->numberOfPlayers; i++)
    {
        if(i<this->lowExt){
            winner[i+this->offset] = this->player[i];
            tree[i+this->offset] = i;
        }
        else 
        {
           winner[i+this->numberOfPlayers-this->lowExt-1] = this->player[i];
           tree[i+this->numberOfPlayers-this->lowExt-1] = i;
        } 
    }    
    //生成赢者树
    for (int i = (offset+lowExt-1)/2-1; i >= 0; i--)
    {
        T left = winner[i*2+1];
        T right = winner[i*2+2];
        if (left >= right)
        {
            winner[i] = left;
            tree[i] = tree[i*2+1];
        }
        else
        {
            winner[i] = right;
            tree[i] = tree[i*2+2];
        }
    }
    // for (int i = 0; i < this->offset+this->lowExt; i++)
    // {
    //     std::cout << winner[i] ;
    //     std::cout << " ";
    // }
    // std::cout << '\n';
    // for (int i = 0; i < this->offset+this->lowExt; i++)
    // {
    //     std::cout <<tree[i] << " ";
    // }
    // std::cout << '\n';
    delete[] winner;
}

template<typename T>
void completeWinnerTree<T>::output() const
{
    std::cout << "winnerTree:\n";
    for (int i = 0; i < this->offset+this->lowExt; i++)
    {
        std::cout << this->tree[i] << " ";
    }
    std::cout << '\n';
}

template<typename U>
std::ostream& operator<<(std::ostream& os, const completeWinnerTree<U>& tree)
{
    tree.output();
    return os;
}

template<typename T>
void completeWinnerTree<T>::rePlay(int thePlayer,T score) 
{
    std::cout << "rePlay " << thePlayer << '\n';//重播树
    if(thePlayer >= this->numberOfPlayers) throw std::invalid_argument("thePlayer out of range");
    this->player[thePlayer] = score;
    this->play();
}

class binType {
    // int capacity;
    public:
    int unusedcapacity;
    friend std::ostream& operator<< (std::ostream& os, const binType& bin);
    bool operator >= (const binType& other) const;
};

bool binType::operator >= (const binType& other) const{
    return this->unusedcapacity >= other.unusedcapacity;
}

std::ostream& operator<<(std::ostream& os, const binType& bin) {
    os << "binType" << "\t" << bin.unusedcapacity << '\n';
    return os;
}

// template<typename T>
void firstFitPack(int *objectSize,int numOfObjects,int capacity)
{
    int n = numOfObjects;
    binType *bin = new binType[n];
    for (size_t i = 0; i <n; i++)
    {
       bin[i].unusedcapacity = capacity;
    }
    completeWinnerTree<binType> tree(bin,n);
    for (size_t i = 0; i <n; i++)
    {
        int child = 1;//从树的左孩子开始找
        while(child< (n-1))
        {
            int winner = tree.winner(child);
            if(bin[winner].unusedcapacity < objectSize[i])
            {
                child++;
            }
            child = 2*child+1;
        }
        int binToUse ;//设置为要使用的箱子
        child = (child-1)/2;//撤销向最后左孩子的移动
        if(child < (n-1))//说明child是内部节点
        {
            binToUse = tree.winner(child);
            if(binToUse>0&&bin[binToUse-1].unusedcapacity >= objectSize[i])
            {
                binToUse --;
            } 
        }
        else binToUse = tree.winner((child-1)/2);//child==n-1,说明是外部节点
        std::cout<<"pack object "<<i<<" in bin "<<binToUse<<'\n';
        bin[binToUse].unusedcapacity -= objectSize[i];
        tree.rePlay(binToUse,bin[binToUse]);
    }
    for (size_t i = 0; i <n; i++)
    {
        std::cout<<i<<" "<<bin[i]<<' ';
    }
}

