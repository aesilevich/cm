
// Tests parameter lists of template template parameters

template <typename X, int N>
struct pair_t {
};


// unnamed inner parameters
template <template <typename, int> class T>
struct unnamed_inner {
};


// named inner parameters
template <template <typename X, int N> class T>
struct named_inner {
};


// inner parameter pack
template <template <typename ...> class T>
struct pack_inner {
};
