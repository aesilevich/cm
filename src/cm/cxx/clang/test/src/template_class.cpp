
// Test for parsing template class definition

template <typename T, T N>
class my_class;

template <typename T, T N>
class my_class {
    typedef my_class<T, N> this_type;
    T var;
    void func(int x);
};

my_class<int, 10> x;
