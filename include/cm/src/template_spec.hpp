// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_spec.hpp
/// Contains definition of the template_spec class.

#pragma once

#include "ast_node_impl.hpp"
#include "entity_attr.hpp"
#include "name_attr.hpp"
#include "scope_attr.hpp"


namespace cm::src {


/// Template specifier
using template_spec = ast_node_impl <
    "template_spec",
    ast_node,
    ast_node,
    entity_attr<template_name>::type,
    name_attr,
    scope_attr
>;


}
