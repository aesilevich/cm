
// Test for parsing explicit specialization of a template variable

template <typename T>
T var = T{};

template <>
int var<int> = 5;
