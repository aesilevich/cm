// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_argument.hpp
/// Contains definition of classes for representing template arguments.

#pragma once

#include "entity.hpp"
#include "template_argument_desc.hpp"
#include "value.hpp"


namespace cm {

class template_substitution;


/// Template argument
class template_argument: virtual public entity {
public:
    /// Constructs template argument
    explicit template_argument(template_substitution * subst):
        subst_{subst} {}

    /// Returns template argument description
    virtual template_argument_desc desc() = 0;

    /// Returns const template argument description
    virtual const_template_argument_desc desc() const = 0;

    /// Returns pointer to template substitution
    template_substitution * substitution() { return subst_; }

    /// Returns const pointer to template substitution
    const template_substitution * substitution() const { return subst_; }

private:
    template_substitution * subst_;     ///< Pointer to template substitution
};


/// Type template argument
class type_template_argument: public template_argument, public qual_type_use_impl<> {
public:
    /// Constructs type template argument
    explicit type_template_argument(template_substitution * subst, const qual_type & t):
        template_argument{subst}, qual_type_use_impl<>{t} {}

    /// Returns template argument description
    template_argument_desc desc() override {
        return template_argument_desc{type()};
    }

    /// Returns const template argument description
    const_template_argument_desc desc() const override {
        return const_template_argument_desc{type()};
    }

    /// Prints template argument description
    void print_desc(std::ostream & str) const override {
        type().print_desc(str);
    }
};


/// Value template argument
class value_template_argument: public template_argument {
public:
    /// Constructs value template argument
    explicit value_template_argument(template_substitution * subst, const value & val):
        template_argument{subst}, val_{val} {}

    /// Returns parameter value
    const value & val() const { return val_; }

    /// Sets parameter value
    void set_value(const value & v) { val_ = v; }

    /// Returns template argument description
    template_argument_desc desc() override {
        return template_argument_desc{val()};
    }

    /// Returns const template argument description
    const_template_argument_desc desc() const override {
        return const_template_argument_desc{val()};
    }

    /// Prints template argument description
    void print_desc(std::ostream & str) const override {
        str << val().str();
    }

private:
    value val_;
};


}
