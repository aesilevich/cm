// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file enum_type.hpp
/// Contains definition of the enum_type class.

#include "builtin_type.hpp"
#include "named_type.hpp"
#include <vector>


namespace cm {


/// Represents enum type in code model
class enum_type: public named_type {
public:
    /// Enum item
    struct item {
        std::string name;
        int value;
    };

    /// Constructs enum type with specified context, name and base type for enum
    enum_type(context * ctx, const std::string & nm, builtin_type * b):
        named_type(ctx, nm), context_type(ctx), context_entity(ctx), base_{b} {}

    /// Returns base type for enum
    auto base() { return base_; }

    /// Returns base type for enum
    const builtin_type * base() const { return base_; }

    /// Returns vector of enum items
    auto & items() { return items_; }

    /// Returns vector of enum items
    auto & items() const { return items_; }

    /// Prints type description to output stream
    void print_desc(std::ostream & str) const override {
        str << "enum " << name();
    }

private:
    builtin_type * base_;           ///< Base type for enum
    std::vector<item> items_;       ///< List of enum items
};


}
