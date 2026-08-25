
// Test for parsing template parameter packs in different positions

// trailing type parameter pack
template <typename ... Types>
struct tuple_t;

// type parameter followed by a parameter pack
template <typename First, typename ... Rest>
struct list_t;

// non-type parameter pack
template <int ... Values>
struct int_sequence;

// function template with a parameter pack that is not the last parameter
template <typename ... Types, typename Last>
void func_pack_not_last();

// function template with two parameter packs
template <typename ... First, typename ... Second>
void func_two_packs();
