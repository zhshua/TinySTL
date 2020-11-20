#include <iostream>
#include "../vector.h"
#include "../Sources/alloc.cpp"

int main()
{
    
    TinySTL::vector<int> v;
    for (int i = 0; i < 20;++i){
        v.push_back(i);
        //std::cout << v.capacity() << std::endl;
    }
    std::cout << "begin()里存储的元素为：" << *(v.begin()) << std::endl;
    /*std::cout << "v的size为：" << v.size() << std::endl;
    std::cout << "begin()里存储的元素为：" << *(v.begin()) << std::endl;
    std::cout << "end()里存储的元素为：" << *(v.end()-1) << std::endl;
    for (int i = 0; i < (int)v.size();++i)
        std::cout << v[i] << " ";
    std::cout << std::endl;
    std::cout << "v.capacity() = " << v.capacity() << std::endl;
    v.pop_back();
    std::cout << "pop掉一个元素后end() = " << v.back() << std::endl;
    
    v.erase(v.begin());
    std::cout << "erase第一个元素后begin() = " << v.front() << std::endl;
    v.insert(v.begin(), 10, 55);
    for (int i = 0; i < (int)v.size();++i)
        std::cout << v[i] << " ";
    std::cout << "v的size为：" << v.size() << std::endl;
    std::cout << "v.capacity() = " << v.capacity() << std::endl;*/
    return 0;
}