// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file template_record.cpp
/// Contains implementation of template record related types.

#include "pch.hpp"
#include "cm/template_record.hpp"


namespace cm {


//////////////////////////////////////////////////////////////////////
// template_record_type

template_record_type::template_record_type(context * ctx):
context_type{ctx},
context_entity{ctx} {
}


template_record * template_record_type::templ() {
    auto res = dynamic_cast<template_record*>(ctx());
    assert(res && "parent context of template record type is not a template record");
    return res;
}


const template_record * template_record_type::templ() const {
    auto res = dynamic_cast<const template_record*>(ctx());
    assert(res && "parent context of template record type is not a template record");
    return res;
}


void template_record_type::print_desc(std::ostream & str) const {
    str << templ()->name();
}


void template_record_type::dump(std::ostream & str,
                                const dump_options & opts,
                                unsigned int indent) const {
    print_indent_spaces(str, indent);
    str << "injected " << record_kind_name(templ()->kind()) << ' ' << templ()->name();
    context_entity::dump_loc(str, opts);
    str << ";\n";
}



//////////////////////////////////////////////////////////////////////
// template_record

template_record_type * template_record::this_type() {
    // injected type must be the first type in template record context
    auto ents = entities<template_record_type>();
    assert(!std::ranges::empty(ents) && "no injected template record type in context");
    return ents.front();
}


const template_record_type * template_record::this_type() const {
    // injected type must be the first type in template record context
    auto ents = entities<template_record_type>();
    assert(!std::ranges::empty(ents) && "no injected template record type in context");
    return ents.front();
}


}
