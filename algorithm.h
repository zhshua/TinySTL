#ifndef _ALGORITHM_H_
#define _ALGORITHM_H_
#include <string.h>
#include "type_traits.h"

namespace TinySTL{
    // *************[copy]的相关函数*************
    template <class InputIterator, class OutputIterator>
    inline OutputIterator _copy(InputIterator first, InputIterator last, OutputIterator result, _true_type){
        memmove(result, first, sizeof(*first) * (last - first));
        return result + (last - first);
    }
    template <class InputIterator, class OutputIterator>
    inline OutputIterator _copy(InputIterator first, InputIterator last, OutputIterator result, _false_type){
        for (; first != last; ++result, ++first)
            *result = *first;
        return result;
    }
    // T*指针形式版本
    template <class InputIterator, class OutputIterator, class T>
    inline OutputIterator _copy(InputIterator first, InputIterator last, OutputIterator result, T*){
        typedef typename TinySTL::_type_traits<T>::is_POD_type is_POD;
        return __copy(first, last, result, is_POD());
    }
    // copy完全泛化的版本,把区间[first, last)的内容复制到以result为起始位置的空间去
    template <class InputIterator, class OutputIterator>
    inline OutputIterator copy(InputIterator first, InputIterator last, OutputIterator result){
        return _copy(first, last, result, value_type(first));
    }
    // copy针对原生指针的重载版本
    inline char* copy(const char* first, const char* last, char* result){
        memmove(result, first, last - first); // 用来将first所指的空间中前last-first个内容复制到result去
        return result + (last - first);
    }
    inline wchar_t* copy(const wchar_t* first, const wchar_t* last, wchar_t* result){
        memmove(result, first, sizeof(wchar_t) * (last - first));
        return result + (last - first);
    }
    
    // *************[copy_backward]****************
    template <class BidirectionalIterator1, class BidirectionalIterator2>
    inline BidirectionalIterator2 copy_backward(BidirectionalIterator1 first,
                                                 BidirectionalIterator1 last,
                                                 BidirectionalIterator2 result){
        for (; first != last; --result, --last)
            *result = *last;
        return result - (last - first);
    }
}
#endif