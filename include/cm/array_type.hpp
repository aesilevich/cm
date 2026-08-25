// Copyright (c) 2024, Alexandr Esilevich
//
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file array_type.hpp
/// Contains definitions of the array type classes.

#pragma once

#include "type.hpp"
#include "value.hpp"
#include <cassert>
#include <cstdint>


namespace cm {


/// Base class for all array types in code model
class array_type: public type_t {
public:
    /// Constructor, makes array type with specified element type.
    /// Adds use to the element type
    explicit array_type(type_t * b):
    base_{b} {
        assert(base_ && "array element type should not be null");
        base_->add_use(this);
    }

    /// Destructor, removes use of the element type
    ~array_type() override {
        base_->remove_use(this);
    }

    /// Returns type of array element
    type_t * base() {
        return base_;
    }

    /// Returns type of array element
    const type_t * base() const {
        return base_;
    }

private:
    type_t * base_;         ///< Type of array element
};


/// Fixed size array type ID
struct fixed_array_type_id {
    const type_t * type;
    uint64_t size;

    bool operator==(const fixed_array_type_id & id2) const {
        return type == id2.type && size == id2.size;
    }
};


/// Represents array type with constant size in code model
class fixed_array_type: public array_type {
public:
    /// Constructor, makes array type with specified element type and size
    fixed_array_type(type_t * b, uint64_t sz):
        array_type{b}, size_{sz} {}

    /// Returns array size
    auto size() const {
        return size_;
    }

    /// Creates array type description
    fixed_array_type_id type_id() const {
        return {base(), size_};
    }

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const override {
        base()->print_desc(str);
        str << "[" << size_ << "]";
    }

private:
    uint64_t size_;         ///< Array size
};


/// Incomplete array type ID
using incomplete_array_type_id = const type_t *;


/// Represents array type with no size in code model
class incomplete_array_type: public array_type {
public:
    /// Constructor, makes array type with specified element type
    explicit incomplete_array_type(type_t * b):
        array_type{b} {}

    /// Creates array type description
    incomplete_array_type_id type_id() const {
        return base();
    }

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const override {
        base()->print_desc(str);
        str << "[]";
    }
};


/// Array type with value size ID
struct value_array_type_id {
    const type_t * type;
    cm::value size;

    bool operator==(const value_array_type_id & id2) const {
        return type == id2.type && size == id2.size;
    }
};


/// Base class for array types with size specified by a value
class value_sized_array_type: public array_type {
public:
    /// Constructor, makes array type with specified element type and size value
    value_sized_array_type(type_t * b, const cm::value & sz):
        array_type{b}, size_{sz} {}

    /// Returns value of the array size
    const cm::value & size() const {
        return size_;
    }

    /// Creates array type description
    value_array_type_id type_id() const {
        return {base(), size_};
    }

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const override {
        base()->print_desc(str);
        str << "[" << size_.str() << "]";
    }

private:
    cm::value size_;        ///< Value of the array size
};


/// Represents variable length array type in code model
class dynamic_array_type: public value_sized_array_type {
public:
    /// Constructor, makes array type with specified element type and size value
    dynamic_array_type(type_t * b, const cm::value & sz):
        value_sized_array_type{b, sz} {}
};


/// Represents array type with size depending on template parameters in code model
class dependent_sized_array_type: public value_sized_array_type {
public:
    /// Constructor, makes array type with specified element type and size value
    dependent_sized_array_type(type_t * b, const cm::value & sz):
        value_sized_array_type{b, sz} {}
};


/// Hash function for the fixed_array_type_id
struct fixed_array_type_id_hash {
    using argument_type = const fixed_array_type_id &;
    using result_type = size_t;
    result_type operator()(argument_type arg) const {
        auto res = static_cast<size_t>(reinterpret_cast<intptr_t>(arg.type));
        res |= arg.size;
        return res;
    }
};


/// Hash function for the value_array_type_id
struct value_array_type_id_hash {
    using argument_type = const value_array_type_id &;
    using result_type = size_t;
    result_type operator()(argument_type arg) const {
        auto res = static_cast<size_t>(reinterpret_cast<intptr_t>(arg.type));
        res ^= arg.size.hash();
        return res;
    }
};


}


namespace std {
    /// std::hash specialization for fixed_array_type_id
    template <> struct hash<cm::fixed_array_type_id>:
        public cm::fixed_array_type_id_hash {};

    /// std::hash specialization for value_array_type_id
    template <> struct hash<cm::value_array_type_id>:
        public cm::value_array_type_id_hash {};
}
