
// Tests template arguments of the Template kind: template template arguments

template <typename T>
struct vec {
};


template <template <typename> class T>
struct templ_arg {
};


templ_arg<vec> v;
