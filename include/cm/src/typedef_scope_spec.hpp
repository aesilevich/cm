// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file typedef_scope_spec.hpp
/// Contains definition of the typedef_scope_spec class.

#pragma once

#include "entity_attr.hpp"
#include "scope_spec.hpp"


namespace cm::src {


/// Typedef scope specifier
using typedef_scope_spec = ast_node_impl <
    "typedef_scope_spec",
    scope_spec,
    ast_node,
    entity_attr<typedef_type>::type
>;


}
