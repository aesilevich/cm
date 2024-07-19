// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file ptr_or_ref_type.hpp
/// Contains definition of the ptr_or_ref_type class.

#pragma once

#include "array_type.hpp"
#include "function_type.hpp"
#include "type.hpp"
#include "qual_type.hpp"


namespace cm {


/// Represents pointer or reference type in code model
class ptr_or_ref_type: public type_t {
public:
    /// Constructor, makes pointer or reference type with specified pointee type.
    /// Adds use to base type
    ptr_or_ref_type(const qual_type & p):
    base_{p} {
        assert(base_ && "Invalid pointee type");
        base_.type()->add_use(this);
    }

    /// Destructor, removes use to base type
    ~ptr_or_ref_type() {
        base_.type()->remove_use(this);
    }

    /// Returns type of pointee
    qual_type base() {
        return base_;
    }

    /// Returns type of pointee
    const_qual_type base() const {
        return base_;
    }

    /// Returns description for composite type
    auto type_id() const {
        return base();
    }

    /// Returns true if type is reference
    virtual bool is_ref() const = 0;

private:
    qual_type base_;        ///< Type of pointee
};


}
