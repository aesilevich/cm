// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file mem_ptr_type.hpp
/// Contains definitions of classes for pointer to member types.

#pragma once

#include "array_type.hpp"
#include "function_type.hpp"
#include "type.hpp"
#include "record_type.hpp"


namespace cm {


class record_type;


/// Structure that completely describes pointer to member
class mem_ptr_type_id {
public:
    /// Constructs member pointer type description with specified object
    /// type and member type
    mem_ptr_type_id(const record_type * ot, const const_qual_type mt):
        obj_type_{ot}, mem_type_{mt} {}

    /// Returns object type
    auto obj_type() const { return obj_type_; }

    /// Returns member type
    auto mem_type() const { return mem_type_; }

    /// Calculates hash of type description
    size_t hash() const {
        auto res = std::hash<const record_type*>()(obj_type());
        res += std::hash<const_qual_type>()(mem_type());
        return res;
    }

    /// Compares this type description with other
    bool operator==(const mem_ptr_type_id & other) const {
        return obj_type() == other.obj_type() && mem_type() == other.mem_type();
    }

private:
    const record_type * obj_type_;        ///< Pointer to object type
    const_qual_type mem_type_;            ///< Member type
};


/// Pointer to member type
class mem_ptr_type: public type_t {
public:
    /// Constructs pointer to member type with specified this object type
    /// and member type
    mem_ptr_type(record_type * ot, const qual_type & mt):
    obj_type_{ot}, mem_type_{mt} {
        obj_type_->add_use(this);
        mem_type_->add_use(this);
    }

    /// Destoys object and removes type use for object type and member type
    virtual ~mem_ptr_type() {
        mem_type_->remove_use(this);
        obj_type_->remove_use(this);
    }

    /// Returns object type
    auto obj_type() { return obj_type_; }

    /// Returns object type
    const record_type * obj_type() const { return obj_type_; }

    /// Returns member type
    auto mem_type() { return mem_type_; }

    /// Returns member type
    const_qual_type mem_type() const { return mem_type_; }

    /// Returns composite type description
    auto type_id() const {
        return mem_ptr_type_id{obj_type(), mem_type()};
    }

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const override {
        obj_type_->print_desc(str);
        str << "::";
        mem_type().print_desc(str);
    }

private:
    record_type * obj_type_;        ///< Pointer to object type
    qual_type mem_type_;            ///< Pointer to member type
};


}


namespace std {
    /// std::hash specialization for mem_ptr_type_id
    template <> struct hash<cm::mem_ptr_type_id> {
        using argument_type = const cm::mem_ptr_type_id &;
        using result_type = size_t;

        result_type operator()(argument_type arg) const {
            return arg.hash();
        }
    };
}
