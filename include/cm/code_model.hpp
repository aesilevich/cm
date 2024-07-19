// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file code_model.hpp
/// Contains definition of the code_model class.

#pragma once

#include "array_type.hpp"
#include "builtin_type.hpp"
#include "concat.hpp"
#include "decltype_type.hpp"
#include "dependent_type.hpp"
#include "function_type.hpp"
#include "mem_ptr_type.hpp"
#include "named_type.hpp"
#include "namespace.hpp"
#include "pointer_type.hpp"
#include "qual_type.hpp"
#include "record_type.hpp"
#include "lvalue_reference_type.hpp"
#include "rvalue_reference_type.hpp"
#include "source_file.hpp"
#include "template_method.hpp"
#include "template_function.hpp"
#include "template_record.hpp"
#include "template_specialization.hpp"
#include "type.hpp"
#include "typedef_type.hpp"
#include "variable.hpp"
#include "vector_type.hpp"
#include <ranges>
#include <array>
#include <filesystem>
#include <unordered_map>
#include <tuple>


namespace cm {


/// Hash function for filesystem path
/// NOTE: We need this because of problems with std::hash<path> in libc++
struct fs_path_hash {
    auto operator()(const std::filesystem::path & p) const noexcept {
        return std::filesystem::hash_value(p);
    }
};


/// Map of composite types in code model such as arrays/functions/pointers
template <typename TypeDesc, typename Type>
class composite_type_map {
    using map_type = std::unordered_map<TypeDesc, std::unique_ptr<Type>>;

public:
    /// Gets previously created composite type or creates new
    /// using specified parameters
    template <typename ... TypeParams>
    Type * get_or_create(TypeParams && ... params) {
        // creating type description
        TypeDesc desc{params...};

        // looking for existing type
        auto & res = types_[desc];
        if (res) {
            return res.get();
        }

        // creating new type
        res.reset(new Type{params...});
        return res.get();
    }

    /// Removes specified type from map
    void erase(Type * t) {
        auto cnt = types_.erase(t->type_id());
        assert(cnt != 0 && "type not found in map");
    }

    /// Removes all types from map
    void clear() {
        types_.clear();
    }

    /// Returns pointer to the first type in map
    Type * first() {
        assert(!types_.empty() && "map is empty");
        return types_.begin()->second.get();
    }

    /// Returns true if map if empty
    bool empty() const {
        return types_.empty();
    }

    /// Removes all types matching predicate. Returns true if something
    /// was removed
    template <typename Pred>
    bool remove_if(const Pred & pred) {
        bool removed = false;
        for (auto it = types_.begin(), end = types_.end(); it != end;) {
            if (pred(it->second)) {
                auto curr_it = it;
                ++it;
                types_.erase(curr_it);
                removed = true;
            } else {
                ++it;
            }
        }

        return removed;
    }

    /// Returns range of pointers to types
    auto types() {
        // we don't use lambda because lambdas are not copy assignable
        // by default, and we need it for join.
        // NOTE: can be replaced with labmda in C++20.
        struct get_ptr_t {
            typedef Type * result_type;
            Type * operator()(const typename map_type::value_type & pair) const {
                return pair.second.get();
            }
        };

        return types_ | std::ranges::views::transform(get_ptr_t());
    }

    /// Returns range of const pointers to types
    auto types() const {
        // we don't use lambda because lambdas are not copy assignable
        // by default, and we need it for join.
        // NOTE: can be replaced with labmda in C++20.
        struct get_ptr_t {
            typedef const Type * result_type;
            const Type * operator()(const typename map_type::value_type & pair) const {
                return const_cast<const Type*>(pair.second.get());
            }
        };

        return types_ | std::ranges::views::transform(get_ptr_t());
    }

private:
    /// Map of types
    map_type types_;
};


/// Macro for defining builtin type accessors
#define CM_CODE_MODEL_DEF_BT_ACCESSOR(bt_name) \
    const builtin_type * bt_##bt_name() const { \
        return &builtin_types_[static_cast<int>(builtin_type::kind_t::bt_name##_)]; \
    } \
    \
    builtin_type * bt_##bt_name() { \
        return &builtin_types_[static_cast<int>(builtin_type::kind_t::bt_name##_)]; \
    }


/// Code model
class code_model: public namespace_ {
public:
    /// Constructor, initializes code model
    code_model();

    /// Deleted copy constructor
    code_model(const code_model &) = delete;

    /// Deleted move constructor
    code_model(code_model &&) = delete;

    /// Destroys code model and removes all its contents
    virtual ~code_model();

    // Builtin type accessors
    CM_CODE_MODEL_DEF_BT_ACCESSOR(void)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(bool)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(char)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(short)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(int)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(long)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(long_long)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(int128)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(signed_char)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(unsigned_char)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(unsigned_short)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(unsigned_int)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(unsigned_long)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(unsigned_long_long)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(uint128)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(float)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(double)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(long_double)

    CM_CODE_MODEL_DEF_BT_ACCESSOR(wchar_t)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(char8_t)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(char16_t)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(char32_t)

    CM_CODE_MODEL_DEF_BT_ACCESSOR(complex_char)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(complex_short)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(complex_int)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(complex_long)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(complex_long_long)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(complex_unsigned_char)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(complex_unsigned_short)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(complex_unsigned_int)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(complex_unsigned_long)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(complex_unsigned_long_long)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(complex_float)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(complex_double)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(complex_long_double)

    // ARM SVE

    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_int8x1)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_int8x2)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_int8x3)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_int8x4)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_int16x1)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_int16x2)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_int16x3)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_int16x4)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_int32x1)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_int32x2)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_int32x3)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_int32x4)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_int64x1)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_int64x2)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_int64x3)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_int64x4)

    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_uint8x1)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_uint8x2)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_uint8x3)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_uint8x4)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_uint16x1)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_uint16x2)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_uint16x3)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_uint16x4)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_uint32x1)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_uint32x2)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_uint32x3)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_uint32x4)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_uint64x1)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_uint64x2)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_uint64x3)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_uint64x4)

    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_float16x1)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_float16x2)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_float16x3)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_float16x4)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_float32x1)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_float32x2)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_float32x3)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_float32x4)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_float64x1)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_float64x2)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_float64x3)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_float64x4)

    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_bfloat16x1)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_bfloat16x2)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_bfloat16x3)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_bfloat16x4)

    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_boolx1)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_boolx2)
    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_boolx4)

    CM_CODE_MODEL_DEF_BT_ACCESSOR(arm_sve_count)

    /// Returns pointer to builtin type of specified kind
    const auto bt_type(builtin_type::kind_t kind) const {
        return &builtin_types_[static_cast<int>(kind)];
    }

    /// Returns pointer to builtin type of specified kind
    auto bt_type(builtin_type::kind_t kind) {
        return &builtin_types_[static_cast<int>(kind)];
    }

    /// Returns pointer to opaque type
    auto * opaque_type() { return &opaque_type_; }

    /// Returns pointer to opaque type
    auto * opaque_type() const { return &opaque_type_; }

    /// Returns or creates new pointer to type
    pointer_type * get_or_create_ptr_type(const qual_type & pointee) {
        return ptr_types_.get_or_create(pointee);
    }

    /// Returns existing or creates new array type
    array_type * get_or_create_arr_type(type_t * base, uint64_t sz) {
        return arr_types_.get_or_create(base, sz);
    }

    /// Returns existing or creates new vector type
    vector_type * get_or_create_vec_type(builtin_type * base, uint64_t sz) {
        return vec_types_.get_or_create(base, sz);
    }

    /// Returns or creates new lvalue reference type
    lvalue_reference_type * get_or_create_lvalue_ref_type(const qual_type & pointee) {
        return lvalue_ref_types_.get_or_create(pointee);
    }

    /// Returns or creates new rvalue reference type
    rvalue_reference_type * get_or_create_rvalue_ref_type(const qual_type & pointee) {
        return rvalue_ref_types_.get_or_create(pointee);
    }

    /// Gets existing or creates new function type with specified return type
    /// and range of parameter type
    template <typename Params>
    function_type * get_or_create_func_type_r(const qual_type & ret, Params && params) {
        return func_types_.get_or_create(ret, std::forward<Params>(params));
    }

    /// Gets existing or creates new function type with specified return type
    /// and initializer list of parameter types
    function_type * get_or_create_func_type_r(const qual_type & ret,
                                              const std::initializer_list<qual_type> & params) {
        using Params = const std::initializer_list<qual_type> &;
        return get_or_create_func_type_r<Params>(ret, params);
    }

    /// Gets existing or creates new function type with specified return type
    /// and parameter types
    template <typename ... Params>
    function_type * get_or_create_func_type(const qual_type & ret, Params && ... pars) {
        return get_or_create_func_type_r(ret, {pars...});
    }

    /// Gets existing or creates new function type for specified function
    function_type * get_or_create_func_type(function * func);

    /// Gets existing or creates new pointer to member type
    mem_ptr_type * get_or_create_mem_ptr_type(record_type * obj_type,
                                              const qual_type & mem_type) {
        return mem_ptr_types_.get_or_create(obj_type, mem_type);
    }

    /// Replaces all uses of type with another type
    void replace_type(type_t * src, type_t * dst);

    /// Removes all unused composite types
    void remove_unused_composite_types();

    /// Returns range of all pointer types
    auto ptr_types() { return ptr_types_.types(); }

    /// Returns range of all lvalue reference types
    auto lvalue_ref_types() { return lvalue_ref_types_.types(); }

    /// Returns range of all rvalue reference types
    auto rvalue_ref_types() { return rvalue_ref_types_.types(); }

    /// Returns range of all array types
    auto arr_types() { return arr_types_.types(); }

    /// Returns range of all function types
    auto func_types() { return func_types_.types(); }

    /// Returns range of all pointer to member types
    auto mem_ptr_types() { return mem_ptr_types_.types(); }

    /// Returns range of all composite types in code model
    auto composite_types() {
        // we don't use lambda because lambdas are not copy assignable
        // by default, and we need it for join.
        // NOTE: can be replaced with labmda in C++20.
        struct cast_fn_t {
            typedef type_t * result_type;
            type_t * operator()(type_t * t) const { return t; }
        };

        auto ptrs = ptr_types() | std::ranges::views::transform(cast_fn_t());
        auto lrefs = lvalue_ref_types() | std::ranges::views::transform(cast_fn_t());
        auto rrefs = rvalue_ref_types() | std::ranges::views::transform(cast_fn_t());
        auto arrs = arr_types() | std::ranges::views::transform(cast_fn_t());
        auto funcs = func_types() | std::ranges::views::transform(cast_fn_t());
        auto mem_ptrs = mem_ptr_types() | std::ranges::views::transform(cast_fn_t());

        auto tmp1 = ranges::views::concat(ptrs, lrefs);
        auto tmp2 = ranges::views::concat(tmp1, rrefs);
        auto tmp3 = ranges::views::concat(tmp2, arrs);
        auto tmp4 = ranges::views::concat(tmp3, funcs);
        auto tmp5 = ranges::views::concat(tmp4, mem_ptrs);
        return tmp5;
    }

    /// Removes specified type. Type must have no uses
    void remove_type(type_t * type);

    /// Searches for existing source file with specified path. Returns null if not found.
    const source_file * find_source(const std::filesystem::path & p, bool find_name = false) const;

    /// Gets existing or creates new source file object with specified path
    const source_file * source(const std::filesystem::path & p) {
        auto & res = sources_[p];
        if (!res) {
            res = std::make_unique<source_file>(p);
        }

        return res.get();
    }

    /// Dumps code model to output stream
    void dump(std::ostream & str,
              const dump_options & opts = {},
              unsigned int indent = 0) const override;

private:
    /// Recursively removes all base records for all record types in decl context
    /// and itss nested decl contexts
    void remove_decl_context_base_records(context * ctx);

    /// Recursively removes all base records for all record types in namespace
    /// and its nested decl contexts and namespaces
    void remove_namespace_base_records(namespace_ * ns);

    /// Recursively removes entity and all its uses
    void remove_entity_and_uses(entity * ent);

    /// Recursively removes all entities in namespace
    void remove_namespace_entities(namespace_ * ns);

    std::vector<builtin_type> builtin_types_;       ///< Predefined builtin types
    record_type opaque_type_;                       ///< Opaque type

    /// Map of pointer types
    composite_type_map<const_qual_type, pointer_type> ptr_types_;

    /// Map of lvalue reference types
    composite_type_map<const_qual_type, lvalue_reference_type> lvalue_ref_types_;

    /// Map of rvalue reference types
    composite_type_map<const_qual_type, rvalue_reference_type> rvalue_ref_types_;

    /// Map of array types
    composite_type_map<array_type_id, array_type> arr_types_;

    /// Map of vector types
    composite_type_map<vector_type_id, vector_type> vec_types_;

    /// Map of function types
    composite_type_map<function_type_id, function_type> func_types_;

    /// Map of pointer to member types
    composite_type_map<mem_ptr_type_id, mem_ptr_type> mem_ptr_types_;

    /// Map os source file objects
    std::unordered_map<std::filesystem::path, std::unique_ptr<source_file>, fs_path_hash> sources_;
};


inline pointer_type * type_t::ptr(code_model & cm) {
    return cm.get_or_create_ptr_type(this);
}

inline lvalue_reference_type * type_t::ref(code_model & cm) {
    return cm.get_or_create_lvalue_ref_type(this);
}


}
