// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file functional_decl.hpp
/// Contains definition of the functional_decl class.

#pragma once

#include "ast_node_impl.hpp"
#include "decl_context.hpp"
#include "decl_context_attr.hpp"
#include "entity_attr.hpp"
#include "identifier.hpp"
#include "named_decl.hpp"
#include "opaque_ast_node.hpp"
#include "scope_attr.hpp"
#include "type_spec.hpp"
#include "value_decl.hpp"


namespace cm::src {

class functional_decl;


/// Function parameter declaration
using function_parameter_decl = ast_node_impl <
    "function_parameter_decl",
    value_decl,
    functional_decl,
    entity_attr<function_parameter>::type
>;


/// Represents functional-like declaration that has parameters and return type
class functional_decl: public named_decl {
public:
    /// Returns reference to list of function parameters
    auto & params_list() { return params_; }

    /// Returns true if function has return type
    bool has_ret_type() const { return ret_type_ ? true : false; }

    /// Returns pointer to return type specifier
    type_spec * ret_type() { return ret_type_.get(); }

    /// Returns const pointer to return type specifier
    const type_spec * ret_type() const { return ret_type_.get(); }

    /// Sets return type specifier
    void set_ret_type(std::unique_ptr<type_spec> && r_t) { ret_type_ = std::move(r_t); }

    /// Returns range of funciton parameters
    auto params() {
        auto fn = [](auto && uptr) { return uptr.get(); };
        return params_ | std::ranges::views::transform(fn);
    }

    /// Returns const range of function parameters
    auto params() const {
        auto fn = [](auto && uptr) {
            return const_cast<const function_parameter_decl*>(uptr.get());
        };
        return params_ | std::ranges::views::transform(fn);
    }

    /// Adds function parameter to the end of parameters list
    void add_param(std::unique_ptr<function_parameter_decl> && p) {
        params_.push_back(std::move(p));
    }

    /// Returns pointer to function body
    opaque_ast_node * body() { return body_.get(); }

    /// Returns const pointer to function body
    const opaque_ast_node * body() const { return body_.get(); }

    /// Returns true if function has body
    bool has_body() const { return body() != nullptr; }

    /// Sets function body
    void set_body(std::unique_ptr<opaque_ast_node> && b) { body_ = std::move(b); }

    /// Creates function body
    void create_body() { set_body(std::make_unique<opaque_ast_node>(this)); }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(ast_visitor & v) override;

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(const_ast_visitor & v) const override;


private:
    std::unique_ptr<type_spec> ret_type_;           ///< Function return type

    /// List of function parameters
    std::list<std::unique_ptr<function_parameter_decl>> params_;

    /// Function body
    std::unique_ptr<opaque_ast_node> body_;
};


}
