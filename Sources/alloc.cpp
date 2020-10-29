#include "../alloc.h"

namespace TinySTL{
    // 下面的四条语句为给alloc.h里的静态变量赋初值
    char *Alloc::start_free = 0;
    char *Alloc::end_free = 0;
    size_t Alloc::heap_size = 0;
    Alloc::obj *Alloc::free_list[__NFREELISTS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    void *Alloc::allocate(size_t bytes){
        // 超过128字节,则交给malloc分配
        if(bytes > __MAX_BYTES){
            return malloc(bytes);
        }
        obj **my_free_list; // my_free_list指的是free_list下面挂着的那16个链表其中一个
        obj *result; // 用来暂存上面说的链表

        my_free_list = free_list + FREELIST_INDEX(bytes);
        result = *my_free_list;
        // 如果没有找到可用的free_list, 则去内存池挖一块填充
        if(0 == result){
            return refill(bytes);
        }
        // 如果找到可用的, 则将my_free_list往下挪一位,然后把可用的空间返回给客户端
        *my_free_list = result->next;
        return (result);
    }

    void Alloc::dellocate(void *ptr, size_t bytes){
        // 超过128字节,则交给free释放
        if(bytes > __MAX_BYTES){
            return free(ptr);
        }
        obj **my_free_list;
        obj *q = (obj *)ptr;
        my_free_list = free_list + FREELIST_INDEX(bytes);
        q->next = *my_free_list;
        *my_free_list = q;
    }
}