// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file ast_node_impl_base.hpp
/// Contains definition of the ast_node_impl_base class.

#pragma once

#include "ast_node.hpp"


namespace cm::src {


/// Wrapper for string for AST node class name
template <size_t N>
struct ast_node_class_name {
    /// Constructs class name instance
    constexpr ast_node_class_name(const char (&str)[N]) {
        for (size_t i = 0; i < N; ++i) {
            value[i] = str[i];
        }
    }

    // /// Compares this class name with another
    // template <size_t N2>
    // constexpr bool operator==(const ast_node_class_name<N2> & other) const {
    //     if constexpr (N != N2) {
    //         return false;
    //     } else {
    //         for (size_t i = 0; i < N; ++i) {
    //             if (value[i] != other.value[i]) {
    //                 return false;
    //             }
    //         }

    //         return true;
    //     }
    // }

    // /// Compares this class name with characters array
    // template <size_t N2>
    // constexpr bool operator==(const char (&str)[N2]) const {
    //     return *this == ast_node_class_name{str};
    // }

    char value[N];
};




/// Base helper class for implementing AST nodes without attributes
template <
    ast_node_class_name Name,
    std::derived_from<ast_node> Base = ast_node,
    typename Parent = ast_node
>
class ast_node_impl_base: public Base {
public:
    /// Constructs AST node
    explicit ast_node_impl_base(Parent * parent):
        parent_{parent} {}

    /// Returns AST node class name
    std::string class_name() const override {
        return Name.value;
    }

    /// Returns pointer to parent node
    Parent * parent() override { return parent_; }

    /// Returns const pointer to parent node
    const Parent * parent() const override { return parent_; }

    /// Returns range in source code for AST node
    source_file_range source_range() const override { return range_; }

    /// Sets range in source code for AST node
    void set_source_range(const source_file_range & r) override { range_ = r; }

private:
    Parent * parent_;                   ///< Pointer to parent node
    source_file_range range_;           ///< Node range in source code
};


}
