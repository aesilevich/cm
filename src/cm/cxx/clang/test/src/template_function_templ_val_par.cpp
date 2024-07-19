
// Tests parsing template function with value parameter of template type

template <typename T, T val>
void foo() {
}

void bar() {
    foo<int, 10>();
}
