// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_argument_desc.hpp
/// Contains definitions of types for describing tempalte arguments

#pragma once

#include "type.hpp"
#include "value.hpp"
#include <variant>


namespace cm {


/// Description of template instantiation argument
template <bool Const>
class template_argument_desc_t {
    template <bool Const2>
    friend class template_argument_desc_t;

public:
    /// Type of qualified type used in this parameter description
    using par_qual_type_t = std::conditional_t<Const, const_qual_type, qual_type>;

    /// Constructs type template argument description
    template_argument_desc_t(const qual_type & t):
        val_{par_qual_type_t{t}} {}

    /// Constructs type template argument description
    template_argument_desc_t(const const_qual_type & t) requires (Const):
        val_{t} {}

    /// Constructs type template argument description
    template_argument_desc_t(type_t * t):
        val_{par_qual_type_t{t}} {}

    /// Constructs type template argument description
    template_argument_desc_t(const type_t * t) requires (Const):
        val_{par_qual_type_t{t}} {}

    /// Constructs value template argument description
    template_argument_desc_t(const value & val):
        val_{val} {}

    /// Default copy constructor
    template_argument_desc_t(const template_argument_desc_t &) = default;

    /// Default move constructor
    template_argument_desc_t(template_argument_desc_t &&) = default;

    /// Constructs non-const parameter description from const parameter description
    template_argument_desc_t(const template_argument_desc_t<false> & desc)
    requires (Const):
        val_{convert_const_val(desc.val_)} {}

    /// Returns true if parameter is a type parameter
    bool is_type() const {
        return std::holds_alternative<par_qual_type_t>(val_);
    }

    /// Returns true if parameter is a value parameter
    bool is_value() const {
        return std::holds_alternative<cm::value>(val_);
    }

    /// Returns type stored in this parameter.
    par_qual_type_t type() const {
        assert(is_type() && "template parameter is not a type template parameter");
        return std::get<par_qual_type_t>(val_);
    }

    /// Returns value stored in this parameter.
    cm::value value() const {
        assert(is_value() && "template parameter is not a type template parameter");
        return std::get<cm::value>(val_);
    }

    /// Calculates hash of parameter
    size_t hash() const {
        if (is_type()) {
            return std::hash<par_qual_type_t>{}(type());
        } else {
            return value().hash();
        }
    }

    /// Compares this parameter description with another
    template <bool Const2>
    bool operator==(const template_argument_desc_t<Const2> & other) const {
        if (is_value()) {
            if (other.is_value()) {
                return value() == other.value();
            } else {
                return false;
            }
        } else {
            if (!other.is_value()) {
                return const_qual_type{type()} == const_qual_type{other.type()};
            } else {
                return false;
            }
        }
    }

    /// Compares this parameter description with type
    template <typename T>
    bool operator==(const qual_type_t<T> & qt) const {
        if (!is_type()) {
            return false;
        }

        return const_qual_type{type()} == const_qual_type{qt};
    }

    /// Prints template instantiation description to the output stream
    void print_desc(std::ostream & str) const {
        if (is_type()) {
            type().print_desc(str);
        } else {
            str << value().str();
        }
    }

private:
    std::variant<par_qual_type_t, cm::value>
    convert_const_val(const std::variant<qual_type, cm::value> & v) {
        if (auto ptype = std::get_if<qual_type>(&v)) {
            return *ptype;
        } else if (auto pval = std::get_if<cm::value>(&v)) {
            return *pval;
        } else {
            assert(false && "unknown variant value");
        }
    }

    std::variant<par_qual_type_t, cm::value> val_;
};


/// Non-constant template parameter description
using template_argument_desc = template_argument_desc_t<false>;

/// Constant template parameter description
using const_template_argument_desc = template_argument_desc_t<true>;


/// Range of template arguments descriptions
template <typename Range>
concept template_argument_desc_range =
    std::ranges::range<Range> && 
    std::convertible_to<std::ranges::range_value_t<Range>, template_argument_desc>;


/// Range of const template arguments descriptions
template <typename Range>
concept const_template_argument_desc_range =
    std::ranges::range<Range> && 
    std::convertible_to<std::ranges::range_value_t<Range>, const_template_argument_desc>;


/// Vector of template argument descriptions
using template_argument_desc_vector = std::vector<template_argument_desc>;



}
