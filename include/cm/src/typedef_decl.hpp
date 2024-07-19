// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file typedef_decl.hpp
/// Contains definition of the typedef_decl type.

#pragma once

#include "ast_node_impl.hpp"
#include "named_decl.hpp"
#include "type_attr.hpp"


namespace cm::src {


/// Record declaration AST node
using 
typedef_decl = ast_node_impl <
    "typedef",
    named_decl,
    ast_node,
    entity_attr<typedef_type>::type,
    type_attr
>;


}
