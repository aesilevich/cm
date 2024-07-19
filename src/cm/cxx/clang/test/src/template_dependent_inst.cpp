
template <typename T>
struct nested_t {};

template <typename T>
struct str {
    nested_t<T> x;
};
