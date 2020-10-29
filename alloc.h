#ifndef _ALLOC_H_
#define _ALLOC_H_

#include <cstddef>

namespace TinySTL{
    class Alloc{
    private:
        enum {__ALIGN = 8}; // 小型区块的上调边界
        enum {__MAX_BYTES = 128}; // 小型区块的上限
        enum {__NFREELISTS = __MAX_BYTES / __ALIGN}; // 维护的自由链表个数
    private:
        // free_list的节点结构体
        union obj{
            union obj *next; //指向下一个节点
        };
        // 16个自由链表, 分别负责分配区块大小为8, 16, 24, ... 128字节的空间
        static obj *free_list[__NFREELISTS];
    private:
        static char *start_free; // 内存池的起始位置
        static char *end_free; // 内存池的结束位置
        static size_t heap_size; // 堆的大小
    private:
        //根据区块大小bytes决定使用第n号free_list, n从0开始
        static size_t FREELIST_INDEX(size_t bytes){
            return (((bytes) + __ALIGN-1) / __ALIGN - 1); 
        }
        //将bytes上调至8的倍数
        static size_t ROUND_UP(size_t bytes){
            return (((bytes) + __ALIGN - 1) & ~(__ALIGN - 1)); // 暂时我还不知道为什么这样进行逻辑运算
        }
        // 这个函数主要用来从内存池中取空间给free_list
        static char *chunk_alloc(size_t size, int &nobj);
        // 这个函数主要用来切割由chunk_alloc得到的大区块，并且把第一个区块返回给客户端
        static void *refill(size_t n);
    public:
        static void *allocate(size_t bytes);
        static void *dellocate(void *ptr, size_t bytes);
        static void *reallocate(void *ptr, size_t old_sz, size_t new_sz);
    };
}

#endif