#ifndef _VECTOR_H_
#define _VECTOR_H_

#include "allocator.h"
#include "uninitialized.h"
#include "algorithm.h"
namespace TinySTL{
    template<class T, class Alloc = allocator<T>>
    class vector{
    public:
        // vector的嵌套型别定义
        typedef T           value_type;
        typedef T*          iterator;
        typedef T*          pointer;
        typedef T&          reference;
        typedef size_t      size_type;
        typedef ptrdiff_t   difference_type;
    protected:
        iterator start; // 表示目前使用的空间的头
        iterator finish; // 表示目前使用的空间的尾
        iterator end_of_storage; //表示目前可用的空间的尾
        typedef Alloc data_alloctor;
        
        void deallocate(){
            if(start)
                data_alloctor::deallocate(start, end_of_storage - start);// 释放空间
        }
        iterator allocate_and_fill(size_type n, const T& value){
            iterator result = data_alloctor::allocate(n); // 分配n个空间
            uninitialized_fill_n(result, n, value); // 全局函数,给区间[result, result+n)填充n个值为value的元素
            return result;
        }
        // 用于初始化填充vector
        void fill_initialize(size_type n, const T& value){
            start = allocate_and_fill(n, value);
            finish = start + n;
            end_of_storage = finish;
        }
    public:
        iterator begin() { return start; }
        iterator end() { return finish; }
        size_type size() const { return size_type(finish - start); }
        size_type capacity() const { return size_type(end_of_storage - start); }
        bool empty() const { return start == finish; }
        reference operator[](size_type n) { return *(begin() + n); }
        reference front() const { return *start; }
        reference back() const { return *(finish - 1); }

        vector() : start(nullptr), finish(nullptr), end_of_storage(nullptr){}
        vector(size_type n, const T &value) { fill_initialize(n, value); }
        explicit vector(size_type n) { fill_initialize(n, T()); }

        // vector的析构函数,以下函数要两个结合使用才能正确析构类
        ~vector(){
            destory(start, finish); // destory只析构对象,而不负责释放申请的空间
            deallocate(); // deallocate只负责释放申请的空间,而不负责析构对象
        }

        void insert_aux(iterator position, const T &x); // 在pos位置上插入值x
        void insert(iterator position, size_type n, const T &x); // 从位置pos开始插入n个初值为x的元素
        void insert(iterator position, const T &x);// 在位置pos上插入元素x

        void push_back(const T& x){
            if(finish != end_of_storage){ // 查看之前分配的空间是否已经用完了
                construct(finish, x); // 没用完的话直接构造一个对象
                ++finish;
            }
            else{// 用完的话得重新申请空间
                insert_aux(end(), x);
            }
        }

        void pop_back(){
            --finish; // 由于左闭右开原则,先--
            destory(finish);
        }

        // 删除位置pos上的某个元素
        iterator erase(iterator position){
            if(position + 1 != end())
                copy(position + 1, end(), position);// copy是全局函数,在algorithm头文件里
            --finish;
            destory(finish);
            return position;
        }

        // 删除区间[first, last)位置上的元素
        iterator erase(iterator first, iterator last){
            iterator i = copy(last, finish, first); // copy全局函数,把区间[last, finish)内的元素移动到以first为起始位置的空间上
            destory(i, finish); //移动完之后释放后面区间的对象
            finish = finish - (last - first); // 更新finish的值
            return first;
        }

        void resize(size_type new_size, const T& x){
            if(new_size < size())
                erase(begin() + new_size, end());
            else
                insert(end(), new_size - size(), x);
        }
        void resize(size_type new_size) { resize(new_size, T()); }
        void clear() { erase(begin(), end()); }
    };

    // **********************以下实现insert和insert_aux函数**********************
    // 在pos位置上插入值x
    template <class T, class Alloc>
    void vector<T, Alloc>::insert_aux(iterator position, const T&x){
        if(finish != end_of_storage){ // 原vector还有空间，不需要再申请
            construct(finish, *(finish - 1)); // 先再finish位置构造一个，并且把初值设置为最后一个元素
            ++finish; // 调整vector的size
            copy_backward(position, finish - 2, finish - 1);
            *position = x;
        }
        else{
            const size_type old_size = size();
            // 如果原大小为0,则配置一个元素,如果不为0,则配置原来大小的2倍
            const size_type len = old_size == 0 ? 1 : 2 * old_size;

            iterator new_start = data_alloctor::allocate(len); // 申请空间
            iterator new_finish = new_start;

            new_finish = uninitialized_copy(start, position, new_start); // 移动元素到新的空间
            construct(new_finish, x); // 构造插入的元素
            ++new_finish;
            // 将安插点之后的元素也拷贝过来,因为本函数也可能会被insert(p, x)调用
            new_finish = uninitialized_copy(position, finish, new_finish);

            // 析构掉原有数据,释放空间
            destory(begin(), end());
            deallocate();

            // 调整新的迭代器
            start = new_start;
            finish = new_finish;
            end_of_storage = start + len;
        }
    }

    // 从位置pos开始插入n个初值为x的元素
    template <class T, class Alloc>
    void vector<T, Alloc>::insert(iterator position, size_type n, const T& x){
        if(n != 0){
            if(size_type(end_of_storage - finish) >= n){ // 备用空间大于新增元素个数
                const size_type elems_after = finish - position; // 计算插入点之后现有的元素
                iterator old_finish = finish;
                if(elems_after > n){ // 插入点之后的元素大于要插入的元素个数
                    uninitialized_copy(finish - n, finish, finish);
                    finish += n;
                    copy_backward(position, old_finish - n, old_finish);
                    fill(position, position + n, x); // fill是algorithm里的函数
                }
                else{
                    // 插入点之后的元素个数小于要插入的元素个数
                    uninitialized_fill_n(finish, n - elems_after, x);
                    finish += n - elems_after;
                    uninitialized_copy(position, old_finish, finish);
                    finish += elems_after;
                    fill(position, old_finish, x); // fill是algorithm里的函数
                }
            }
            else{
                // 备用空间小于新增元素个数,得重新申请更大的空间
                // 新长度是旧长度的两倍,或者是旧长度 + 新增元素个数
                const size_type old_size = size();
                const size_type len = old_size + max(old_size, n);

                // 配置新空间
                iterator new_start = data_alloctor::allocate(len);
                iterator new_finish = new_start;
                // 先将插入点之前的元素复制过来
                new_finish = uninitialized_copy(start, position, new_start);
                // 再在插入点插入元素
                new_finish = uninitialized_fill_n(new_finish, n, x);
                // 再把插入点之后的元素复制过来
                new_finish = uninitialized_copy(position, finish, new_finish);

                // 析构并释放原vector
                destory(begin(), end());
                deallocate();

                // 以下调整新vector的标记
                start = new_start;
                finish = new_finish;
                end_of_storage = finish + len;
            }

        }
    }

    // 在位置pos上插入元素x
    template <class T,class Alloc>
    void vector<T, Alloc>::insert(iterator position, const T &x){
        // 如果要插入的元素在末尾
        if(finish != end_of_storage && position == end()){
            construct(end(), x);
            ++finish;
        }
        else{
            insert_aux(position, x); // 要插入的元素在中间位置,调用insert_aux函数进行插入
        }
    }
}
#endif