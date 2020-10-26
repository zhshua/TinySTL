#ifndef _TINYSTL_ITERATOR_H_
#define _TINYSTL_ITERATOR_H_
#include <cstddef>

namespace TinySTL {
    //定义五种迭代器的类型，写成类的形式方便自动推导
    struct input_iterator_tag {}; //输入迭代器
    struct output_iterator_tag {}; //输出迭代器
    struct forward_iterator_tag : public input_iterator_tag {}; //前向迭代器
    struct bidirectional_iterator_tag : public forward_iterator_tag {}; //双向迭代器
    struct random_access_iterator_tag : public bidirectional_iterator_tag {}; //随机迭代器

    //为了统一接口标准，定义一个迭代器类。自行开发的迭代器要继承此类
    template <class Category, class T, class Distance = ptrdiff_t, class Pointer = T*, class Reference = T&>
    struct iterator{
        typedef Category    iterator_category;
        typedef T           value_type;
        typedef Distance    difference_type;
        typedef Pointer     pointer;
        typedef Reference   reference;
    };

    //萃取剂
    template <class Iterator>
    struct iterator_traits{
        typedef typename Iterator::iterator_category    iterator_category;
        typedef typename Iterator::value_type           value_type;
        typedef typename Iterator::difference_type      difference_type;
        typedef typename Iterator::pointer              pointer;
        typedef typename Iterator::reference            reference;
    };

    //一个针对原生指针的偏特化版本
    template <class T>
    struct iterator_traits<T*>{
        typedef random_access_iterator_tag      iterator_category;
        typedef T                               value_type;
        typedef ptrdiff_t                       difference_type;
        typedef T*                              pointer;
        typedef T&                              reference;
    };

    //一个针对const原生指针的偏特化版本
    template <class T>
    struct iterator_traits<const T*>{
        typedef random_access_iterator_tag      iterator_category;
        typedef T                               value_type;
        typedef ptrdiff_t                       difference_type;
        typedef const T*                        pointer;
        typedef const T&                        reference;
    };

    // 这个函数用来求出某个迭代器的类型
    template <class Iterator>
    inline typename iterator_traits<Iterator>::iterator_category
    iterator_category(const Iterator& It){
        typedef typename iterator_traits<Iterator>::iterator_category category;
        return category();
    }

    // 这个函数用来求出某个迭代器的distance type
    template <class Iterator>
    inline typename iterator_traits<Iterator>::difference_type*
    distance_type(const Iterator& It){
        return static_cast<typename iterator_traits<Iterator>::difference_type *>(0);
    }

    // 这个函数用来求出某个迭代器的value type
    template <class Iterator>
    inline typename iterator_traits<Iterator>::value_type*
    value_type(const Iterator& It){
        return static_cast<typename iterator_traits<Iterator>::value_type *>(0);
    }

    // 如果是输入迭代器，进行此函数
    // 此函数对forward_iterator_tag、bidirectional_iterator_tag也适用
    // 因为继承关系，没有找到合适的匹配，则会匹配父类
    template <class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    __distance(InputIterator first, InputIterator last, input_iterator_tag){
        iterator_traits<InputIterator>::distance_type n = 0;
        while (first != last){
            ++first;
            ++n;
        }
        return n;
    }

    // 此函数为随机迭代器时候的计算
    template <class RandomAccessIterator>
    inline typename iterator_traits<RandomAccessIterator>::difference_type
    __distance(RandomAccessIterator first, RandomAccessIterator last, random_access_iterator_tag){
        return last - first; //随机迭代器，支持 ± 一段距离
    }

    // distance函数，求两个迭代器之间的距离
    template <class InputIterator>
    inline typename iterator_traits<InputIterator>::difference_type
    distance(InputIterator first, InputIterator last){
        typedef typename iterator_traits<InputIterator>::iterator_category category; // 先萃取出迭代器的类型
        return __distance(first, last, category()); // 再根据迭代器类型进入相应的求距离函数
    }

    // 如果是输入迭代器，进入此函数
    // 此函数对forward_iterator_tag也适用，也会进入此函数
    template <class InputIterator, class Distance>
    inline void __advance(InputIterator& i, Distance n, input_iterator_tag){
        while (n--) ++i;
    }

    // 如果是双向迭代器，进入此函数
    template <class BidirectionalIterator, class Distance>
    inline void __advance(BidirectionalIterator& i, Distance n, bidirectional_iterator_tag){
        if (n >= 0)
            while (n--) ++i;
        else
            while (n++) --i;
    }

    // 如果是随机迭代器，进入此函数
    template <class RandomAccessIterator, class Distance>
    inline void __advance(RandomAccessIterator& i, Distance n, random_access_iterator_tag){
        i += n;
    }

    // advance函数，求出迭代器移动n位后的位置
    template <class InputIterator, class Distance>
    inline void advance(InputIterator& i, Distance n){
        __advance(i, n, iterator_category(i));
    }
}

#endif
