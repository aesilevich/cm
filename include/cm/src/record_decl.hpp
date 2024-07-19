// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file recod_decl.hpp
/// Contains definition of the record_decl class.

#pragma once

#include "decl_context.hpp"
#include "decl_context_attr.hpp"
#include "named_decl.hpp"
#include "scope_attr.hpp"


namespace cm::src {


/// Record declaration AST node
using record_decl = ast_node_impl <
    "record",
    named_decl,
    ast_node,
    entity_attr<record>::type,
    decl_context_attr
>;


}
