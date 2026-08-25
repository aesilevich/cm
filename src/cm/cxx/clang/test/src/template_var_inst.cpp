
// Test for parsing implicit instantiation of a template variable

template <typename T>
T var = T{};

int use() {
    return var<int>;
}
