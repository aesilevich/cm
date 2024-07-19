// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file named_decl.hpp
/// Contains definition of the named_decl class.

#pragma once

#include "ast_node_impl.hpp"
#include "declaration.hpp"
#include "identifier.hpp"
#include "name_attr.hpp"
#include "scope_attr.hpp"


namespace cm::src {


/// Helper class for defining declaration nodes that have name
using named_decl = intermediate_ast_node_impl <
    declaration,
    name_attr,
    scope_attr
>;


}
