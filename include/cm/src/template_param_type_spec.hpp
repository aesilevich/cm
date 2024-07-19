// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_param_type_spec.hpp
/// Contains definition of the template_param_type_spec type.

#pragma once

#include "ast_node_impl.hpp"
#include "entity_attr.hpp"
#include "name_attr.hpp"
#include "type_spec.hpp"


namespace cm::src {


/// Template parameter type specifier
using template_param_type_spec = ast_node_impl <
    "template_param_type_spec",
    type_spec,
    ast_node,
    entity_attr<type_template_parameter>::type,
    name_attr
>;


}
