// Copyright (c) 2024, Alexandr Esilevich
//
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_variable_partial_specialization_converter.cpp
/// Contains implementation of the template_variable_partial_specialization_converter class.

#include "pch.hpp"

#include "template_variable_partial_specialization_converter.hpp"
#include "converter_impl.hpp"
#include "convert_loc.hpp"
#include "convert_record.hpp"
#include "convert_template.hpp"


namespace cm::cxx::clang {


template_variable_partial_specialization *
template_variable_partial_specialization_converter::create_entity(converter_impl & conv,
                                                                  const clang_decl_t * decl) {
    // getting existing or creating new code model entity for template variable
    auto templ_decl = decl->getSpecializedTemplate();
    assert(templ_decl && "can't get clang template decl for specialization");
    auto templ = conv.decl_entity_as<template_variable>(templ_decl);

    return templ->create_partial_specialization();
}


template_variable_partial_specialization *
template_variable_partial_specialization_converter::convert_decl(converter_impl & conv,
                                                                 const clang_decl_t * decl) {
    // getting existing or creating new entity for partial specialization
    auto spec = conv.decl_entity_as<template_variable_partial_specialization>(decl);

    // performing conversion only for definition or canonical declaration if there is
    // no definition
    auto def_decl = decl->getDefinition();
    if (def_decl != nullptr && def_decl != decl ||
        def_decl == nullptr && !decl->isCanonicalDecl()) {
        return spec;
    }

    spec->set_access_lev(get_clang_decl_acc_level(decl));
    spec->set_loc(convert_loc(conv, decl->getLocation()));

    // setting context to template variable partial specialization
    converter_impl::decl_context_setter csetter{conv, spec, decl};

    // converting template parameters
    convert_template_params(conv, spec, decl->getTemplateParameters());

    // converting template arguments
    auto args = convert_template_arguments(conv, decl->getTemplateArgs().asArray());
    for (auto && arg : args) {
        spec->add_arg(arg);
    }

    // converting variable type after template parameters because it can use them
    spec->set_type(conv.type(decl->getType()));

    return spec;
}


}
