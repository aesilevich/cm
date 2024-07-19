
// Test for parsing template class definition

template <typename TYPE, int NUM>
class my_class;

template <typename T, int N>
class my_class {
    void foo(T x);
};

my_class<int, 10
> x;
