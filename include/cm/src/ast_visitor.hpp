// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file ast_visitor.hpp
/// Contains definition of the ast_visitor class.

#pragma once


namespace cm::src {

class ast_node;


/// AST node visitor. Contains single method for traversing through AST nodes
class ast_visitor {
public:
    /// Traverses through AST node. Returns true if traversing should continue after call.
    virtual bool traverse(ast_node * node) = 0;
};


/// Const AST node visitor. Contains single method traversing const AST nodes
class const_ast_visitor {
public:
    /// Traverses through AST node. Returns true if traversing should continue after call.
    virtual bool traverse(const ast_node * node) = 0;
};


}
