// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file context.hpp
/// Contains definition of the context class.

#pragma once

#include "context_entity.hpp"
#include "enum_type.hpp"
#include "function_type.hpp"
#include "record_kind.hpp"
#include "typedef_type.hpp"
#include "variable.hpp"
#include <ranges>
#include <unordered_map>
#include <sstream>


namespace cm {


class code_model;
class function;
class record_type;
class named_function;
class named_record_type;
class template_;
class template_function_instantiation;
class template_function;
class template_instantiation;
class template_record;
class template_record_instantiation_type;


/// Represents context in code model that contains code model entities
class context: virtual public context_entity {
public:
    /// Exception that is thrown in case of type cast errors
    struct type_cast_error: public std::runtime_error {
        type_cast_error(const std::string & msg):
            std::runtime_error{msg} {}
    };

protected:
    /// Constructs context with specified optional parent context
    context(context * p):
        context_entity(p) {}

public:
    /// Default destructor
    virtual ~context() = default;

    /// Returns true if context_entity context is root (has no parent)
    bool is_root() const { return ctx() == nullptr; }

    /// Returns default access level for this context
    virtual access_level default_access_level() const = 0;


    //////////////////////////////////////////////////////////////////////
    // Enetities

    /// Returns range of const entities in context, filtering them by entity type
    /// if requested
    template <typename Entity = context_entity>
    auto entities() const {
        static_assert(std::derived_from<Entity, context_entity>, "invalid entity filter type");

        if constexpr (std::same_as<Entity, context_entity>) {
            auto transform_fn = [](auto && ptr) { return const_cast<const context_entity*>(ptr.get()); };
            return entities_ | std::ranges::views::transform(transform_fn);
        } else {
            auto transform_fn = [](auto && ptr) {
                return dynamic_cast<const Entity*>(ptr.get());
            };

            auto filter_fn = [](const Entity * ent) {
                return ent != nullptr;
            };

            return entities_
                | std::ranges::views::transform(transform_fn)
                | std::ranges::views::filter(filter_fn);
        }
    }

    /// Returns range of entities in context, filtering them by entity type if requested
    template <typename Entity = context_entity>
    auto entities() {
        static_assert(std::derived_from<Entity, context_entity>, "invalid entity filter type");

        if constexpr (std::same_as<Entity, context_entity>) {
            auto transform_fn = [](auto && ptr) { return ptr.get(); };
            return entities_ | std::ranges::views::transform(transform_fn);
        } else {
            auto transform_fn = [](auto && ptr) {
                return dynamic_cast<Entity*>(ptr.get());
            };

            auto filter_fn = [](Entity * ent) {
                return ent != nullptr;
            };

            return entities_
                | std::ranges::views::transform(transform_fn)
                | std::ranges::views::filter(filter_fn);
        }
    }

    /// Removes entity from context. The entity must have no uses
    virtual void remove_entity(context_entity * ent);

    /// Returns pointer to existing named entity of specified type in context
    /// or nullptr if entity does not exist
    template <typename Entity = named_context_entity>
    Entity * find_named_entity(const std::string & name) {
        static_assert(std::derived_from<Entity, named_context_entity>, "invalid named entity type");

        auto it = named_entities_.find(name);
        if (it == named_entities_.end()) {
            return nullptr;
        }

        if constexpr (std::same_as<Entity, named_context_entity>) {
            return it->second;
        } else {
            return dynamic_cast<Entity*>(it->second);
        }
    }

    /// Returns const pointer to existing named entity of specified type in context
    /// or nullptr if entity does not exist
    template <typename Entity = named_context_entity>
    const Entity * find_named_entity(const std::string & name) const {
        static_assert(std::derived_from<Entity, named_context_entity>, "invalid named entity type");

        auto it = named_entities_.find(name);
        if (it == named_entities_.end()) {
            return nullptr;
        }

        if constexpr (std::same_as<Entity, named_context_entity>) {
            return it->second;
        } else {
            return dynamic_cast<const Entity*>(it->second);
        }
    }

    /// Creates entity in this context
    template <typename Entity, typename ... Args>
    Entity * create_entity(Args && ... args) {
        static_assert(!std::is_base_of<named_context_entity, Entity>::value,
                      "only create_named_entity_impl can be used for creating named entities");
        return create_entity_impl<Entity>(this, std::forward<Args>(args)...);
    }

    /// Creates named entity in this context
    template <typename Entity, typename ... Args>
    Entity * create_named_entity(const std::string & name, Args && ... args) {
        return create_named_entity_impl<Entity>(this, name, std::forward<Args>(args)...);
    }

    /// Renames named entity in this context
    template <typename String>
    requires (std::assignable_from<std::string &, String>)
    void rename_entity(named_context_entity * ent, String && str) {
        if (ent->name() == str) {
            return;
        }

        remove_named_entity_from_map(ent);
        named_entities_.emplace(str, ent);
        ent->set_name_impl(std::forward<String>(str));
    }


    //////////////////////////////////////////////////////////////////////
    // Nested types

    /// Returns const range of types in context
    auto types() const { return entities<context_type>(); }

    /// Returns range of types in context
    auto types() { return entities<context_type>(); }

    /// Returns range of named types in context
    auto named_types() const { return entities<named_type>(); }

    /// Returns range of named types in context
    auto named_types() { return entities<named_type>(); }

    /// Searches for named type in context. Returns pointer to type or null if type not found
    const named_type * find_named_type(const std::string & name) const;

    /// Searches for named type in context. Returns pointer to type or null if type not found
    named_type * find_named_type(const std::string & name);


    //////////////////////////////////////////////////////////////////////
    // Nested record types

    /// Returns const range of records in context
    /// NOTE: implemented in record_type.hpp
    auto records() const;

    /// Returns range of records in context
    /// NOTE: implemented in record_type.hpp
    auto records();

    /// Creates anonymous record type in context
    record_type * create_record(record_kind knd = record_kind::struct_);


    //////////////////////////////////////////////////////////////////////
    // Nested named record types

    /// Returns const range of records in context
    /// NOTE: implemented in record_type.hpp
    auto named_records() const;

    /// Returns range of records in context
    /// NOTE: implemented in record_type.hpp
    auto named_records();

    /// Searches for record with specified name
    named_record_type * find_named_record(const std::string & name);

    /// Searches for record with specified name
    const named_record_type * find_named_record(const std::string & name) const;

    /// Creates named record in context
    named_record_type * create_named_record(const std::string & name,
                                            record_kind knd = record_kind::struct_);


    //////////////////////////////////////////////////////////////////////
    // Nested typedef types

    /// Returns const range of nested typedef types
    auto typedefs() const { return entities<typedef_type>(); }

    /// Returns const range of nested typedef types
    auto typedefs() { return entities<typedef_type>(); }

    /// Searches for typedef with specified name. Returns null if typedef not found
    const typedef_type * find_typedef(const std::string & name) const;

    /// Searches for typedef with specified name. Returns null if typedef not found
    typedef_type * find_typedef(const std::string & name);

    /// Creates nested typedef type in this context
    typedef_type * create_typedef(const std::string & name, const qual_type & base);


    //////////////////////////////////////////////////////////////////////
    // Nested enum types

    /// Returns const range of nested enum types
    auto enums() const { return entities<enum_type>(); }

    /// Returns range of nested enum types
    auto enums() { return entities<enum_type>(); }

    /// Searches for enum type with specified name. Returns null if enum not found
    const enum_type * find_enum(const std::string & name) const;

    /// Searches for enum type with specified name. Returns null if enum not found
    enum_type * find_enum(const std::string & name);

    /// Creates enum in context
    enum_type * create_enum(const std::string & name, builtin_type * base);


    //////////////////////////////////////////////////////////////////////
    // Variables

    /// Returns const range of variables in context
    auto vars() const { return entities<variable>(); }

    /// Returns range of variables in context
    auto vars() { return entities<variable>(); }

    /// Finds variable with specified name. Returns nullptr if variable not found.
    variable * find_var(const std::string & name);

    /// Creates variable with specified name and type in context
    virtual variable * create_var(const std::string & name, const qual_type & type);


    //////////////////////////////////////////////////////////////////////
    // Functions

    /// Returns const range of functions in this context
    /// NOTE: implemented in function.hpp
    auto functions() const;

    /// Returns range of functions in this context
    /// NOTE: implemented in function.hpp
    auto functions();

    /// Returns const range of named functions in this context
    /// NOTE: implemented in function.hpp
    auto named_functions() const;

    /// Returns range of named functions in this context
    /// NOTE: implemented in function.hpp
    auto named_functions();

    /// Searches for function with specified name. Returns null if function not found
    const named_function * find_function(const std::string & nm) const;

    /// Searches for function with specified name. Returns null if function not found
    named_function * find_function(const std::string & nm);

    /// Creates function with specified name
    named_function * create_function(const std::string & name);


    //////////////////////////////////////////////////////////////////////
    // Templates

    /// Returns const range of templates
    /// NOTE: implemented in template.hpp
    auto templates() const;

    /// Returns range of templates
    /// NOTE: implemented in template.hpp
    auto templates();

    /// Searches for template with specified name. Returns nullptr if templte not found.
    const template_ * find_template(const std::string & name) const;

    /// Searches for template with specified name. Returns nullptr if templte not found.
    template_ * find_template(const std::string & name);


    //////////////////////////////////////////////////////////////////////
    // Template functions

    /// Returns const range of template functions
    /// NOTE: implemented in template_function.hpp
    auto template_functions() const;

    /// Returns const range of template functions
    /// NOTE: implemented in template_function.hpp
    auto template_functions();

    /// Searches for template function with specified name. Returns nullptr if not found.
    const template_function * find_template_function(const std::string & name) const;

    /// Searches for template function with specified name. Returns nullptr if not found.
    template_function * find_template_function(const std::string & name);

    /// Creates template function in this context
    template_function * create_template_function(const std::string & name);


    //////////////////////////////////////////////////////////////////////
    // Template records

    /// Returns const range of template records
    /// NOTE: implemented in template_record.hpp
    auto template_records() const;

    /// Returns range of template records
    /// NOTE: implemented in template_record.hpp
    auto template_records();

    /// Searches for template record with specified name. Return null if not found.
    const template_record * find_template_record(const std::string & name) const;

    /// Searches for template record with specified name. Return null if not found.
    template_record * find_template_record(const std::string & name);

    /// Creates template record with specified name, kind, parameter pack flag, and parameters list
    template <typename ... Params>
    template_record * create_template_record(const std::string & name,
                                             record_kind knd,
                                             Params && ... params) {
        return create_named_entity<template_record>(name, knd, std::forward<Params>(params)...);
    }

    /// Helper function that gets existing or creates new template record
    template <typename ... Params>
    template_record * get_or_create_template_record(const std::string & name,
                                                    record_kind knd,
                                                    bool is_variadic,
                                                    Params && ... params);


    //////////////////////////////////////////////////////////////////////
    // Dump utilities

    /// Dumps context entities to output stream
    void dump_entities(std::ostream & str, const dump_options & opts, unsigned int indent) const;

protected:
    /// Creates entity in context with custom context type and dds it into list of entities
    template <typename Entity, typename Context, typename ... Args>
    Entity * create_entity_impl(Context * ctx, Args && ... args) {
        auto ent = std::make_unique<Entity>(ctx, std::forward<Args>(args)...);
        auto res = ent.get();
        entities_.push_back(std::move(ent));
        return res;
    }

    /// Creates named entity in context with custom context type
    template <typename Entity, typename Context, typename ... Args>
    Entity * create_named_entity_impl(Context * ctx, const std::string & name, Args && ... args) {
        static_assert(std::is_base_of<named_context_entity, Entity>::value,
                      "T should be derived from named_entity");
        auto res = create_entity_impl<Entity>(ctx, name, std::forward<Args>(args)...);
        named_entities_.emplace(name, res);
        return res;
    }

private:
    /// Removes named entity from map of named entities
    void remove_named_entity_from_map(named_context_entity * ent);

    /// Dynamic casts template instantiation to record type
    static template_record_instantiation_type *
    dynamic_cast_template_record_instantiation_type(template_instantiation * inst);

    /// Vector of entities in context
    std::vector<std::unique_ptr<context_entity>> entities_;

    /// Map of named entities in context
    std::unordered_multimap<std::string, named_context_entity*> named_entities_;
};


/// Entity context parametrized by variable and function types
template <typename Variable = variable, typename Function = function>
class entity_context_t: virtual public context {
public:
    /// Constructs context_entity context with specified parent
    explicit entity_context_t(context * parent):
        context{parent},
        context_entity{parent} {}


    //////////////////////////////////////////////////////////////////////
    // Variables

    /// Returns const range of variables in context
    auto vars() const { return entities<Variable>(); }

    /// Returns range of variables in context
    auto vars() { return entities<Variable>(); }
};


}
