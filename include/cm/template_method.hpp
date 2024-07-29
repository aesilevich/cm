// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_method.hpp
/// Contains definition of the template_method class.

#pragma once

#include "template_function.hpp"
#include "record_type.hpp"


namespace cm {

class record;


/// Template method instantiation
class template_method_instantiation: virtual public method,
                                     virtual public template_function_instantiation {
public:
    /// Constructs template method instantiation
    template <typename ... ParamsInitArgs>
    requires (std::constructible_from<template_instantiation, template_ *, ParamsInitArgs...>)
    explicit template_method_instantiation(record * ctx,
                                           template_method * templ,
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
        str << "template_instantiation method ";
        print_desc(str);
        function::dump_parameters_and_ret_type(str);
        str << ";\n";
    }
};


/// Template instance function
class template_method: virtual public template_function,
                       virtual public named_method {
public:
    /// Constructs template instance function
    template_method(record * rec, const std::string & name):
        template_function{rec, name},
        template_{rec, name},
        named_method{rec, name},
        named_function{rec, name},
        function{rec},
        context{rec},
        named_context_entity{rec, name},
        context_entity{rec} {}

    /// Returns pointer to parent record
    record * ctx() {
        auto rec = dynamic_cast<record *>(context_entity::ctx());
        assert(rec && "parent context of method is not a record");
        return rec;
    }

    /// Returns const pointer record
    const record * ctx() const {
        auto rec = dynamic_cast<const record *>(context_entity::ctx());
        assert(rec && "parent context of method is not a record");
        return rec;
    }

    /// Creates template instantiation with specified template arguments
    template <typename ... Args>
    template_method_instantiation * create_instantiation(Args && ... args) {
        // checking that there is no existing template instantiation with same parameters
        assert(find_instantiation(args...) == nullptr && "template instantiation already exists");

        // creating new instantiation
        return ctx()->create_entity<template_method_instantiation>(this, std::forward<Args>(args)...);
    }

    /// Searches for template instantiation with specified arguments. Returns null if not found.
    template <typename ... Args>
    template_method_instantiation * find_instantiation(Args && ... args) {
        auto inst = template_::find_substitution(std::forward<Args>(args)...);
        if (inst == nullptr) {
            return nullptr;
        }

        auto rinst = dynamic_cast<template_method_instantiation*>(inst);
        assert(rinst != nullptr && "invalid instantiation for method remplate");
        return rinst;
    }

    /// Searches for template instantiation with specified arguments. Returns null if not found.
    template <typename ... Args>
    const template_method_instantiation * find_instantiation(Args && ... args) const {
        auto inst = template_::find_substitution(std::forward<Args>(args)...);
        if (inst == nullptr) {
            return nullptr;
        }

        auto rinst = dynamic_cast<const template_method_instantiation*>(inst);
        assert(rinst != nullptr && "invalid instantiation for method remplate");
        return rinst;
    }

    /// Dumps template function to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override {
        print_indent_spaces(str, indent);
        str << "template method " << name();
        dump_template_params(str, opts);
        dump_parameters_and_ret_type(str);
        str << ";\n";
    }

private:
    /// Creates template instantiation from vector of arguments
    template_method_instantiation *
    create_instantiation_impl(const template_argument_desc_vector & args) override {
        return create_instantiation(args);
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
template_method_instantiation::template_method_instantiation(record * ctx,
                                                             template_method * templ,
                                                             ParamsInitArgs && ... args):
method{templ->ctx()},
context_entity{templ->ctx()},
template_function_instantiation{ctx, templ, std::forward<ParamsInitArgs>(args)...},
template_substitution{templ, std::forward<ParamsInitArgs>(args)...},
function{templ->ctx()},
context{templ->ctx()} {
}


inline template_method *
record::create_template_method(const std::string & name) {
    return create_named_entity_impl<template_method>(this, name);
}


}
