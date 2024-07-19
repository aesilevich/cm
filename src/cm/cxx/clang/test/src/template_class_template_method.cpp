
template <typename T, int N>
class my_class {
public:
    template <typename T2>
    void foo(T x, T2 y);
};

void test_func() {
    my_class<int, 10> x;
    x.foo(10, 20.0f);
}
