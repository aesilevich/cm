// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file namespace_scope_spec.hpp
/// Contains definition of the namespace_sepc class.

#pragma once

#include "ast_node_impl.hpp"
#include "entity_attr.hpp"
#include "scope_spec.hpp"
#include "../cm.hpp"


namespace cm::src {


/// Namespace scope specifier
using namespace_scope_spec = ast_node_impl <
    "namespace_scope_spec",
    scope_spec,
    ast_node,
    entity_attr<namespace_>::type,
    name_attr
>;


}
