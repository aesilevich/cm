
// Tests template arguments of the Declaration kind: non-type template parameters
// referring to an entity with linkage

int glob_var;
int glob_arr[4];


template <int * P>
struct ptr_arg {
};


ptr_arg<&glob_var> v1;
ptr_arg<glob_arr> v2;
