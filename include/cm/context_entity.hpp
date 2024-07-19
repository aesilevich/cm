// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

#pragma once

/// \file context_entity.hpp
/// Contains definition of the context_entity class.

#include "entity.hpp"
#include "source_location.hpp"


namespace cm {


class context;


/// Represents abstract entity in code model located inside some context
class context_entity: virtual public entity {
public:
    /// Constructs entity with specified pointer to context
    context_entity(context * ctx):
        entity_ctx_{ctx} {}

    /// Default destructor
    virtual ~context_entity() = default;

    /// Returns pointer to context
    auto ctx() { return entity_ctx_; }

    /// Returns const pointer to context
    const context * ctx() const { return entity_ctx_; }

    /// Returns location of declaration in source code
    auto & loc() const { return loc_; }

    /// Sets location of declaration in source code
    void set_loc(const source_location & l) { loc_ = l; }

    /// Dumps location to output stream
    void dump_loc(std::ostream & str, const dump_options & opts) const;

private:
    context * entity_ctx_;      ///< Pointer to parent context
    source_location loc_;       ///< Location in source code
};


}
