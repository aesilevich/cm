// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file pointer_type.hpp
/// Contains definition of the pointer_type class.

#pragma once

#include "array_type.hpp"
#include "function_type.hpp"
#include "ptr_or_ref_type.hpp"
#include "lvalue_reference_type.hpp"


namespace cm {


/// Represents pointer type in code model
class pointer_type: public ptr_or_ref_type {
public:
    /// Constructor, makes pointer type with specified pointee type
    pointer_type(const qual_type & p):
    ptr_or_ref_type(p) {
        assert(p.type() && "reference base type is invalid");
        assert(!p.type()->is<lvalue_reference_type>() && "pointer to reference is not valid");
    }

    /// Returns true if type is reference. Always returns false
    bool is_ref() const override { return false; }

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const override {
        base()->print_desc(str);
        str << " *";
    }
};


}
