#ifndef LOCK_FREE_STACK_HPP
#define LOCK_FREE_STACK_HPP

#include <atomic>
#include <memory>

template<typename T>
class lock_free_stack
{
    private:
        struct node
        {
            T data;
            node* next;
            node(const T& data_):data(data_){};
        };
        std::atomic<node*> head;
    public:
        void push(T const& data)
        {
            node* const new_node = new node(data);
            new_node->next = head.load();
            while(!head.compare_exchange_weak(new_node->next, new_node));
        //首先不等,new_node->next = head;然后相等head=new_node，退出循环
        }
        void pop(T& result)
        {
            node* old_head=head.load();
            while(!head.compare_exchange_weak(old_head,old_head->next));
            result=old_head->data;
        }//一旦比较交换操作成功,说明只有当前线程在改动栈容器,从栈顶弹出节点
        //该段代码未处理空栈情况,以及安全异常处理
};

template<typename T>
class lock_free_stack_sptr
{
    private:
    struct node{
        std::shared_ptr<T> data;
        node* next;
        node(T const& data_):data(std::make_shared<T>(data)){}
    };
    std::atomic<node*> head;
    public:
        void push(T const& data)
        {
            node* const new_node = new node(data);
            new_node->next = head.load();
            while(!head.compare_exchange_weak(new_node->next, new_node));
        }
        std::shared_ptr<T> pop()
        {
            node* old_head=head.load();
            while(old_head && !head.compare_exchange_weak(old_head,old_head->next));
            return old_head? old_head->data : std::shared_ptr<T>();
        }
        //这段代码虽然是无锁实现,却非免等实现,原因在于若compare_exchange_weak()的结果总是false,理论上会导致push()和pop()中的while循环持续进行
    //只有等到没有线程调用pop()时才删除链表中的节点
    private:
        std::atomic<node*> to_be_deleted;
        static void delete_nodes(node* nodes)
        {
            while(nodes)
            {
                node* next = nodes->next;
                delete nodes;
                nodes = next;
            }
        }
        void try_reclaim(node* old_head)
        {
            if(threads_in_pop==1)
            {
                node* nodes_to_delete = to_be_deleted.exchange(nullptr);//当前线程把候删链表收归己有
                if(!--threads_in_pop)//判断pop()是否仅仅正被当前线程唯一调用
                {
                    delete_nodes(nodes_to_delete);
                }
                else if(nodes_to_delete)
                {
                    chain_pending_nodes(nodes_to_delete);
                }
                delete old_head;
            }
            else{
                chain_pending_node(old_head);
                --threads_in_pop;
            }
        }

        void chain_pending_nodes(node* nodes)
        {
            node* last=nodes;
            while(node* const next=last->next)
            {
                last=next;
            }
            chain_pending_nodes(nodes,last);
        }

        void chain_pending_nodes(node* first, node* last)
        {
            last->next = to_be_deleted;
            while(!to_be_deleted.compare_exchane_weak(last->next,first));//借循环保证last->next指向正确
        }

        void chain_pending_node(node* n)
        {
            chain_pending_nodes(n,n);
        }
    
    private:    
        std::atomic<unsigned> threads_in_pop;
        void try_reclaim(node* old_head);
    public:
        std::shared_ptr<T> pop_safe()
        {
            ++threads_in_pop;
            node* old_head = head.load();
            while(old_head&&!head.compare_exchange_weak(old_head,old_head->next));
            //此处head已经指向下一个节点,所以不会有线程再访问old_head
            std::shared_ptr<T> res;
            if(old_head)
            {
                res.swap(old_head->data);
            }
            try_reclaim(old_head);
            return res;
        }
        //原子变量threads_in_pop记录目前正有多少线程试图从栈容器弹出数据
        //它在pop()函数的最开始处自增,在try_reclaim()内部自减,每当有节点被删除,程序就调用try_reclaim()一次
        //由于可能延后删除节点,所以暂时只用swap将数据置换出来,而先不删除地址
};

#endif // LOCK_FREE_STACK_HPP