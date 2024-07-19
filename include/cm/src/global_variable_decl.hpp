// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file global_variable_decl.hpp
/// Contains definition of the global_variable_decl class

#pragma once

#include "ast_node_impl.hpp"
#include "entity_attr.hpp"
#include "scope_attr.hpp"
#include "variable_decl.hpp"
#include "../cm.hpp"


namespace cm::src {


/// Represents global variable declaration in source code.
using global_variable_decl = ast_node_impl <
    "global_variable_decl",
    variable_decl,
    ast_node,
    entity_attr<variable>::type
>;


}
