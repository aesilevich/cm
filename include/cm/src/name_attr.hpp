// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file name_attr.hpp
/// Contains definition of the name_attr class.

#pragma once

#include "identifier.hpp"


namespace cm::src {


/// AST node name attribute
template <typename Derived>
class name_attr {
public:
    /// True if node has name
    bool has_name() const { return name_ ? true : false; }

    /// Return pointer to name identifier
    identifier * name() { return name_.get(); }

    /// Return const pointer to name identifier
    const identifier * name() const{ return name_.get(); }

    /// Sets name identifier
    void set_name(std::unique_ptr<identifier> && nm) { name_ = std::move(nm); }

    /// Sets name string and source location. Creates identifier AST node if it's
    /// note created yet
    template <typename NameString>
    requires (std::assignable_from<std::string &, NameString>)
    void set_name(NameString && name, const source_file_range & r) {
        if (!has_name()) {
            name_ = std::make_unique<identifier>(static_cast<Derived*>(this));
        }

        name_->set_string(std::forward<NameString>(name));
        name_->set_source_range(r);
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(ast_visitor & v) {
        // traversing name
        if (has_name()) {
            if (!v.traverse(name())) {
                return false;
            }
        }

        return true;
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(const_ast_visitor & v) const {
        // traversing name
        if (has_name()) {
            if (!v.traverse(name())) {
                return false;
            }
        }

        return true;
    }

private:
    std::unique_ptr<identifier> name_;          ///< Name of node
};


}
