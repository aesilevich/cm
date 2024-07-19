// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file decltype_type.hpp
/// Contains definition of the decltype_type class.

#pragma once

#include "context_type.hpp"


namespace cm {


/// Represents type declared with the decltype(...) C++ keyword
class decltype_type: virtual public context_type {
public:
    /// Constructs dependent type
    explicit decltype_type(context * ctx):
        context_type{ctx},
        context_entity{ctx} {}

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const override {
        str << "decltype()";
    }
};


}
