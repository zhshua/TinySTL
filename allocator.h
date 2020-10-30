#ifndef _ALLOCATOR_H_
#define _ALLOCATOR_H_

#include "alloc.h"

namespace TinySTL{
    template<class T>
    class allocator{
    public:
        typedef T           value_type;
        typedef T*          pointer;
        typedef T&          reference;
        typedef const T*    const_pointer;
        typedef const T&    const_reference;
        typedef size_t      size_type;
        typedef ptrdiff_t   difference_type;
    public:
        // 下面的函数主要是为了使接口符合STL标准
        static T *allocate(size_t n){
            return 0 == n ? 0 : (T *)Alloc::allocate(n * sizeof(T));
        }
        static T *allocate(){
            return (T *)Alloc::allocate(sizeof(T));
        }
        static void deallocate(T *ptr, size_t n){
            if(n != 0)
                Alloc::deallocate(ptr, n * sizeof(T));
        }
        static void deallocate(T *ptr){
            Alloc::deallocate(ptr, sizeof(T));
        }
    };
}

#endif