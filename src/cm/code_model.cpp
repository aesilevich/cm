// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file code_model.cpp
/// Contains implementation of the code_model class.

#include "pch.hpp"
#include "cm/code_model.hpp"
#include <ranges>
#include <sstream>


namespace cm {


code_model::code_model():
namespace_{nullptr, ""}, context{nullptr}, context_entity{nullptr},
opaque_type_{this, record_kind::struct_} {

    // adding builtin types
    builtin_types_.push_back({builtin_type::kind_t::void_, "void"});
    builtin_types_.push_back({builtin_type::kind_t::bool_, "bool"});
    builtin_types_.push_back({builtin_type::kind_t::char_, "char"});
    builtin_types_.push_back({builtin_type::kind_t::short_, "short"});
    builtin_types_.push_back({builtin_type::kind_t::int_, "int"});
    builtin_types_.push_back({builtin_type::kind_t::long_, "long"});
    builtin_types_.push_back({builtin_type::kind_t::long_long_, "long long"});
    builtin_types_.push_back({builtin_type::kind_t::int128_, "int128"});
    builtin_types_.push_back({builtin_type::kind_t::signed_char_, "signed char"});
    builtin_types_.push_back({builtin_type::kind_t::unsigned_char_, "unsigned char"});
    builtin_types_.push_back({builtin_type::kind_t::unsigned_short_, "unsigned short"});
    builtin_types_.push_back({builtin_type::kind_t::unsigned_int_, "unsigned int"});
    builtin_types_.push_back({builtin_type::kind_t::unsigned_long_, "unsigned long"});
    builtin_types_.push_back({builtin_type::kind_t::unsigned_long_long_, "unsigned long long"});
    builtin_types_.push_back({builtin_type::kind_t::uint128_, "uint128"});
    builtin_types_.push_back({builtin_type::kind_t::float_, "float"});
    builtin_types_.push_back({builtin_type::kind_t::double_, "double"});
    builtin_types_.push_back({builtin_type::kind_t::long_double_, "long double"});

    builtin_types_.push_back({builtin_type::kind_t::wchar_t_, "wchar_t"});
    builtin_types_.push_back({builtin_type::kind_t::char8_t_, "char8_t"});
    builtin_types_.push_back({builtin_type::kind_t::char16_t_, "char16_t"});
    builtin_types_.push_back({builtin_type::kind_t::char32_t_, "char32_t"});

    builtin_types_.push_back({builtin_type::kind_t::nullptr_t_, "nullptr_t"});

    builtin_types_.push_back({builtin_type::kind_t::complex_char_, "char complex"});
    builtin_types_.push_back({builtin_type::kind_t::complex_short_, "short complex"});
    builtin_types_.push_back({builtin_type::kind_t::complex_int_, "int complex"});
    builtin_types_.push_back({builtin_type::kind_t::complex_long_, "long complex"});
    builtin_types_.push_back({builtin_type::kind_t::complex_long_long_, "long long complex"});

    builtin_types_.push_back({builtin_type::kind_t::complex_unsigned_char_, "unsigned char complex"});
    builtin_types_.push_back({builtin_type::kind_t::complex_unsigned_short_, "unsigned short complex"});
    builtin_types_.push_back({builtin_type::kind_t::complex_unsigned_int_, "unsigned int complex"});
    builtin_types_.push_back({builtin_type::kind_t::complex_unsigned_long_, "unsigned long complex"});
    builtin_types_.push_back({builtin_type::kind_t::complex_unsigned_long_long_, "unsigned long long complex"});

    builtin_types_.push_back({builtin_type::kind_t::complex_float_, "float complex"});
    builtin_types_.push_back({builtin_type::kind_t::complex_double_, "double complex"});
    builtin_types_.push_back({builtin_type::kind_t::complex_long_double_, "long double double"});

    builtin_types_.push_back({builtin_type::kind_t::arm_sve_int8x1_, "svint8x1"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_int8x2_, "svint8x2"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_int8x2_, "svint8x3"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_int8x4_, "svint8x4"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_int16x1_, "svint16x1"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_int16x2_, "svint16x2"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_int16x2_, "svint16x3"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_int16x4_, "svint16x4"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_int32x1_, "svint32x1"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_int32x2_, "svint32x2"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_int32x2_, "svint32x3"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_int32x4_, "svint32x4"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_int64x1_, "svint64x1"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_int64x2_, "svint64x2"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_int64x2_, "svint64x3"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_int64x4_, "svint64x4"});

    builtin_types_.push_back({builtin_type::kind_t::arm_sve_uint8x1_, "svuint8x1"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_uint8x2_, "svuint8x2"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_uint8x2_, "svuint8x3"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_uint8x4_, "svuint8x4"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_uint16x1_, "svuint16x1"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_uint16x2_, "svuint16x2"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_uint16x2_, "svuint16x3"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_uint16x4_, "svuint16x4"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_uint32x1_, "svuint32x1"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_uint32x2_, "svuint32x2"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_uint32x2_, "svuint32x3"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_uint32x4_, "svuint32x4"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_uint64x1_, "svuint64x1"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_uint64x2_, "svuint64x2"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_uint64x2_, "svuint64x3"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_uint64x4_, "svuint64x4"});


    builtin_types_.push_back({builtin_type::kind_t::arm_sve_float16x1_, "svfloat16x1"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_float16x2_, "svfloat16x2"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_float16x2_, "svfloat16x3"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_float16x4_, "svfloat16x4"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_float32x1_, "svfloat32x1"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_float32x2_, "svfloat32x2"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_float32x2_, "svfloat32x3"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_float32x4_, "svfloat32x4"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_float64x1_, "svfloat64x1"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_float64x2_, "svfloat64x2"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_float64x2_, "svfloat64x3"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_float64x4_, "svfloat64x4"});

    builtin_types_.push_back({builtin_type::kind_t::arm_sve_bfloat16x1_, "svbfloat16x1"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_bfloat16x2_, "svbfloat16x2"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_bfloat16x2_, "svbfloat16x3"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_bfloat16x4_, "svbfloat16x4"});

    builtin_types_.push_back({builtin_type::kind_t::arm_sve_boolx1_, "svboolx1"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_boolx2_, "svboolx2"});
    builtin_types_.push_back({builtin_type::kind_t::arm_sve_boolx4_, "svboolx4"});

    builtin_types_.push_back({builtin_type::kind_t::arm_sve_count_, "svcount"});
}


code_model::~code_model() {
    // then removing all base classes for all records. This is required because
    // it's possible to make loop of type uses via base class, i. e.
    // class MyClass: Base<MyClass> ...
    remove_namespace_base_records(this);

    // removing all entities in global namespace
    remove_namespace_entities(this);

    // now removing all composite types (ptr/ref/arr/func) and types depended on them

    while (!ptr_types_.empty()) {
        remove_entity_and_uses(ptr_types_.first());
    }

    while (!lvalue_ref_types_.empty()) {
        remove_entity_and_uses(lvalue_ref_types_.first());
    }

    while (!rvalue_ref_types_.empty()) {
        remove_entity_and_uses(rvalue_ref_types_.first());
    }

    while (!arr_types_.empty()) {
        remove_entity_and_uses(arr_types_.first());
    }

    while (!func_types_.empty()) {
        remove_entity_and_uses(func_types_.first());
    }

    while (!vec_types_.empty()) {
        remove_entity_and_uses(vec_types_.first());
    }
}


function_type * code_model::get_or_create_func_type(function * func) {
    // creating vector of function parameter types
    std::vector<qual_type> params;
    for (auto && par : func->params()) {
        params.push_back(par->type());
    }

    return get_or_create_func_type_r(func->ret_type(), params);
}


void code_model::replace_type(type_t * src, type_t * dst) {
    // first replacing uses of all composite types with new
    // composite types from dst. Replaceing of uses of composite types
    // derived from the src type does not changes uses of src type itself.
    for (auto use : src->uses()) {
        if (auto ptr_type = dynamic_cast<pointer_type*>(use)) {
            auto new_base = ptr_type->base().replaced_type(src, dst);
            auto new_ptr = get_or_create_ptr_type(new_base);
            replace_type(ptr_type, new_ptr);
        } else if (auto ref_type = dynamic_cast<lvalue_reference_type*>(use)) {
            auto new_base = ref_type->base().replaced_type(src, dst);
            auto new_ref = get_or_create_lvalue_ref_type(new_base);
            replace_type(ref_type, new_ref);
        } else if (auto arr_type = dynamic_cast<array_type*>(use)) {
            auto new_arr = get_or_create_arr_type(dst, arr_type->size());
            replace_type(arr_type, new_arr);
        } else if (auto func_type = dynamic_cast<function_type*>(use)) {
            auto new_ret = func_type->ret_type().replaced_type(src, dst);
            auto fn = [src, dst](auto && par) { return par.replaced_type(src, dst); };
            auto new_pars = func_type->params() | std::ranges::views::transform(fn);
            auto new_ftype = get_or_create_func_type_r(new_ret, new_pars);
            replace_type(func_type, new_ftype);
        } else if (auto mtype = dynamic_cast<mem_ptr_type*>(use)) {
            auto new_obj = mtype->obj_type();

            if (new_obj == src) {
                new_obj = dst->cast<record_type>();
                assert(new_obj && "Replacement type is not an record type");
            }

            auto new_mem = mtype->mem_type().replaced_type(src, dst);
            auto new_mptr = get_or_create_mem_ptr_type(new_obj, new_mem);
            replace_type(mtype, new_mptr);
        }
    }

    // we can't use range based for because replacing of type deletes
    // uses of the original type

    auto use_it = std::begin(src->uses());
    auto end_it = std::end(src->uses());
    while (use_it != end_it) {
        auto use = *use_it;

        // skip composite types. This use will not be removed
        if (dynamic_cast<pointer_type*>(use) ||
            dynamic_cast<lvalue_reference_type*>(use) ||
            dynamic_cast<mem_ptr_type*>(use) ||
            dynamic_cast<array_type*>(use) ||
            dynamic_cast<function_type*>(use)) {

            ++use_it;
            continue;
        }

        // after this big if-else-if-else... use_it will be invalidaed
        // so we need to increase it before removing uses
        ++use_it;

        if (auto var = dynamic_cast<variable*>(use)) {
            var->set_type(var->type().replaced_type(src, dst));
        } else if (auto func = dynamic_cast<function*>(use)) {
            // function return type
            func->set_ret_type(func->ret_type().replaced_type(src, dst));
        } else if (auto param = dynamic_cast<function_parameter*>(use)) {
            param->set_type(param->type().replaced_type(src, dst));
        } else if (auto td = dynamic_cast<typedef_type*>(use)) {
            td->set_base(td->base().replaced_type(src, dst));
        } else if (auto t_arg = dynamic_cast<type_template_argument*>(use)) {
            t_arg->set_type(t_arg->type().replaced_type(src, dst));
        } else if (auto type = dynamic_cast<type_t*>(use)) {
            if (auto rtype = type->cast<record_type>()) {
                rtype->replace_base(src, dst);
            } else {
                assert(false && "don't know how to replace type use");
            }
        } else {
            assert(false && "don't know how to replace type use");
        }
    }
}


void code_model::remove_unused_composite_types() {
    bool removed = true;

    while (removed) {
        removed = false;

        auto pred = [](auto && type) { return std::ranges::empty(type->uses()); };
        removed |= ptr_types_.remove_if(pred);
        removed |= lvalue_ref_types_.remove_if(pred);
        removed |= rvalue_ref_types_.remove_if(pred);
        removed |= arr_types_.remove_if(pred);
        removed |= func_types_.remove_if(pred);
        removed |= mem_ptr_types_.remove_if(pred);
        removed |= vec_types_.remove_if(pred);
    }
}

void code_model::remove_type(type_t * type) {
    assert(type->uses().empty() && "type must have no uses");

    // detecting if type is composite (ptr/ref/func/array)
    if (auto ptr_type = type->cast<pointer_type>()) {
        ptr_types_.erase(ptr_type);
    } else if (auto ref_type = type->cast<lvalue_reference_type>()) {
        lvalue_ref_types_.erase(ref_type);
    } else if (auto ref_type = type->cast<rvalue_reference_type>()) {
        rvalue_ref_types_.erase(ref_type);
    } else if (auto arr_type = type->cast<array_type>()) {
        arr_types_.erase(arr_type);
    } else if (auto vec_type = type->cast<vector_type>()) {
        vec_types_.erase(vec_type);
    } else if (auto func_type = type->cast<function_type>()) {
        func_types_.erase(func_type);
    } else if (auto mtype = type->cast<mem_ptr_type>()) {
        mem_ptr_types_.erase(mtype);
    } else if(auto decl = type->cast<context_type>()) {

        // type is declared in some declaration context
        decl->ctx()->remove_entity(decl);

    } else {
        assert(false && "don't know how to remove type");
    }
}


/// Searches for existing source file with specified path. Returns null if not found.
const source_file * code_model::find_source(const std::filesystem::path & p,
                                            bool find_name) const {
    // trying find source with exact path match
    if (auto it = sources_.find(p); it != sources_.end()) {
        return it->second.get();
    }

    if (!find_name || p.filename() != p) {
        return nullptr;
    }

    // trying search source with matching file name
    for (auto && [path, src] : sources_) {
        if (path.filename() == p) {
            return src.get();
        }
    }

    return nullptr;
}


void code_model::dump(std::ostream & str, const dump_options & opts, unsigned int indent) const {
    namespace_::dump_entities(str, opts, indent);
}


void code_model::remove_decl_context_base_records(context * ctx) {
    // removing bases from records
    for (auto && decl : ctx->entities()) {
        // removing all base records if type is a record
        if (auto * rtype = dynamic_cast<record*>(decl)) {
            rtype->remove_all_bases();
        }

        // removing base records for all records in nested decl ctx
        if (auto nested_dctx = dynamic_cast<context*>(decl)) {
            remove_decl_context_base_records(nested_dctx);
        }
    }
}


void code_model::remove_namespace_base_records(namespace_ * ns) {
    // removing base records from namespace decl ctx
    remove_decl_context_base_records(ns);

    // removing base records from all nested namespaces
    for (auto && nested_ns : ns->namespaces()) {
        remove_namespace_base_records(nested_ns);
    }
}


void code_model::remove_entity_and_uses(entity * ent) {
    // removing all nested entities
    if (auto ctx = ent->cast<context>()) {
        auto ctx_entities = ctx->entities();
        while (!std::ranges::empty(ctx_entities)) {
            remove_entity_and_uses(*std::ranges::begin(ctx_entities));
        }
    }

    // removing all entity uses
    auto ent_uses = ent->uses();
    while (!std::ranges::empty(ent_uses)) {
        auto ent_use_ent = dynamic_cast<entity*>(*std::ranges::begin(ent_uses));
        assert(ent_use_ent && "don't know how to remove non code model entity use");

        // special case for function return type
        auto func = dynamic_cast<function*>(ent_use_ent);
        if (func && func->ret_type().type() == ent) {
            func->set_ret_type({});
        } else {
            remove_entity_and_uses(ent_use_ent);
        }
    }

    if (auto ctx_ent = ent->cast<context_entity>()) {
        // removing context entity from parent context
        ctx_ent->ctx()->remove_entity(ctx_ent);
    } else if (auto par = ent->cast<function_parameter>()) {
        // removing parameter from function
        par->func()->remove_param(par);
    } else if (auto type = ent->cast<type_t>()) {
        // removing composite type
        if (auto ptr_type = type->cast<pointer_type>()) {
            ptr_types_.erase(ptr_type);
        } else if (auto ref_type = type->cast<lvalue_reference_type>()) {
            lvalue_ref_types_.erase(ref_type);
        } else if (auto ref_type = type->cast<rvalue_reference_type>()) {
            rvalue_ref_types_.erase(ref_type);
        } else if (auto arr_type = type->cast<array_type>()) {
            arr_types_.erase(arr_type);
        } else if (auto vec_type = type->cast<vector_type>()) {
            vec_types_.erase(vec_type);
        } else if (auto func_type = type->cast<function_type>()) {
            func_types_.erase(func_type);
        } else if (auto mtype = type->cast<mem_ptr_type>()) {
            mem_ptr_types_.erase(mtype);
        } else {
            assert(false && "don't know how to remove type");
        }
    } else if (auto t_arg = ent->cast<type_template_argument>()) {
        t_arg->substitution()->remove_arg(t_arg);
    } else {
        assert(false && "don't know how to remove entity");
    }
}


void code_model::remove_namespace_entities(namespace_ * ns) {
    // removing all entities frmo this namespace
    auto entities = ns->entities();
    while (!std::ranges::empty(entities)) {
        remove_entity_and_uses(*std::ranges::begin(entities));
    }

    // removing all nested namespaces
    auto nss = ns->namespaces();
    while (!std::ranges::empty(nss)) {
        auto nested_ns = *std::ranges::begin(nss);
        remove_namespace_entities(nested_ns);
        ns->remove_namespace(nested_ns);
    }
}


}
