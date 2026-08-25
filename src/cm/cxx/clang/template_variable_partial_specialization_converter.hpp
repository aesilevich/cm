// Copyright (c) 2024, Alexandr Esilevich
//
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_variable_partial_specialization_converter.hpp
/// Contains definition of the template_variable_partial_specialization_converter class.

#pragma once

#include "decl_converter.hpp"
#include "cm/cm.hpp"
#include <clang/AST/DeclTemplate.h>


namespace cm::cxx::clang {


/// Template variable partial specialization converter
struct template_variable_partial_specialization_converter {
    /// Type of required clang node
    using clang_decl_t = ::clang::VarTemplatePartialSpecializationDecl;

    /// Checks if clang decl matches converter
    static constexpr bool match(const clang_decl_t * decl) { return true; }

    /// Creates entity for clang template variable partial specialization decl
    static template_variable_partial_specialization *
    create_entity(converter_impl & conv, const clang_decl_t * decl);

    /// Converts clang template variable partial specialization decl
    static template_variable_partial_specialization *
    convert_decl(converter_impl & conv, const clang_decl_t * decl);
};


static_assert(decl_converter<template_variable_partial_specialization_converter>);


}
