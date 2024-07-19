struct str {
    using nested = int;
};


template <typename T>
class my_class {
    using my_type = typename T::nested;
};

void bar() {
    my_class<str> c;
}
