
template <typename X1, typename Y1>
struct str {
    template <typename X2, typename Y2>
    struct nested {
        X1 my_field;
        static constexpr bool is_spec = false;
    };
};

template <typename X1, typename Y1>
template <typename T>
struct str<X1, Y1>::nested<T, Y1> {
    static constexpr bool is_spec = true;
};

static_assert(!str<int, float>::nested<double, char>::is_spec);
static_assert(str<int, float>::nested<double, float>::is_spec);
