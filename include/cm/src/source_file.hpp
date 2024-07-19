// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file source_file.hpp
/// Contains definition of the source_file class.

#pragma once

#include "ast_node_impl.hpp"
#include "decl_context_attr.hpp"
#include "entity_attr.hpp"


namespace cm::src {


/// Base class for source file node
using source_file_base = intermediate_ast_node_impl <
    ast_node,
    decl_context_attr
>;


/// Represents single source file containing declarations
class source_file: public source_file_base {
public:
    /// Constructs empty source file with specified pointer to code model source file
    source_file(const cm::source_file * src):
        cm_src_{src} {}

    /// Returns const pointer to code model source file
    const cm::source_file * cm_src() const { return cm_src_; }

    /// Always returns nullptr
    ast_node * parent() override { return nullptr; }

    /// Always returns nullptr
    const ast_node * parent() const override { return nullptr; }

    /// Returns range in source code for entire source file
    source_file_range source_range() const override {
        // TODO
        static source_file_range r;
        return r;
    }

    /// Sets range in source code for AST node
    void set_source_range(const source_file_range & r) override {
        // TODO
    }

    /// Returns AST node class name
    std::string class_name() const override {
        return "source_file";
    }

    /// Returns code model entity associated with this AST node. Always returns nullptr.
    const class entity * entity() const override {
        return nullptr;
    }

    /// Returns code model entity associated with this AST node. Always returns nullptr.
    class entity * entity() override {
        return nullptr;
    }

private:
    const cm::source_file * cm_src_;        ///< Code model source file
};


}
