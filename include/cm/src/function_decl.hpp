// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file function_decl.hpp
/// Contains definition of the function_decl class.

#pragma once

#include "entity_attr.hpp"
#include "functional_decl.hpp"


namespace cm::src {


/// Function declaration
using function_decl = ast_node_impl <
    "function_decl",
    functional_decl,
    ast_node,
    entity_attr<function>::type
>;


}
