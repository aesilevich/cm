// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

#pragma once

/// \file named_entity.hpp
/// Contains definition of the named_entity class.

#include "entity.hpp"


namespace cm {


/// An context_entity that has name
class named_entity: virtual public entity {
public:
    /// Returns name of context_entity
    virtual const std::string & name() const = 0;

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const override {
        str << name();
    }

    /// Returns true if entity is builtin entity (defined by language or compiler)
    bool is_builtin() const override;
};


}
