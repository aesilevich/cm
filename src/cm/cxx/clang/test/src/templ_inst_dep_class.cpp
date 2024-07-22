
/// Tests template instantiation with dependency on class declared later

template <typename T>
void foo(T x) {
}

struct str {};

void bar() {
    foo<str>(str{});
}
