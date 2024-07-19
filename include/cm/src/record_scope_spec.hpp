// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file record_scope_spec.hpp
/// Contains definition of the record_scope_spec class.

#pragma once

#include "ast_node_impl.hpp"
#include "entity_attr.hpp"
#include "scope_spec.hpp"


namespace cm::src {


/// Represents record scope specifier in source code
using record_scope_spec = ast_node_impl <
    "record_scope_spec",
    scope_spec,
    ast_node,
    entity_attr<named_record_type>::type,
    name_attr
>;


}
