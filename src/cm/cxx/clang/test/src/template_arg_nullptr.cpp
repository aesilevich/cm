
// Tests template arguments of the NullPtr kind

template <int * P>
struct ptr_arg {
};


ptr_arg<nullptr> v;
