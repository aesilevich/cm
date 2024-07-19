// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file record_decl_type_spec.hpp
/// Contains definition of the record_decl_type_spec type.

#pragma once

#include "ast_node_impl.hpp"
#include "entity_attr.hpp"
#include "type_spec.hpp"


namespace cm::src {


using record_decl_type_spec_base = ast_node_impl <
    "record_decl_type_spec",
    context_type_spec,
    ast_node,
    entity_attr<record_type>::type
>;


/// Represents type specifier containing record declaration (i. e. typedef struct {...} my_type; )
class record_decl_type_spec: public record_decl_type_spec_base {
public:
    /// Constructs record declaration type specified
    explicit record_decl_type_spec(ast_node * parent):
        record_decl_type_spec_base{parent} {}

    /// Returns pointer to record declaration
    record_decl * decl() { return decl_.get(); }

    /// Returns const pointer to record declaration
    const record_decl * decl() const { return decl_.get(); }

    /// Sets record declaration
    void set_decl(std::unique_ptr<record_decl> && d) {
        decl_ = std::move(d);
    }

    /// Traverses all children nodes. Returns true if traversing should continue.
    bool traverse_children(ast_visitor & v) override {
        // traversing base class children
        if (!record_decl_type_spec_base::traverse_children(v)) {
            return false;
        }

        // traversing record decl
        return v.traverse(decl_.get());
    }

    /// Traverses all children nodes. Returns true if traversing should continue.
    bool traverse_children(const_ast_visitor & v) const override {
        // traversing base class children
        if (!record_decl_type_spec_base::traverse_children(v)) {
            return false;
        }

        // traversing record decl
        return v.traverse(decl_.get());
    }

private:
    std::unique_ptr<record_decl> decl_;     ///< Record declaration
};


}
