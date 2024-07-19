// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file array_type.hpp
/// Contains definition of the array_type class.

#pragma once

#include "array_or_vector_type.hpp"


namespace cm {


/// Array type ID
using array_type_id = array_or_vector_type_id;


/// Represents array type in code model
class array_type: public array_or_vector_type {
public:
    /// Constructor, makes array type with specified element
    /// type and size
    array_type(type_t * b, uint64_t sz):
        array_or_vector_type{b, sz} {}

    /// Destructor, removes use of element type
    ~array_type() override = default;

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const override {
        base()->print_desc(str);
        str << "[" << size() << "]";
    }
};


}
