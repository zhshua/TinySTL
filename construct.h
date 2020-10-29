#ifndef _CONSTRUCT_H_
#define _CONSTRUCT_H_

#include <new>
#include "type_traits.h"


namespace TinySTL{
    template<class T1, class T2>
    inline void construct(T1* ptr, const T2& value){
        new (ptr) T1(value);  // placement new, 调用T1的构造函数 T1::T1(value)
                            // 它是opreator new 的一个重载版本，作用是在已分配的内存中创建一个对象。
    }

    //释放ptr所指内存空间
    template<class T>
    inline void destory(T* ptr){
        ptr->~T();  //调用T的析构函数
    }

    //如果不是POD型别的，则自己释放内存
    template<class ForwardIterator>
    inline void __destory(ForwardIterator first, ForwardIterator last, _false_type){
        while(first != last){
            destory(&*first);
            ++first;
        }
    }

    //如果是POD型别的，则什么都不做
    template<class ForwardIterator>
    inline void __destory(ForwardIterator first, ForwardIterator last, _true_type) {}

    //destory的重载版本，释放区间[first, last)的内存
    template<class ForwardIterator>
    inline void destory(ForwardIterator first, ForwardIterator last){
        typedef typename __type_traits<ForwardIterator>::is_POD_type is_POD;
        __destory(first, last, is_POD());   // 判断迭代器类型是否为POD型(也就是标量型别或传统的C struct型别)
                                            // POD型别的有自己的构造析构函数，我们什么都不用做
    }

}

#endif