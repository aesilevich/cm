// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file functional_decl.cpp
/// Contains implementation of the functional_decl class and related classes.

#include "cm/src/functional_decl.hpp"


namespace cm::src {


bool functional_decl::traverse_children(ast_visitor & v) {
    if (!named_decl::traverse_children(v)) {
        return false;
    }

    if (has_ret_type()) {
        if (!v.traverse(ret_type())) {
            return false;
        }
    }

    if (!traverse_nodes(params(), v)) {
        return false;
    }

    if (has_body()) {
        if (!v.traverse(body())) {
            return false;
        }
    }

    return true;
}


bool functional_decl::traverse_children(const_ast_visitor & v) const {
    if (!named_decl::traverse_children(v)) {
        return false;
    }

    if (has_ret_type()) {
        if (!v.traverse(ret_type())) {
            return false;
        }
    }

    if (!traverse_nodes(params(), v)) {
        return false;
    }

    if (has_body()) {
        if (!v.traverse(body())) {
            return false;
        }
    }

    return true;
}



}
