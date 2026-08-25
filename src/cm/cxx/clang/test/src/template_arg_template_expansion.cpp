
// Tests template arguments of the TemplateExpansion kind: expansion of a pack
// of template template parameters

template <typename T>
struct vec {
};


template <template <typename> class ... T>
struct templ_pack {
};


template <template <typename> class ... U>
struct holder {
    using type = templ_pack<U...>;
};
