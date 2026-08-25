// Copyright (c) 2024, Alexandr Esilevich
//
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_variable_converter.cpp
/// Contains implementation of the template_variable_converter class

#include "pch.hpp"

#include "template_variable_converter.hpp"
#include "converter_impl.hpp"
#include "convert_loc.hpp"
#include "convert_record.hpp"
#include "convert_template.hpp"
#include "template_variable_specialization_converter.hpp"


namespace cm::cxx::clang {


template_variable * template_variable_converter::create_entity(converter_impl & conv,
                                                               const clang_decl_t * decl) {
    auto parent_ctx = conv.parent_ctx(decl);
    auto var = parent_ctx->create_template_variable(decl->getNameAsString());

    // adding additional map for described variable declaration to make sure it's possible
    // to find template variable entity from VarDecl
    conv.add_decl_entity(decl->getTemplatedDecl(), var);

    return var;
}


template_variable * template_variable_converter::convert_decl(converter_impl & conv,
                                                              const clang_decl_t * decl) {
    // looking for existing CM declaration associated with clang template declaration
    auto var = conv.decl_entity_as<template_variable>(decl);

    auto var_decl = decl->getTemplatedDecl();

    // performing conversion only for definition or canonical declaration if there is
    // no definition
    auto def_decl = var_decl->getDefinition();
    if (def_decl != nullptr && def_decl != var_decl ||
        def_decl == nullptr && !decl->isCanonicalDecl()) {
        return var;
    }

    var->set_access_lev(get_clang_decl_acc_level(decl));
    var->set_loc(convert_loc(conv, decl->getLocation()));

    // setting new context before converting template parameters
    converter_impl::decl_context_setter csetter{conv, var, var_decl};

    // converting template parameters
    convert_template_params(conv, var, decl->getTemplateParameters());

    // converting variable type after template parameters because it can use them
    var->set_type(conv.type(var_decl->getType()));

    // converting template specializations only for canonical decl
    // (canonical decl is only one in all translation unit)
    if (decl->isCanonicalDecl()) {
        for (auto && spec : decl->specializations()) {
            template_variable_specialization_converter::convert_decl(conv, spec);
        }
    }

    return var;
}


}
