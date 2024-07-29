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


/// Context entity access level
enum class access_level {
    public_,
    protected_,
    private_
};


/// Represents abstract entity in code model located inside some context
class context_entity: virtual public entity {
public:
    /// Constructs entity with specified pointer to context and access level
    explicit context_entity(context * ctx, access_level acc_lev):
        entity_ctx_{ctx}, acc_lev_{acc_lev} {}

    /// Constructs entity with specified pointer to context. Sets access level to
    /// default access level for context
    explicit context_entity(context * ctx);

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

    /// Returns entity access level
    access_level access_lev() const { return acc_lev_; }

    /// Sets entity access level
    void set_access_lev(access_level l) { acc_lev_ = l; }

private:
    context * entity_ctx_;      ///< Pointer to parent context
    source_location loc_;       ///< Location in source code
    access_level acc_lev_;      ///< Access level
};


}
