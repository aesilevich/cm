// Copyright (c) 2024, Alexandr Esilevich
//
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_variable.hpp
/// Contains definitons of classes for template variables.

#pragma once

#include "template_specialization.hpp"
#include "variable.hpp"


namespace cm {

class template_variable;
class template_variable_partial_specialization;


/// Template variable substitution
class template_variable_substitution: virtual public template_substitution {
public:
    /// Constructs template variable substitution from pack of template arguments
    template <std::convertible_to<template_argument_desc> ... Args>
    explicit template_variable_substitution(context * ctx,
                                            template_variable * templ,
                                            Args && ... args);

    /// Constructs template variable substitution from range of template arguments
    template <template_argument_desc_range Args>
    explicit template_variable_substitution(context * ctx,
                                            template_variable * templ,
                                            Args && args);
};


/// Template variable instantiation
class template_variable_instantiation: public variable,
                                       virtual public template_variable_substitution,
                                       virtual public template_instantiation {
public:
    /// Constructs template variable instantiation from pack of template arguments
    template <std::convertible_to<template_argument_desc> ... Args>
    explicit template_variable_instantiation(context * ctx,
                                             const std::string & nm,
                                             template_variable * templ,
                                             Args && ... args);

    /// Constructs template variable instantiation from range of template arguments
    template <template_argument_desc_range Args>
    explicit template_variable_instantiation(context * ctx,
                                             const std::string & nm,
                                             template_variable * templ,
                                             Args && args);

    /// Prints entity short description to output stream
    void print_desc(std::ostream & str) const override {
        template_substitution::print_desc(str);
    }

    /// Dumps template variable instantiation to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override {
        print_indent_spaces(str, indent);
        str << "template_instantiation ";
        dump_header(str);
    }

protected:
    /// Dumps variable kind, name with template arguments and type to output stream
    void dump_header(std::ostream & str) const {
        str << "var ";
        print_desc(str);
        str << ": ";
        type().print_desc(str);
        str << ";\n";
    }
};


/// Template variable specialization
class template_variable_specialization: public template_variable_instantiation,
                                        public template_specialization {
public:
    /// Constructs template variable specialization from pack of template arguments
    template <std::convertible_to<template_argument_desc> ... Args>
    explicit template_variable_specialization(context * ctx,
                                              const std::string & nm,
                                              template_variable * templ,
                                              Args && ... args);

    /// Constructs template variable specialization from range of template arguments
    template <template_argument_desc_range Args>
    explicit template_variable_specialization(context * ctx,
                                              const std::string & nm,
                                              template_variable * templ,
                                              Args && args);

    /// Dumps template variable specialization to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override {
        print_indent_spaces(str, indent);
        str << "template_specialization ";
        dump_header(str);
    }
};


/// Template variable
class template_variable: public variable, public template_ {
public:
    /// Constructs template variable with specified parent context and name.
    /// Type of the variable is empty and should be set after converting template parameters.
    explicit template_variable(context * ctx, const std::string & nm):
        variable{ctx, nm, {}},
        template_{ctx, nm},
        named_context_entity{ctx, nm},
        context{ctx},
        context_entity{ctx},
        single_type_use{{}} {}

    /// Destroys template variable
    ~template_variable() override {
        // removing the variable type before destroying the base template class because
        // it can use template parameters types
        set_type({});
    }

    /// Returns template variable name
    const std::string & name() const override {
        return named_context_entity::name();
    }

    /// Returns default access level for this context
    access_level default_access_level() const override {
        return access_level::public_;
    }


    //////////////////////////////////////////////////////////////////////
    // Template variable instantiations

    /// Searches for template variable instantiation for specified arguments
    template <typename ... Args>
    template_variable_instantiation * find_instantiation(Args && ... args) {
        return find_substitution<template_variable_instantiation>(std::forward<Args>(args)...);
    }

    /// Searches for template variable instantiation for specified arguments
    template <typename ... Args>
    const template_variable_instantiation * find_instantiation(Args && ... args) const {
        return find_substitution<template_variable_instantiation>(std::forward<Args>(args)...);
    }

    /// Creates template variable instantiation from template arguments
    template <typename ... Args>
    template_variable_instantiation * create_instantiation(Args && ... args) {
        return create_named_substitution_impl<template_variable_instantiation>(
            this, std::forward<Args>(args)...);
    }


    //////////////////////////////////////////////////////////////////////
    // Template variable specializations

    /// Searches for template variable specialization for specified arguments
    template <typename ... Args>
    template_variable_specialization * find_specialization(Args && ... args) {
        return find_substitution<template_variable_specialization>(std::forward<Args>(args)...);
    }

    /// Searches for template variable specialization for specified arguments
    template <typename ... Args>
    const template_variable_specialization * find_specialization(Args && ... args) const {
        return find_substitution<template_variable_specialization>(std::forward<Args>(args)...);
    }

    /// Creates template variable specialization from template arguments
    template <typename ... Args>
    template_variable_specialization * create_specialization(Args && ... args) {
        return create_named_substitution_impl<template_variable_specialization>(
            this, std::forward<Args>(args)...);
    }


    //////////////////////////////////////////////////////////////////////
    // Template variable partial specializations

    /// Creates template variable partial specialization
    inline template_variable_partial_specialization * create_partial_specialization();


    //////////////////////////////////////////////////////////////////////
    // Printing

    /// Prints entity short description to output stream
    void print_desc(std::ostream & str) const override {
        str << name();
        dump_template_params(str, {false, false});
    }

    /// Dumps template variable to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override {
        print_indent_spaces(str, indent);
        str << "template var " << name();
        dump_template_params(str, opts);
        str << ": ";
        type().print_desc(str);
        str << ";\n";
    }

private:
    /// Creates template instantiation from vector of arguments
    template_variable_instantiation *
    create_instantiation_impl(const template_argument_desc_vector & args) override {
        return create_instantiation(args);
    }

    /// Creates template specialization from vector of arguments
    template_variable_specialization *
    create_specialization_impl(const template_argument_desc_vector & args) override {
        return create_specialization(args);
    }

    /// Creates template dependent instantiation from vector of arguments
    template_dependent_instantiation *
    create_dependent_instantiation_impl(const template_argument_desc_vector & args) override {
        // dependent instantiation is a type and can't be created for a variable
        assert(false && "template variable can't have dependent instantiation");
        return nullptr;
    }
};


/// Template variable partial specialization
class template_variable_partial_specialization: public variable,
                                                public templated_entity,
                                                public template_substitution {
public:
    /// Constructs template variable partial specialization
    explicit template_variable_partial_specialization(context * ctx,
                                                      const std::string & nm,
                                                      template_variable * templ):
        variable{ctx, nm, {}},
        templated_entity{ctx},
        template_substitution{templ},
        named_context_entity{ctx, nm},
        context{ctx},
        context_entity{ctx},
        single_type_use{{}} {}

    /// Destroys template variable partial specialization
    ~template_variable_partial_specialization() override {
        // removing the variable type before destroying the base templated entity class
        // because it can use template parameters types
        set_type({});
    }

    /// Returns default access level for this context
    access_level default_access_level() const override {
        return access_level::public_;
    }

    /// Prints entity short description to output stream
    void print_desc(std::ostream & str) const override {
        template_substitution::print_desc(str);
    }

    /// Dumps template variable partial specialization to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override {
        print_indent_spaces(str, indent);
        str << "template_partial_specialization";
        dump_template_params(str, opts);
        str << " var ";
        print_desc(str);
        str << ": ";
        type().print_desc(str);
        str << ";\n";
    }
};


//////////////////////////////////////////////////////////////////////
// Implementation


template <std::convertible_to<template_argument_desc> ... Args>
template_variable_substitution::template_variable_substitution(context * ctx,
                                                               template_variable * templ,
                                                               Args && ... args):
template_substitution{templ, std::forward<Args>(args)...},
context_entity{ctx} {}


template <template_argument_desc_range Args>
template_variable_substitution::template_variable_substitution(context * ctx,
                                                               template_variable * templ,
                                                               Args && args):
template_substitution{templ, std::forward<Args>(args)},
context_entity{ctx} {}


template <std::convertible_to<template_argument_desc> ... Args>
template_variable_instantiation::template_variable_instantiation(context * ctx,
                                                                 const std::string & nm,
                                                                 template_variable * templ,
                                                                 Args && ... args):
variable{ctx, nm, {}},
template_instantiation{templ, std::forward<Args>(args)...},
template_variable_substitution{ctx, templ, std::forward<Args>(args)...},
template_substitution{templ, std::forward<Args>(args)...},
named_context_entity{ctx, nm},
context_entity{ctx},
single_type_use{{}} {}


template <template_argument_desc_range Args>
template_variable_instantiation::template_variable_instantiation(context * ctx,
                                                                 const std::string & nm,
                                                                 template_variable * templ,
                                                                 Args && args):
variable{ctx, nm, {}},
template_instantiation{templ, std::forward<Args>(args)},
template_variable_substitution{ctx, templ, std::forward<Args>(args)},
template_substitution{templ, std::forward<Args>(args)},
named_context_entity{ctx, nm},
context_entity{ctx},
single_type_use{{}} {}


template <std::convertible_to<template_argument_desc> ... Args>
template_variable_specialization::template_variable_specialization(context * ctx,
                                                                   const std::string & nm,
                                                                   template_variable * templ,
                                                                   Args && ... args):
template_variable_instantiation{ctx, nm, templ, std::forward<Args>(args)...},
template_specialization{ctx, templ, std::forward<Args>(args)...},
template_instantiation{templ, std::forward<Args>(args)...},
template_variable_substitution{ctx, templ, std::forward<Args>(args)...},
template_substitution{templ, std::forward<Args>(args)...},
named_context_entity{ctx, nm},
context_entity{ctx},
single_type_use{{}} {}


template <template_argument_desc_range Args>
template_variable_specialization::template_variable_specialization(context * ctx,
                                                                   const std::string & nm,
                                                                   template_variable * templ,
                                                                   Args && args):
template_variable_instantiation{ctx, nm, templ, std::forward<Args>(args)},
template_specialization{ctx, templ, std::forward<Args>(args)},
template_instantiation{templ, std::forward<Args>(args)},
template_variable_substitution{ctx, templ, std::forward<Args>(args)},
template_substitution{templ, std::forward<Args>(args)},
named_context_entity{ctx, nm},
context_entity{ctx},
single_type_use{{}} {}


inline template_variable_partial_specialization *
template_variable::create_partial_specialization() {
    return ctx()->create_named_entity<template_variable_partial_specialization>(name(), this);
}


inline auto context::template_variables() const { return entities<template_variable>(); }

inline auto context::template_variables() { return entities<template_variable>(); }


}
