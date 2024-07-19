// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_spec_and_arguments_attr.hpp
/// Contains definition of the template_spec_and_arguments_attr class.

#pragma once

#include "template_arguments_attr.hpp"
#include "template_attr.hpp"


namespace cm::src {


/// Template and arguments AST node attribute
template <typename Derived>
class template_and_arguments_attr: public template_attr<Derived>,
                                   public template_arguments_attr<Derived> {
public:
    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(ast_visitor & v) {
        return traverse_impl(*this, v);
    }

    /// Traverses all children nodes. Returns true if visiting should continue after the call.
    bool traverse_children(const_ast_visitor & v) const {
        return traverse_impl(*this, v);
    }

private:
    /// Traverse impementation
    template <typename Self, typename Visitor>
    static bool traverse_impl(Self & self, Visitor & v) {
        if (!self.template_attr<Derived>::traverse_children(v)) {
            return false;
        }

        return self.template_arguments_attr<Derived>::traverse_children(v);
    }
};


}
