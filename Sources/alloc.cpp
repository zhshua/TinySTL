#include "../alloc.h"

namespace TinySTL{
    // 下面的四条语句为给alloc.h里的静态变量赋初值
    char *Alloc::start_free = 0;
    char *Alloc::end_free = 0;
    size_t Alloc::heap_size = 0;
    Alloc::obj *Alloc::free_list[__NFREELISTS] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

    // 此函数用于申请内存
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

    //  此函数用于释放内存
    void Alloc::deallocate(void *ptr, size_t bytes){
        // 超过128字节,则交给free释放
        if(bytes > __MAX_BYTES){
            return free(ptr);
        }
        obj **my_free_list;
        obj *q = (obj *)ptr;
        // 找到对应的my_free_list
        my_free_list = free_list + FREELIST_INDEX(bytes);
        // 调整指针进行回收, 这一步可以看作是在链表my_free_list头部插入一个节点ptr
        q->next = *my_free_list;
        *my_free_list = q;
    }

    // 此函数用于追加内存
    void *Alloc::reallocate(void* ptr, size_t old_sz, size_t new_sz){
        // 释放掉原有的旧内存
        deallocate(ptr, old_sz);
        // 重新申请一块新的大小
        ptr = allocate(new_sz);
        return ptr;
    }

    // 这个函数主要用来切割由chunk_alloc得到的大区块，并且把第一个区块返回给客户端
    void *Alloc::refill(size_t n){
        int nobjs = 20; // 20个区块
        // 尝试调用chunk_alloc函数问内存池要nobjs个大小为n的空间给free_list
        char *chunk = chunk_alloc(n, nobjs); // nobjs是按引用传递
        // 如果只得到了一个区块, 则直接返回给客户端, 否则的话把得到的大区块进行切割
        if(nobjs == 1) { return (chunk); };

        // 下面过程是把得到的大区块切割成nobjs个大小为n的小区块
        obj *result = (obj*)chunk; //第一个小区块是要返回给客户端使用的, 暂存一下地址
        obj **my_free_list;
        obj *cur_obj, *next_obj;
        // 求出在free_list内的标号
        my_free_list = free_list + FREELIST_INDEX(n);
        *my_free_list = next_obj = (obj *)(chunk + n); //由于第一块是要返回给客户端的,因此*my_free_list里存的是下一块的地址

        for (int i = 1; ;++i){ // 从1开始切割,因为第0号区块是分配给客户端的
            cur_obj = next_obj;
            next_obj = (obj *)((char *)next_obj + n); // 每个切割的区块大小为n
            if(i == nobjs - 1){ // 最后一个区块
                cur_obj->next = 0;
                break;
            }
            else{
                cur_obj->next = next_obj; // 把切割的区块串起来
            }
        }
        return (result);
    }

    // 这个函数主要用来从内存池中取空间给free_list
    // 假设size已经调整到8的倍数
    char *Alloc::chunk_alloc(size_t size, int &nobjs){
        char *result = 0;
        size_t total_bytes = size * nobjs; // 总共需要分配的字节数
        size_t bytes_left = end_free - start_free; // bytes_left表示内存池中还剩下的字节数
        if(bytes_left >= total_bytes){ // 如果内存池中剩余的空间满足需求量
            result = start_free;
            start_free += total_bytes;
            return result;
        }
        else if(bytes_left >= size){ // 内存池中剩余的空间不够总的需求量,但是至少能够满足供应一个区块
            nobjs = bytes_left / size; // 重新计算能够分配多少个区块
            total_bytes = size * nobjs;
            result = start_free;
            start_free += total_bytes;
            return result;
        }
        else{ //内存池中剩余的空间连一个区块都分配不了
            // 向malloc申请的内存大小,重新申请的大小为总量的2倍,切割后剩下的冲进内存池中
            // 每次加上一个调整到8的倍数的追加量,STL的设计着没有讲为什么追加量是heap_size >> 4,可能是团队的经验值
            size_t bytes_to_get = 2 * total_bytes + ROUND_UP(heap_size >> 4);
            // 以下试着让内存池中的残余量还有利用价值
            if(bytes_left > 0){
                obj **my_free_list = free_list + FREELIST_INDEX(bytes_left); // 找到剩余量属于哪一号链表
                // 可看作是头插节点到链表
                ((obj *)start_free)->next = *my_free_list;
                *my_free_list = (obj *)start_free;
            }
            start_free = (char *)malloc(bytes_to_get);
            if(start_free == 0){ // 假设堆空间内存已经不足以分配这么多了
                // 试着从链表中找还没用的空闲内存
                obj **my_free_list;
                obj *p;
                for (int i = size; i <= __MAX_BYTES; i+=__ALIGN){
                    my_free_list = free_list + FREELIST_INDEX(i);
                    p = *my_free_list;
                    if(p != 0){ // 如果这一号链表有空闲位置
                        // 调整free_list以释放未用的区块
                        *my_free_list = p->next;
                        start_free = (char *)p;
                        end_free = start_free + i;
                        // 递归调用自己,再申请一遍空间,以调整nobjs
                        return chunk_alloc(size, nobjs);
                    }
                }
                end_free = 0; // 假如经过上面的循环还是没找到空间,则没内存可用了
            }
            heap_size += bytes_to_get;
            end_free = start_free + bytes_to_get;
            // 递归调用自己,再申请一遍空间,以调整nobjs
            return chunk_alloc(size, nobjs);
        }
    }
}