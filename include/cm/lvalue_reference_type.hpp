// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file lvalue_reference_type.hpp
/// Contains definition of the lvalue_reference_type class.

#pragma once

#include "ptr_or_ref_type.hpp"


namespace cm {


/// Represents lvalue reference type in code model
class lvalue_reference_type: public ptr_or_ref_type {
public:
    /// Constructor, makes pointer type with specified pointee type
    lvalue_reference_type(const qual_type & p):
    ptr_or_ref_type(p) {
        assert(p.type() && "reference base type is invalid");
        assert(!p.type()->is<lvalue_reference_type>() && "reference to reference is not valid");
    }

    /// Returns true if type is reference. Always returns true
    bool is_ref() const override { return true; }

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const override {
        base()->print_desc(str);
        str << " &";
    }
};



}
