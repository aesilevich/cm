// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file type_template_parameter_converter.cpp
/// Contains implementation of the type_template_parameter_converter class.

#include "pch.hpp"

#include "type_template_parameter_converter.hpp"
#include "log.hpp"


namespace cm::cxx::clang {


type_template_parameter *
type_template_parameter_converter::type(converter_impl & conv, const clang_type_t * type) {
    templated_entity * templ = nullptr;

    // searching for template declaration in the chain of parent declarations, starting
    // from the declaration being converted

    auto c_decl = conv.clang_decl();
    while (c_decl != nullptr) {
        if (auto pars = c_decl->getDescribedTemplateParams(); pars != nullptr) {
            assert(pars->getDepth() >= type->getDepth() &&
                   "missed template parameter list in parents chain");
            if (pars->getDepth() == type->getDepth()) {
                templ = conv.get_decl_entity_as<templated_entity>(c_decl);
                assert(templ && "can't find templated entity");
                break;
            }
        }

        // moving to the parent declaration
        auto parent_ctx = c_decl->getDeclContext();
        c_decl = parent_ctx != nullptr ? ::clang::dyn_cast<::clang::Decl>(parent_ctx) : nullptr;
    }

    assert(templ && "can't find template for template parameter type");

    // getting template parameter from templated entity
    auto pars = templ->template_params();
    assert(type->getIndex() < std::ranges::distance(pars) && "invalid template parameter index");
    auto it = std::ranges::begin(pars);
    std::advance(it, type->getIndex());
    auto par = *it;

    // checking parameter type
    auto type_par = dynamic_cast<type_template_parameter*>(par);
    assert(type_par && "template parameter is not a type");
    return type_par;

}


}
