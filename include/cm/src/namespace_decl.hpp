// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file namespace_decl.hpp
/// Contains definition of the namespace_decl class.

#pragma once

#include "ast_node_impl.hpp"
#include "decl_context_attr.hpp"
#include "named_decl.hpp"
#include "entity_attr.hpp"


namespace cm::src {


/// Represents namespace declaration in source code
using namespace_decl = ast_node_impl <
    "namespace_decl",
    named_decl,
    ast_node,
    entity_attr<namespace_>::type,
    decl_context_attr
>;


}
