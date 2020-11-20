#include <iostream>
#include "../list.h"
#include "../Sources/alloc.cpp"

int main()
{
    TinySTL::list<int> l;
    
    for (int i = 0; i < 5;++i)
        l.push_back(i);
    std::cout << "l.size() = " << l.size() << std::endl;
    return 0;
}