// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file function_type_spec.hpp
/// Contains definition of the function_type_spec type and related types.

#pragma once

#include "ast_node_impl.hpp"
#include "entity_attr.hpp"
#include "value_decl.hpp"


namespace cm::src {


class function_type_spec;


/// Function type specifier parameter
using function_type_spec_parameter = ast_node_impl <
    "function_type_spec_parameter",
    value_decl,
    function_type_spec,
    entity_attr<type_t>::type
>;


/// Represents function type specifier in source code
class function_type_spec:
    public ast_node_impl<
        "function_type_spec",
        type_spec,
        ast_node,
        entity_attr<function_type>::type
    >
{
public:
    using base_t = ast_node_impl<
        "function_type_spec",
        type_spec,
        ast_node,
        entity_attr<function_type>::type
    >;

    /// Constructs function type specifier
    explicit function_type_spec(ast_node * parent, function_type * type):
    base_t{parent} {
        set_entity(type);
    }

    /// Returns true if function has return type
    bool has_ret_type() const { return ret_type_ ? true : false; }

    /// Returns pointer to return type specifier
    type_spec * ret_type() {
        assert(has_ret_type() && "function spec does not have return type");
        return ret_type_.get();
    }

    /// Returns const pointer to return type specifier
    const type_spec * ret_type() const {
        assert(has_ret_type() && "function spec does not have return type");
        return ret_type_.get();
    }

    /// Sets return type specifier
    void set_ret_type(std::unique_ptr<type_spec> && ret_t) {
        ret_type_ = std::move(ret_t);
    }

    /// Returns range of type specifiers for function parameters
    auto params() {
        auto fn = [](auto && uptr) { return uptr.get(); };
        return params_ | std::ranges::views::transform(fn);
    }

    /// Returns range of const type specifiers for function parameters
    auto params() const {
        auto fn = [](auto && uptr) {
            return const_cast<const function_type_spec_parameter*>(uptr.get());
        };
        return params_ | std::ranges::views::transform(fn);
    }

    /// Adds function parameter into list
    void add_param(std::unique_ptr<function_type_spec_parameter> && par) {
        params_.push_back(std::move(par));
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(ast_visitor & v) override {
        if (has_ret_type()) {
            if (!v.traverse(ret_type_.get())) {
                return false;
            }
        }

        return traverse_nodes(params(), v);
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(const_ast_visitor & v) const override {
        if (has_ret_type()) {
            if (!v.traverse(ret_type_.get())) {
                return false;
            }
        }

        return traverse_nodes(params(), v);
    }

private:
    std::unique_ptr<type_spec> ret_type_;           ///< Return type specifier

    ///< Parameter specifiers
    std::list<std::unique_ptr<function_type_spec_parameter>> params_;
};


}
