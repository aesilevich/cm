// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file entity_attr.hpp
/// Contains definition of the entity_attr template class.

#pragma once

#include "ast_visitor.hpp"


namespace cm::src {


template <typename CMEntity, typename Derived>
class entity_attr_impl {
public:
    /// Constructs attribute with specified reference to code model entity.
    /// Adds entity use
    explicit entity_attr_impl(CMEntity * ent = nullptr):
    ent_{ent} {
        if (ent_) {
            ent_->add_use(static_cast<Derived*>(this));
        }
    }

    /// Destroys attribute. Removes entity use
    ~entity_attr_impl() {
        if (ent_) {
            ent_->remove_use(static_cast<Derived*>(this));
        }
    }

    /// Returns pointer to referenced code model entity
    CMEntity * entity() { return ent_; }

    /// Returns const pointer to referenced code model entity
    const CMEntity * entity() const { return ent_; }

    /// Sets pointer to code model entity. Removes use of current entity
    /// and adds use of the new entity
    void set_entity(CMEntity * e) {
        if (ent_) {
            ent_->remove_use(static_cast<Derived*>(this));
        }

        ent_ = e;

        if (ent_) {
            ent_->add_use(static_cast<Derived*>(this));
        }
    }

    /// Traverses all children nodes. Does nothing, returns true
    bool traverse_children(ast_visitor & v) {
        return true;
    }

    /// Traverses all children nodes. Does nothing, returns true
    bool traverse_children(const_ast_visitor & v) const {
        return true;
    }

private:
    CMEntity * ent_;            ///< Pointer to code model entity
};



/// AST node attribute that contains reference to code model entity
template <typename CMEntity>
struct entity_attr {
    template <typename Derived>
    using type = entity_attr_impl<CMEntity, Derived>;
};


}
