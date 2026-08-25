
// Test for parsing explicit instantiation of a template variable

template <typename T>
T var = T{};

template int var<int>;
