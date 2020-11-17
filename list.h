#ifndef _LIST_H_
#define _LIST_H_

#include "iterator.h"
#include "allocator.h"
namespace TinySTL{
    // 定义list的节点结构体类型
    template <class T>
    struct __list_node{
        typedef void* void_pointer;
        void_pointer prev; // 前驱
        void_pointer next; // 后继
        T data; // 数据域
    };

    // 定义list独有的迭代器类
    template <class T, class Ref, class Ptr>
    struct __list_iterator{
        typedef __list_iterator<T, T&, T*>              iterator;
        typedef __list_iterator<T, Ref, Ptr>            self;
        
        // 由于是自己定义的迭代器,没有继承于iterator,因此要定义以下五种基本类型
        typedef bidirectional_iterator_tag iterator_category;
        typedef T value_type;
        typedef Ptr pointer;
        typedef Ref reference;
        typedef ptrdiff_t difference_type;

        typedef __list_node<T>* link_type;
        typedef size_t size_type;

        link_type node; // 迭代器内部的指针,指向list的节点

        // 构造器
        __list_iterator(link_type x) : node(x) {}
        __list_iterator() {}
        __list_iterator(const iterator& x) : node(x.node) {}

        bool operator==(const self &x) { return x.node == node; }
        bool operator!=(const self &x) { return x.node != node; }

        reference operator*() const { return (*node).data; }
        reference operator->() const { return &(operator*()); }
        
        // 前++
        self& operator++(){
            node = node->next;
            return *this;
        }

        // 后++
        self operator++(int){
            self temp = *this;
            ++*this;
            return temp;
        }

        // 前--
        self& operator--(){
            node = node->prev;
            return *this;
        }

        // 后--
        self operator--(int){
            self temp = *this;
            --*this;
            return temp;
        }
    };

    template <class T, class Alloc = allocator<T>>
    class list{
    protected:
        typedef __list_node<T> list_node;
        list_node* node;
        typedef Alloc list_node_allocator; // 专属的配置空间

    public:
        typedef list_node* link_type;
    protected:
        // 配置一个节点
        link_type get_node() { return list_node_allocator::allocate(); }
        // 释放一个节点
        void put_node(link_type p) { list_node_allocator::deallocate(p); }
        // 产生一个节点(配置并构造)
        link_type create_node(const T& x){
            link_type p = get_node();
            construct(&p->data, x); // 在data位置创建一个x对象
            return p;
        }
        // 销毁一个节点(释放并析构)
        void destory_node(link_type p){
            destory(&p->data); // 析构掉data位置上的对象
            put_node(p);  // 释放空间
        }
        // 初始化一个空链表
        void empty_init(){
            node = get_node();
            node->next = node;
            node->prev = node;
        }

        // 在迭代器pos位置上插入一个元素x,以下函数写法实质是双链表在pos前面插入一个节点tmp
        iterator insert(iterator position, const T& x){
            // 先创建一个节点
            link_type tmp = create_node(x);
            tmp->next = position.node;
            tmp->prev = position.node->prev;
            (link_type)(position.node->prev)->next = tmp;
            position.node->prev = tmp;
            return tmp;
        }

        // 移除迭代器pos所指的节点
        iterator erase(iterator position){
            link_type next_node = link_type(position.node->next);
            link_type prev_node = link_type(position.node->prev);
            prev_node->next = next_node;
            next_node_prev = prev_node;
            destory_node(position.node);
            return next_node;
        }

        // 将[first, last)内的所有元素移动到pos之前
        void transfer(iterator position, iterator first, iterator last){
            (link_type)(last.node->prev)->next = position.node;
            (link_type)(first.node->prev)->next = last.node;
            (link_type)(position.node->prev)->next = first.node;
            link_type tmp = (link_type)(position.node->prev);
            position.node->prev = last.node->prev;
            last.node->prev = first.node->prev;
            first.node->prev = tmp;
        }

    public:
        iterator begin() { return (link_type)((*node).next); }
        iterator end() { return node; } // 左闭右开原则,因此返回node而不是node前面的
        bool empty() { return node->next == node; }
        size_type size() const{
            size_type result = 0;
            for (auto i = begin(); i != end();++i)
                ++result;
            return result;
        }
        reference front() { return *begin(); }
        reference back() { return *(--end()); }
        // 构造函数, 产生一个空链表
        list() { empty_init(); }
        void push_back(const T &x) { insert(end(), x); }
        void push_front(const T &x) { insert(begin(), x); }
        void pop_back() { erase(--end()); }
        void pop_front() { erase(begin()); }

        // 清除整个链表
        void clear();

        // 将数值为value的所有元素移除
        void remobe(const T &value);

        // 移除数值相同的连续元素, 只有连续相同的元素，才会被移除只剩下一个
        void unique();

        // 将x接合到pos所指的位置之前, x必须不同于*this
        void splice(iterator position, list &x);
        // 将i所指的元素接合到pos所指的位置之前,pos和i可指向同一个list
        void splice(iterator position, list &, iterator i);
        // 将[first, last)内的所有元素接合到pos所指的位置之前
        // pos和[first,last)可指向同一个list,但pos不能位于[first,last)之内
        void splice(iterator position, list &, iterator first, iterator last);

        // merge()将x合并到*this身上,两个lists的内容都必须递增有序
        void merge(list<T, Alloc> &x);

        // reverse()将*this的内容逆向重置
        void reverse();

        // list不能使用STL算法sort(), 必须使用自己的sort()成员函数
        // 因为STL的sort()算法只接受RamdonAccessIterator
        // 本函数采用快排
        void sort();

        ~list(){
            clear();
            put_node(node);
        }
    };
}

#endif