// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file method_decl.hpp
/// Contains definition of the method_decl class.

#pragma once

#include "ast_node_impl.hpp"
#include "functional_decl.hpp"
#include "scope_attr.hpp"


namespace cm::src {


/// Represents instance method declaration in source code.
using method_decl = ast_node_impl <
    "method_decl",
    functional_decl,
    ast_node,
    entity_attr<method>::type
>;


/// Represents static method declaration in source code.
using static_method_decl = ast_node_impl <
    "static_method_decl",
    functional_decl,
    ast_node,
    entity_attr<function>::type
>;


}
