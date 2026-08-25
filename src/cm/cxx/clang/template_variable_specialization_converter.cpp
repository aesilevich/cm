// Copyright (c) 2024, Alexandr Esilevich
//
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_variable_specialization_converter.cpp
/// Contains implementation of the template_variable_specialization_converter class.

#include "pch.hpp"

#include "template_variable_specialization_converter.hpp"
#include "converter_impl.hpp"
#include "convert_loc.hpp"
#include "convert_record.hpp"
#include "convert_template.hpp"


namespace cm::cxx::clang {


bool template_variable_specialization_converter::match(const clang_decl_t * decl) {
    // skipping template variable partial specializations
    return ::clang::dyn_cast<::clang::VarTemplatePartialSpecializationDecl>(decl) == nullptr;
}


variable * template_variable_specialization_converter::create_entity(converter_impl & conv,
                                                                     const clang_decl_t * decl) {
    // getting existing or creating new code model entity for template variable
    auto templ_decl = decl->getSpecializedTemplate();
    assert(templ_decl && "can't get clang template decl for specialization");
    auto templ = conv.decl_entity_as<template_variable>(templ_decl);

    // creating new template variable instantiation or specialization
    auto args = convert_template_arguments(conv, decl->getTemplateArgs().asArray());
    variable * var = decl->isExplicitSpecialization() ?
                     static_cast<variable*>(templ->create_specialization(args)) :
                     static_cast<variable*>(templ->create_instantiation(args));

    var->set_loc(convert_loc(conv, decl->getLocation()));

    return var;
}


variable * template_variable_specialization_converter::convert_decl(converter_impl & conv,
                                                                    const clang_decl_t * decl) {
    // getting existing or creating new entity for template variable specialization
    auto var = conv.decl_entity_as<template_variable_instantiation>(decl);

    var->set_access_lev(get_clang_decl_acc_level(decl));
    var->set_type(conv.type(decl->getType()));

    return var;
}


}
