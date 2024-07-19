// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file vector_type.hpp
/// Contains definition of the vector_type class.

#pragma once

#include "array_or_vector_type.hpp"
#include "builtin_type.hpp"


namespace cm {


/// Vector type ID
using vector_type_id = array_or_vector_type_id;


/// Represents vector (SSE/AVX) type in code model
class vector_type: public array_or_vector_type {
public:
    /// Constructor, makes vector type with specified element
    /// type and size
    vector_type(type_t * b, uint64_t sz):
    array_or_vector_type{b, sz} {
        assert(b->is<builtin_type>() && "element of vector type must be builtin type");
        assert(sz != SIZE_MAX && "vector type must have size");
    }

    /// Destructor, removes use of element type
    ~vector_type() override = default;

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const override {
        base()->print_desc(str);
        str << "<" << size() << ">";
    }
};


}
