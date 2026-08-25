
template <typename ... Args>
struct str {
};


template <typename ... Args>
struct str2 {
    using type = str<Args...>;
};

