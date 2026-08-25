
// Tests template arguments of the Expression kind: dependent value arguments

template <int N>
struct int_arg {
};


template <int M>
struct holder {
    using type = int_arg<M>;
};
