
// Test for parsing partial specialization of a template variable

template <typename T>
T var = T{};

template <typename T>
T * var<T*> = nullptr;

int * use() {
    return var<int*>;
}
