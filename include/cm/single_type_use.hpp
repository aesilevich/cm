// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file single_type_use.hpp
/// Contains definition of the single_type_use class.

#pragma once

#include "type.hpp"


namespace cm {


/// Helper base class for defining classes with single use of a type.
/// Contains pointer to type and automatically adds and removes type use.
class single_type_use: virtual public entity {
public:
    /// Constructs type use for specified type. Adds type use if type is not null.
    explicit single_type_use(const qual_type & t):
    type_{t} {
        if (type_) {
            type_->add_use(this);
        }
    }

    // non copyable / non moveable
    single_type_use(const single_type_use &) = delete;
    single_type_use(single_type_use &&) = delete;
    single_type_use & operator=(const single_type_use &) = delete;
    single_type_use & operator=(single_type_use &&) = delete;

    /// Destroys type use. Removes type use if type is not null
    ~single_type_use() override {
        if (type_) {
            type_->remove_use(this);
        }
    }

    /// Returns type in this use
    const qual_type & type() { return type_; }

    /// Returns const type in this use
    const_qual_type type() const { return type_; }

    /// Sets type in this use. Removes use of the old type if the old type is not null.
    /// Adds use of the new type if new type is not null.
    void set_type(const qual_type & ct) {
        if (type_) {
            type_->remove_use(this);
        }

        type_ = ct;

        if (type_) {
            type_->add_use(this);
        }
    }

private:
    qual_type type_;            ///< The type
};


}
