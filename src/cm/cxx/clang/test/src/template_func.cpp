
// Test for parsing template function definition and instantiation

template <typename T, int X>
int foo(double a);


int bar() {
    foo<float, 200>(20.0);
}
