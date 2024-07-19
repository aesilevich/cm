
template <typename T1, typename T2>
struct str {
    void foo(T1 x, T2 y);
};

template <>
struct str<int, float>;

template <>
struct str<int, float> {
    void bar();
};
