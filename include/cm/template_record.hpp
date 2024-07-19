// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_record.hpp
/// Contains definitons of classes for template records.

#pragma once

#include "record_type.hpp"
#include "template_dependent_instantiation.hpp"
#include "template_specialization.hpp"


namespace cm {

class template_record_partial_specialization;


/// Template record substitution
class template_record_substitution: virtual public template_substitution {
public:
    /// Constructs template record substitution from pack of template arguments
    template <std::convertible_to<template_argument_desc> ... Args>
    explicit template_record_substitution(context * ctx,
                                          template_record * templ,
                                          Args && ... args);

    /// Constructs template record substitution from range of template arguments
    template <template_argument_desc_range Args>
    explicit template_record_substitution(context * ctx,
                                          template_record * templ,
                                          Args && args);
};


/// Template record instantiation
class template_record_instantiation_type: public record_type,
                                          public template_record_substitution,
                                          virtual public template_instantiation {
public:
    /// Constructs template record instantiation from pack of template arguments
    template <std::convertible_to<template_argument_desc> ... Args>
    explicit template_record_instantiation_type(context * ctx,
                                                template_record * templ,
                                                Args && ... args);

    /// Constructs template record instantiation from range of template arguments
    template <template_argument_desc_range Args>
    explicit template_record_instantiation_type(context * ctx,
                                                template_record * templ,
                                                Args && args);

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const override {
        template_instantiation::print_desc(str);
    }

    /// Dumps template record instantiation to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override {
        print_indent_spaces(str, indent);
        str << "template_instantiation ";
        dump_header_and_contents(str, opts, indent);
    }
};


/// Template record specialization
class template_record_specialization_type: public template_record_instantiation_type,
                                           public template_specialization {
public:
    /// Constructs template record specialization from pack of template arguments
    template <std::convertible_to<template_argument_desc> ... Args>
    explicit template_record_specialization_type(context * ctx,
                                                 template_record * templ,
                                                 Args && ... args);

    /// Constructs template record instantiation from range of template arguments
    template <template_argument_desc_range Args>
    explicit template_record_specialization_type(context * ctx,
                                                 template_record * templ,
                                                 Args && args);

    /// Dumps template record specialization to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override {
        print_indent_spaces(str, indent);
        str << "template_specialization ";
        dump_header_and_contents(str, opts, indent);
    }
};


/// Template record dependent instantiation type
class template_record_dependent_instantiation_type: public template_dependent_instantiation,
                                                    public template_record_substitution {
public:
    /// Constructs template record dependent instantiation from pack of template arguments
    template <std::convertible_to<template_argument_desc> ... Args>
    explicit template_record_dependent_instantiation_type(context * ctx,
                                                          template_record * templ,
                                                          Args && ... args);

    /// Constructs template record instantiation from range of template arguments
    template <template_argument_desc_range Args>
    explicit template_record_dependent_instantiation_type(context * ctx,
                                                          template_record * templ,
                                                          Args && args);

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const override {
        template_substitution::print_desc(str);
    }

    /// Dumps template record dependent instantiation to output stream
    inline void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override;
};


/// Represents this template record type inside context of template, i.e.:
/// template <typename T> struct str { str<T> * x; };
/// Does not have template arguments
class template_record_type: public context_type {
public:
    /// Constructs template record type
    explicit template_record_type(context * ctx);

    /// Returns template record associated with this type
    template_record * templ();

    /// Returns template record associated with this type
    const template_record * templ() const;

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const override;

    /// Dumps template record type to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override;
};


/// Template record
class template_record: public record, public template_ {
public:
    /// Constructs template record with specified parent decl context, name, kind,
    /// parameter pack flag, and template parameters
    explicit template_record(context * ctx, const std::string & nm, record_kind knd):
    record{ctx, knd},
    template_{ctx, nm},
    named_context_entity{ctx, nm},
    context{ctx},
    context_entity{ctx} {
        init();
    }

    /// Constructs template record with specified parent decl context, name, kind,
    /// parameter pack flag, and list of named template parameters
    template <typename ... Params>
    explicit template_record(context * ctx,
                             const std::string & nm,
                             record_kind knd,
                             Params && ... params):
    record{ctx, knd},
    template_{ctx, nm, std::forward<Params>(params)...},
    named_context_entity{ctx, nm},
    context{ctx},
    context_entity{ctx} {
        init();
    }


    //////////////////////////////////////////////////////////////////////
    // Template record instantiations

    /// Returns pointer to injected this template record type that can be
    /// used in template context
    template_record_type * this_type();

    /// Returns const pointer to injected this template record type that can be
    /// used in template context
    const template_record_type * this_type() const;

    /// Searches for template record instantiation for specified arguments
    template <typename ... Args>
    template_record_instantiation_type * find_instantiation(Args && ... args) {
        return find_substitution<template_record_instantiation_type>(std::forward<Args>(args)...);
    }

    /// Searches for template record instantiation for specified arguments
    template <typename ... Args>
    const template_record_instantiation_type * find_instantiation(Args && ... args) const {
        return find_substitution<template_record_instantiation_type>(std::forward<Args>(args)...);
    }

    /// Creates template record instantiation from template arguments
    template <typename ... Args>
    template_record_instantiation_type * create_instantiation(Args && ... args) {
        return create_substitution_impl<template_record_instantiation_type>(
            this, std::forward<Args>(args)...);
    }


    //////////////////////////////////////////////////////////////////////
    // Template record specializations

    /// Searches for template record specialization for specified arguments
    template <typename ... Args>
    template_record_specialization_type * find_specialization(Args && ... args) {
        return find_substitution<template_record_specialization_type>(std::forward<Args>(args)...);
    }

    /// Searches for template record specialization for specified arguments
    template <typename ... Args>
    const template_record_specialization_type * find_specialization(Args && ... args) const {
        return find_substitution<template_record_specialization_type>(std::forward<Args>(args)...);
    }

    /// Creates template record specialization from template arguments
    template <typename ... Args>
    template_record_specialization_type * create_specialization(Args && ... args) {
        return create_substitution_impl<template_record_specialization_type>(
            this, std::forward<Args>(args)...);
    }


    //////////////////////////////////////////////////////////////////////
    // Template record dependent instantiations

    /// Searches for template record dependent instantiation for specified arguments
    template <typename ... Args>
    template_record_dependent_instantiation_type *
    find_dependent_instantiation(Args && ... args) {
        return find_substitution<template_record_dependent_instantiation_type>(
            std::forward<Args>(args)...);
    }

    /// Searches for template record dependent instantiation for specified arguments
    template <typename ... Args>
    const template_record_dependent_instantiation_type *
    find_dependent_instantiation(Args && ... args) const {
        return find_substitution<template_record_dependent_instantiation_type>(
            std::forward<Args>(args)...);
    }

    /// Creates template record dependent instantiation from template arguments
    template <typename ... Args>
    template_record_dependent_instantiation_type *
    create_dependent_instantiation(Args && ... args) {
        return create_substitution_impl<template_record_dependent_instantiation_type>(
            this, std::forward<Args>(args)...);
    }


    //////////////////////////////////////////////////////////////////////
    // Template record partial specializations

    /// Creates template record partial specialization
    inline template_record_partial_specialization * create_partial_specialization();


    //////////////////////////////////////////////////////////////////////
    // Printing

    /// Prints entity short description to output stream
    void print_desc(std::ostream & str) const override {
        str << name();
        dump_template_params(str, {false, false});
    }

    /// Dumps template record to output stream
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override {
        print_indent_spaces(str, indent);
        str << "template ";
        dump_header_and_contents(str, opts, indent);
    }

private:
    /// Initializes template record. Creates injected template record type in record context.
    void init() {
        create_entity<template_record_type>();
    }

    /// Creates template instantiation from vector of arguments
    template_record_instantiation_type *
    create_instantiation_impl(const template_argument_desc_vector & args) override {
        return create_instantiation(args);
    }

    /// Creates template specialization from vector of arguments
    template_record_specialization_type *
    create_specialization_impl(const template_argument_desc_vector & args) override {
        return create_specialization(args);
    }

    /// Creates template dependent instantiation from vector of arguments
    template_record_dependent_instantiation_type *
    create_dependent_instantiation_impl(const template_argument_desc_vector & args) override {
        return create_dependent_instantiation(args);
    }
};


/// Template record partial specialization
class template_record_partial_specialization: public record,
                                              public templated_entity,
                                              public template_substitution {
public:
    /// Constructs template record partial specialization
    explicit template_record_partial_specialization(context * ctx, template_record * templ):
        record{ctx, templ->kind()},
        templated_entity{ctx},
        template_substitution{templ},
        context{ctx},
        context_entity{ctx} {}

    /// Dumps template record partial specialization
    void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const override {
        print_indent_spaces(str, indent);
        str << "template_partial_specialization";
        dump_template_params(str, opts);
        str << ' ';
        dump_header_and_contents(str, opts, indent);
    }

    /// Prints entity short description to output stream
    void print_desc(std::ostream & str) const override {
        template_substitution::print_desc(str);
    }
};


//////////////////////////////////////////////////////////////////////
// Implementation


template <std::convertible_to<template_argument_desc> ... Args>
template_record_substitution::template_record_substitution(context * ctx,
                                                           template_record * templ,
                                                           Args && ... args):
template_substitution{templ, std::forward<Args>(args)...},
context_entity{ctx} {}


template <template_argument_desc_range Args>
template_record_substitution::template_record_substitution(context * ctx,
                                                           template_record * templ,
                                                           Args && args):
template_substitution{templ, std::forward<Args>(args)},
context_entity{ctx} {}


template <std::convertible_to<template_argument_desc> ... Args>
template_record_instantiation_type::template_record_instantiation_type(context * ctx,
                                                                       template_record * templ,
                                                                       Args && ... args):
record_type{ctx, templ->kind()},
template_instantiation{templ, std::forward<Args>(args)...},
template_record_substitution{ctx, templ, std::forward<Args>(args)...},
template_substitution{templ, std::forward<Args>(args)...},
context_type{ctx},
context{ctx},
context_entity{ctx} {}


template <template_argument_desc_range Args>
template_record_instantiation_type::template_record_instantiation_type(context * ctx,
                                                                       template_record * templ,
                                                                       Args && args):
record_type{ctx, templ->kind()},
template_instantiation{templ, std::forward<Args>(args)},
template_record_substitution{ctx, templ, std::forward<Args>(args)},
template_substitution{templ, std::forward<Args>(args)},
context_type{ctx},
context{ctx},
context_entity{ctx} {}


template <std::convertible_to<template_argument_desc> ... Args>
template_record_specialization_type::template_record_specialization_type(context * ctx,
                                                                         template_record * templ,
                                                                         Args && ... args):
template_record_instantiation_type{ctx, templ, std::forward<Args>(args)...},
template_specialization{ctx, templ, std::forward<Args>(args)...},
template_instantiation{templ, std::forward<Args>(args)...},
template_record_substitution{ctx, templ, std::forward<Args>(args)...},
context_type{ctx},
context{ctx},
context_entity{ctx} {}


template <template_argument_desc_range Args>
template_record_specialization_type::template_record_specialization_type(context * ctx,
                                                                         template_record * templ,
                                                                         Args && args):
template_record_instantiation_type{ctx, templ, std::forward<Args>(args)},
template_specialization{ctx, templ, std::forward<Args>(args)},
template_instantiation{templ, std::forward<Args>(args)},
template_substitution{templ, std::forward<Args>(args)},
context_type{ctx},
context{ctx},
context_entity{ctx} {}


template <std::convertible_to<template_argument_desc> ... Args>
template_record_dependent_instantiation_type::template_record_dependent_instantiation_type(
        context * ctx,
        template_record * templ,
        Args && ... args):
template_dependent_instantiation{templ, std::forward<Args>(args)...},
context_type{ctx},
context_entity{ctx} {}


template <template_argument_desc_range Args>
template_record_dependent_instantiation_type::template_record_dependent_instantiation_type(
        context * ctx,
        template_record * templ,
        Args && args):
context_type{ctx},
template_record_substitution{ctx, templ, std::forward<Args>(args)},
template_dependent_instantiation{templ, std::forward<Args>(args)},
template_substitution{templ, std::forward<Args>(args)},
context_entity{ctx} {}


template_record_partial_specialization * template_record::create_partial_specialization() {
    return ctx()->create_entity<template_record_partial_specialization>(this);
}


template <typename ... Params>
template_record * context::get_or_create_template_record(const std::string & name,
                                                         record_kind knd,
                                                         bool is_variadic,
                                                         Params && ... params) {
    // looking for existing named entity
    if (auto ent = find_named_entity(name)) {
        // checking that entity is a template
        auto templ = dynamic_cast<template_record*>(ent);
        assert(templ && "named entity is not a template record");

        // checking equality of template parameters of existing template
        assert(templ->is_variadic() == is_variadic && "parameters pack flag does not match");
        return templ;
    }

    // creating new template record
    auto rec = create_template_record(name, knd, std::forward<Params>(params)...);
    rec->set_is_variadic(is_variadic);
    return rec;
}


/// Dumps template record dependent instantiation to output stream
inline void template_record_dependent_instantiation_type::dump(std::ostream & str,
                                                               const dump_options & opts,
                                                               unsigned int indent) const {
    auto rec = templ<template_record>();

    print_indent_spaces(str, indent);
    str << "template_dependent_instantiation " << record_kind_name(rec->kind()) << ' ';
    print_desc(str);
    str << ";\n";
}


inline auto context::template_records() const { return entities<template_record>(); }

inline auto context::template_records() { return entities<template_record>(); }


}
