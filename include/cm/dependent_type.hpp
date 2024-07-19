// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file dependent_type.hpp
/// Contains definition of the dependent_type class.

#pragma once

#include "context_type.hpp"


namespace cm {


/// Represents C++ dependent name type (i.e. typename T::nested)
class dependent_type: virtual public context_type {
public:
    /// Constructs dependent type
    explicit dependent_type(context * ctx):
        context_type{ctx},
        context_entity{ctx} {}

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const override {
        str << "dependent_type";
    }
};


}
