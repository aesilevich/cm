// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file record_type_spec.hpp
/// Contains definition of the record_type_spec class.

#pragma once

#include "ast_node_impl.hpp"
#include "entity_attr.hpp"
#include "type_spec.hpp"


namespace cm::src {


/// Represents record type specifier in source code
using record_type_spec = ast_node_impl <
    "record_type_spec",
    context_type_spec,
    ast_node,
    entity_attr<record_type>::type,
    name_attr
>;


}
