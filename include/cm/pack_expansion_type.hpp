// Copyright (c) 2024, Alexandr Esilevich
//
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file pack_expansion_type.hpp
/// Contains definition of the pack_expansion_type class.

#pragma once

#include "type.hpp"
#include "qual_type.hpp"


namespace cm {


/// Represents pack expansion type in code model, such as Args ...
class pack_expansion_type: public type_t {
public:
    /// Constructor, makes pack expansion type with specified pattern type.
    /// Adds use to the pattern type
    pack_expansion_type(const qual_type & p):
    pattern_{p} {
        assert(pattern_ && "invalid pack expansion pattern type");
        pattern_.type()->add_use(this);
    }

    /// Destructor, removes use of the pattern type
    ~pack_expansion_type() {
        pattern_.type()->remove_use(this);
    }

    /// Returns pattern type of the expansion
    qual_type pattern() {
        return pattern_;
    }

    /// Returns const pattern type of the expansion
    const_qual_type pattern() const {
        return pattern_;
    }

    /// Returns description for composite type
    auto type_id() const {
        return pattern();
    }

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const override {
        pattern().print_desc(str);
        str << " ...";
    }

private:
    qual_type pattern_;         ///< Pattern type of the expansion
};


}
