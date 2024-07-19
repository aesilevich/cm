// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file context.cpp
/// Contains implementation of the context class.

#include "pch.hpp"
#include "cm/context.hpp"
#include "cm/decltype_type.hpp"
#include "cm/dependent_type.hpp"
#include "cm/function.hpp"
#include "cm/record_type.hpp"
#include "cm/template_function.hpp"
#include "cm/template_instantiation.hpp"
#include "cm/template_record.hpp"
#include <ranges>
#include <sstream>
#include <iostream>


namespace cm {


void context::remove_entity(context_entity * ent) {
    // checking that type context_entity has no uses
    assert(std::ranges::empty(ent->uses()) && "can't remove entity with uses");

    // removing entity from map of named decls if it has name
    if (auto named_ent = dynamic_cast<named_context_entity*>(ent)) {
        remove_named_entity_from_map(named_ent);
    }

    // removing entity
    // TODO: refactor and optimize with iterators
    auto it = std::ranges::find_if(entities_, [ent](auto && t) { return t.get() == ent; });
    assert(it != std::ranges::end(entities_) && "context_entity not found in decl context");
    entities_.erase(it);
}


const named_type * context::find_named_type(const std::string & name) const {
    return find_named_entity<named_type>(name);
}


named_type * context::find_named_type(const std::string & name) {
    return find_named_entity<named_type>(name);
}


record_type * context::create_record(record_kind knd) {
    return create_entity<record_type>(knd);
}


named_record_type * context::find_named_record(const std::string & name) {
    return find_named_entity<named_record_type>(name);
}


const named_record_type * context::find_named_record(const std::string & name) const {
    return find_named_entity<named_record_type>(name);
}


named_record_type * context::create_named_record(const std::string & name, record_kind knd) {
    return create_named_entity_impl<named_record_type>(this, name, knd);
}


const typedef_type * context::find_typedef(const std::string & name) const {
    return find_named_entity<typedef_type>(name);
}


typedef_type * context::find_typedef(const std::string & name) {
    return find_named_entity<typedef_type>(name);
}


typedef_type * context::create_typedef(const std::string & name, const qual_type & base) {
    return create_named_entity<typedef_type>(name, base);
}


const enum_type * context::find_enum(const std::string & name) const {
    return find_named_entity<enum_type>(name);
}


enum_type * context::find_enum(const std::string & name) {
    return find_named_entity<enum_type>(name);
}


enum_type * context::create_enum(const std::string & name, builtin_type * base) {
    return create_named_entity<enum_type>(name, base);
}


variable * context::find_var(const std::string & name) {
    return find_named_entity<variable>(name);
}


variable * context::create_var(const std::string & name, const qual_type & type) {
    return create_named_entity<variable>(name, type);
}


const named_function * context::find_function(const std::string & nm) const {
    return find_named_entity<named_function>(nm);
}


named_function * context::find_function(const std::string & nm) {
    return find_named_entity<named_function>(nm);
}


named_function * context::create_function(const std::string & name) {
    return create_named_entity<named_function>(name);
}


const template_ * context::find_template(const std::string & name) const {
    return find_named_entity<template_>(name);
}


template_ * context::find_template(const std::string & name) {
    return find_named_entity<template_>(name);
}


const template_function * context::find_template_function(const std::string & name) const {
    return find_named_entity<template_function>(name);
}


template_function * context::find_template_function(const std::string & name) {
    return find_named_entity<template_function>(name);
}


template_function * context::create_template_function(const std::string & name) {
    return create_named_entity<template_function>(name);
}


const template_record * context::find_template_record(const std::string & name) const {
    return find_named_entity<template_record>(name);
}


template_record * context::find_template_record(const std::string & name) {
    return find_named_entity<template_record>(name);
}


void context::remove_named_entity_from_map(named_context_entity * ent) {
    auto [first, last] = named_entities_.equal_range(ent->name());
    auto it = std::find_if(first, last, [ent](auto && p) { return p.second == ent; });
    assert(it != last && "named type not found in map");
    named_entities_.erase(it);
}


template_record_instantiation_type *
context::dynamic_cast_template_record_instantiation_type(template_instantiation * inst) {
    return dynamic_cast<template_record_instantiation_type*>(inst);
}


void context::dump_entities(std::ostream & str,
                            const dump_options & opts,
                            unsigned int indent) const {
    bool prev_entity_is_context = false;
    bool first = true;

    for (auto && ent : entities()) {
        // skipping builtins if dump_builtins is false
        if (!opts.builtins && ent->is_builtin()) {
            continue;
        }

        // skipping dependent_type and decltype types
        if (dynamic_cast<const dependent_type*>(ent) ||
            dynamic_cast<const decltype_type*>(ent)) {
            continue;
        }

        // skipping template parameters
        if (dynamic_cast<const template_parameter*>(ent)) {
            continue;
        }

        bool entity_is_context = dynamic_cast<const context*>(ent) != nullptr &&
                                 dynamic_cast<const function*>(ent) == nullptr;

        // printing separator line between context entities (records, namespaces, etc)
        if (prev_entity_is_context || (entity_is_context && !first)) {
            str << "\n";
        }

        // dumping entity
        ent->dump(str, opts, indent);

        first = false;
        prev_entity_is_context = entity_is_context;
    }
}


}
