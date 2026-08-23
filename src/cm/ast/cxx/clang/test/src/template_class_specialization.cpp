
// Test for parsing template class full explicit specialization definition

template <typename T>
class my_class {
    void foo(T x);
};

template <>
class my_class<int> {
    void bar();
};
