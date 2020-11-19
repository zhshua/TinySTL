#include <iostream>
#include <vector>
#include "../allocator.h"
#include "../Sources/alloc.cpp"

using namespace std;

int main()
{
    vector<int, TinySTL::allocator<int>> v;
    for (int i = 0; i < 10000000;++i)
        v.push_back(i);
    cout << v.capacity();
    return 0;
}