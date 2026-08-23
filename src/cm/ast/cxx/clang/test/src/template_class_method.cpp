
template <typename T, int N>
class my_class {
    void foo(T xxx, float second);
};

template <typename XT, int XN>
void my_class<XT,
    XN>::foo(XT xxx, float aaa) {
}
