// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template.hpp
/// Contains definition of the template_ class.

#pragma once

#include "context.hpp"
#include "template_argument_desc.hpp"
#include "template_name.hpp"
#include "template_substitution.hpp"
#include "templated_entity.hpp"
#include <ranges>
#include <list>
#include <vector>
#include <string>


namespace cm {

class template_instantiation;
class template_specialization;
class template_dependent_instantiation;


/// Represents template declared in code model.
class template_: public templated_entity,
                 virtual public named_context_entity,
                 public template_name {
public:
    /// Constructs template with not parameters
    template_(context * ctx, const std::string & nm):
        templated_entity{ctx},
        named_context_entity{ctx, nm} {}

    /// Returns template name
    const std::string & name() const override {
        return named_context_entity::name();
    }

    /// Searches for template substitution. If substitution found then checks
    /// that type of substitution is same as requested.
    ///
    /// \param  args    pack of template arguments
    /// \return         pointer to template substitution or nullptr if substitution is not found
    template <
        typename Substitution = template_substitution,
        std::convertible_to<template_argument_desc> ... Args
    >
    Substitution * find_substitution(const Args & ... args) {
        for (auto && subst : uses<template_substitution>()) {
            if (subst->args_equal(args...)) {
                if constexpr (std::same_as<Substitution, template_substitution>) {
                    return subst;
                } else {
                    auto casted_subst = dynamic_cast<Substitution*>(subst);
                    assert(casted_subst && "invalid substituion type for arguments");
                    return casted_subst;
                }
            }
        }

        return nullptr;
    }

    /// Searches for const template substitution. If substitution found then checks
    /// that type of substitution is same as requested.
    ///
    /// \param  args    pack of template arguments
    /// \return         pointer to template substitution or nullptr if substitution is not found
    template <
        typename Substitution = template_substitution,
        std::convertible_to<const_template_argument_desc> ... Args
    >
    const Substitution * find_substitution(const Args & ... args) const {
        for (auto && subst : uses<template_substitution>()) {
            if (subst->args_equal(args...)) {
                if constexpr (std::same_as<Substitution, template_substitution>) {
                    return subst;
                } else {
                    auto casted_subst = dynamic_cast<Substitution*>(subst);
                    assert(casted_subst && "invalid substituion type for arguments");
                    return casted_subst;
                }
            }
        }

        return nullptr;
    }

    /// Searches for template substitution. If substitution found then checks
    /// that type of substitution is same as requested.
    ///
    /// \param args     range of template arguments
    /// \return         pointer to template substitution or nullptr if substitution is not found
    template <
        typename Substitution = template_substitution,
        const_template_argument_desc_range ArgsRange
    >
    Substitution * find_substitution(ArgsRange && args) {
        for (auto && subst : uses<template_substitution>()) {
            if (subst->args_equal(args)) {
                if constexpr (std::same_as<Substitution, template_substitution>) {
                    return subst;
                } else {
                    auto casted_subst = dynamic_cast<Substitution*>(subst);
                    assert(casted_subst && "invalid substituion type for arguments");
                    return casted_subst;
                }
            }
        }

        return nullptr;
    }

    /// Searches for const template substitution. If substitution found then checks
    /// that type of substitution is same as requested.
    ///
    /// \param args     range of template arguments
    /// \return         pointer to template substitution or nullptr if substitution is not found
    template <
        typename Substitution = template_substitution,
        const_template_argument_desc_range ArgsRange
    >
    const Substitution * find_substitution(ArgsRange && args) const {
        for (auto && subst : uses<template_substitution>()) {
            if (subst->args_equal(args)) {
                if constexpr (std::same_as<Substitution, template_substitution>) {
                    return subst;
                } else {
                    auto casted_subst = dynamic_cast<Substitution*>(subst);
                    assert(casted_subst && "invalid substituion type for arguments");
                    return casted_subst;
                }
            }
        }
    }


protected:
    /// Creates template substitution of specified type from template arguments
    template <
        std::derived_from<template_substitution> Substitution,
        typename Template,
        typename ... Args
    >
    Substitution * create_substitution_impl(Template * this_ptr, Args && ... args) {
        assert(find_substitution(args...) == nullptr &&
               "tempalte substitution with same arguments already exists");

        return ctx()->create_entity<Substitution>(this_ptr, std::forward<Args>(args)...);
    }

    /// Creates template substitution of specified type without checking template arguments.
    template <std::derived_from<template_substitution> Substitution, typename Template>
    Substitution * create_substitution_impl(Template * this_ptr) {
        return ctx()->create_entity<Substitution>(this_ptr);
    }


private:
    /// Creates template instantiation from vector of arguments
    virtual template_instantiation *
    create_instantiation_impl(const template_argument_desc_vector & args) = 0;

    /// Creates template specialization from vector of arguments
    virtual template_specialization *
    create_specialization_impl(const template_argument_desc_vector & args) = 0;

    /// Creates template dependent instantiation from vector of arguments
    virtual template_dependent_instantiation *
    create_dependent_instantiation_impl(const template_argument_desc_vector & args) = 0;
};


inline auto context::templates() const { return entities<templated_entity>(); }

inline auto context::templates() { return entities<templated_entity>(); }


}
