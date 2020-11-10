#ifndef _LIST_H_
#define _LIST_H_

#include "iterator.h"
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
}

#endif