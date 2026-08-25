
// Tests template arguments of the StructuralValue kind: non-type template
// parameters whose value is not an integer, a declaration or a null pointer

template <double D>
struct double_arg {
};


double_arg<1.5> v;
