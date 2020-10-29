#ifndef _UNINITIALIZED_H_
#define _UNINITIALIZED_H_

#include "type_traits.h"
#include "construct.h"

namespace TinySTL{

    // ***************** uninitialized_copy模块 ***********************
    // 如果是POD型
    template <class InputIterator, class ForwardIterator>
    ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, 
                                             ForwardIterator result, _true_type){
        return copy(first, last, result); // 是POD型，直接调用STL算法copy()
    }

    // 如果不是POD型
    template <class InputIterator, class ForwardIterator>
    ForwardIterator __uninitialized_copy_aux(InputIterator first, InputIterator last, 
                                             ForwardIterator result, _false_type){
        ForwardIterator cur = result;
        for (; first != last; ++cur, ++first){
            construct(cur, *first); //一个个构造
        }
        return cur;
    }

    /* 该函数的作用是将区间[first, last)的内容复制到已result为起始位置的空间中
     * 迭代器first指向输入端的起始位置
     * 迭代器last指向输入端的结束位置(前闭后开区间)
     * 迭代器result指向输出端的起始处
     */
    template <class InputIterator, class ForwardIterator>
    ForwardIterator uninitialized_copy(InputIterator first, InputIterator last, ForwardIterator result){
        typedef typename _type_traits<InputIterator>::is_POD_type is_POD;
        return __uninitialized_copy_aux(first, last, result, is_POD());
    }

    // ***************** uninitialized_fill模块 ***********************
    // 如果是POD型
    template <class ForwardIterator, class T>
    inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
                                         const T& x, _true_type){
        fill(first, last, x); // 调用STL算法fill()
    }

    // 如果不是POD型
    template <class ForwardIterator, class T>
    inline void __uninitialized_fill_aux(ForwardIterator first, ForwardIterator last,
                                         const T& x, _false_type){
        ForwardIterator cur = first;
        while (cur != last){
            construct(cur, x);
            ++cur;
        }
    }

    /* 该函数的作用是将区间[first, last)以x进行填充
     * 迭代器first指向输出端的起始位置
     * 迭代器last指向输出端的结束位置
     * x表示初值
     */
    template <class ForwardIterator, class T>
    inline void uninitialized_fill(ForwardIterator first, ForwardIterator last, const T& x){
        typedef typename _type_traits<ForwardIterator>::is_POD_type is_POD;
        __uninitialized_fill_aux(first, last, x, is_POD());
    }

    // ***************** uninitialized_fill_n模块 ***********************
    // 如果是POD型
    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, _true_type){
        return fill_n(first, n, x);// 调用STL算法fill_n()
    }

    // 如果不是POD型
    template <class ForwardIterator, class Size, class T>
    inline ForwardIterator __uninitialized_fill_n_aux(ForwardIterator first, Size n, const T& x, _false_type){
        ForwardIterator cur = first;
        for (; n > 0; ++cur, --n)
            construct(cur, x);
        return cur;
    }
    
    /* 该函数的作用是将区间[first, first + n)以x进行填充
     * 迭代器first指向输出端的起始位置
     * x表示初值
     */
    template <class ForwardIterator, class Size, class T>
    inline void uninitialized_fill_n(ForwardIterator first, Size n, const T& x){
        typedef typename _type_traits<ForwardIterator>::is_POD_type is_POD;
        __uninitialized_fill_n_aux(first, n, x, is_POD());
    }

}

#endif