// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_function.hpp
/// Contains definition of classes for template functions.

#pragma once

#include "function.hpp"
#include "template.hpp"
#include "cxx/print.hpp"


namespace cm {


/// Template function instantiation
class template_function_instantiation: virtual public function, public template_instantiation {
public:
    /// Constructs template function instantiation with specified pointer to template function
    /// and template arguments
    template <typename ... ParamsInitArgs>
    requires (std::constructible_from<template_instantiation, template_ *, ParamsInitArgs...>)
    explicit template_function_instantiation(context * ctx,
                                             template_function * templ,
                                             ParamsInitArgs && ... args);

    /// Returns function name with template parameters
    std::string name_with_templ_params() const {
        std::ostringstream str;
        // TODO: remove use of cxx printer in core classes
        cxx::print_template_instantiation_name(str, this);
        return str.str();
    }

    /// Dumps template function instantiation to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override {
        print_indent_spaces(str, indent);
        str << "template_instantiation func ";
        print_desc(str);
        function::dump_parameters_and_ret_type(str);
        str << ";\n";
    }
};


/// Template function
class template_function: virtual public named_function, virtual public template_ {
public:
    /// Constructs template function with pointer to parent context, function name,
    /// and tempalte parameters pack flag
    template_function(context * ctx, const std::string & nm):
        named_function{ctx, nm},
        function{ctx},
        template_{ctx, nm},
        named_context_entity{ctx, nm},
        context{ctx},
        context_entity{ctx} {}

    /// Destroys template function
    ~template_function() override {
        // removing all function parameters and return type before destroying
        // base template class because they can use template parameters types
        remove_all_params();
        set_ret_type({});
    }

    /// Returns function name
    const std::string & name() const override {
        return named_function::name();
    }

    // /// Creates template instantiation with specified value for initializing vector of parameters
    // template <typename ... Args>
    // requires (std::constructible_from<template_argument_desc_vector, Args...>)
    // template_function_instantiation * create_instantiation(Args && ... args) {
    //     return create_substitution_impl(
    //         template_argument_desc_vector{std::forward<Args>(args)...});
    // }

    /// Creates template instantiation with parameters pack
    template <std::convertible_to<template_argument_desc> ... Params>
    template_function_instantiation * create_instantiation(Params && ... params) {
        return create_instantiation(std::initializer_list<template_argument_desc>{
            std::forward<Params>(params)...});
    }

    /// Creates template instantiation from range of parameters
    template <template_argument_desc_range Params>
    template_function_instantiation * create_instantiation(Params && params) {
        template_argument_desc_vector vparams;
        if constexpr (std::ranges::sized_range<Params>) {
            vparams.reserve(std::ranges::size(params));
        }

        std::ranges::copy(params, std::back_inserter(vparams));
        return create_instantiation_impl(std::move(vparams));
    }

    /// Searches for template instantiation with specified arguments. Returns null if not found.
    template <typename ... Args>
    template_function_instantiation * find_instantiation(Args && ... args) {
        auto inst = template_::find_substitution(std::forward<Args>(args)...);
        if (inst == nullptr) {
            return nullptr;
        }

        auto rinst = dynamic_cast<template_function_instantiation*>(inst);
        assert(rinst != nullptr && "invalid instantiation for function remplate");
        return rinst;
    }

    /// Searches for template instantiation with specified arguments. Returns null if not found.
    template <typename ... Args>
    const template_function_instantiation * find_instantiation(Args && ... args) const {
        auto inst = template_::find_substitution(std::forward<Args>(args)...);
        if (inst == nullptr) {
            return nullptr;
        }

        auto rinst = dynamic_cast<const template_function_instantiation*>(inst);
        assert(rinst != nullptr && "invalid instantiation for function remplate");
        return rinst;
    }

    /// Dumps template function to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override {
        print_indent_spaces(str, indent);
        str << "template func " << name();
        dump_template_params(str, opts);
        dump_parameters_and_ret_type(str);
        str << ";\n";
    }

private:
    /// Creates template instantiation from vector of arguments
    template_function_instantiation *
    create_instantiation_impl(const template_argument_desc_vector & args) override {
        // creating new instantiation
        return ctx()->create_entity<template_function_instantiation>(this, args);
    }

    /// Creates template specialization from vector of arguments
    template_specialization *
    create_specialization_impl(const template_argument_desc_vector & args) override {
        // TODO
        // // creating new instantiation
        // return ctx()->create_entity<template_function_instantiation>(this, args);
        assert(false);
        return nullptr;
    }

    /// Creates template dependent instantiation from vector of arguments
    template_dependent_instantiation *
    create_dependent_instantiation_impl(const template_argument_desc_vector & args) override {
        // TODO
        // // creating new instantiation
        // return ctx()->create_entity<template_function_instantiation>(this, args);
        assert(false);
        return nullptr;
    }
};


template <typename ... ParamsInitArgs>
requires (std::constructible_from<template_instantiation, template_ *, ParamsInitArgs...>)
template_function_instantiation::template_function_instantiation(
        context * ctx,
        template_function * templ,
        ParamsInitArgs && ... args):
function{templ->ctx()},
context_entity{templ->ctx()},
template_instantiation{templ, std::forward<ParamsInitArgs>(args)...},
template_substitution{templ, std::forward<ParamsInitArgs>(args)...},
context{templ->ctx()} {
}


inline auto context::template_functions() const { return entities<template_function>(); }

inline auto context::template_functions() { return entities<template_function>(); }


}
