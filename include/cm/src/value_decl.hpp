// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file value_decl.hpp
/// Contains definition of the value_decl class.

#pragma once

#include "named_decl.hpp"
#include "type_attr.hpp"


namespace cm::src {


/// Value declaration (has name and type)
using value_decl = intermediate_ast_node_impl <
    named_decl,
    type_attr
>;


}
