
template <typename A, A y>
void foo(A a);

template <typename T, T x>
void foo(T zzz) {
    return;
}

void bar() {
    foo<int, 10>(20);
}
