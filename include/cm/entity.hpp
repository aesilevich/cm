// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file entity.hpp
/// Contains definition of the entity class.

#pragma once

#include <cassert>
#include <iomanip>
#include <iostream>
#include <ranges>
#include <set>
#include <sstream>


namespace cm {


/// Abstract use of code model entity
class entity_use {
public:
    /// Default virtual destructor
    virtual ~entity_use() = default;
};


/// Options for dumping entities
struct dump_options {
    bool builtins = false;
    bool locations = false;
};


/// Base class for all code model entities
class entity: virtual public entity_use {
public:
    /// Destroys entity. Checks that entity has no uses.
    virtual ~entity() {
        assert(uses_.empty() && "can't delete entity with uses");
    }

    /// Performs dynamic cast to specified type
    template <typename T>
    T * cast() { return dynamic_cast<T*>(this); }

    /// Performs dynamic cast to specified type
    template <typename T>
    const T * cast() const { return dynamic_cast<const T*>(this); }

    /// Checks if type can be casted to another type
    template <typename T>
    bool is() const { return cast<T>() != nullptr; }

    /// Returns const range of entity uses
    auto uses() const {
        auto fn = [](auto && use) { return const_cast<const entity_use*>(use); };
        return uses_ | std::ranges::views::transform(fn);
    }

    /// Returns range of entity uses
    auto uses() {
        auto fn = [](auto && use) { return use; };
        return uses_ | std::ranges::views::transform(fn);
    }

    /// Returns const range of entity uses of specified type
    template <typename UseType>
    auto uses() const {
        auto transform_fn = [](const entity_use * use) {
            return dynamic_cast<const UseType*>(use);
        };

        auto filter_fn = [](const UseType * use) {
            return use != nullptr;
        };

        return uses()
            | std::ranges::views::transform(transform_fn)
            | std::ranges::views::filter(filter_fn);
    }

    /// Returns range of entity uses of specified type
    template <typename UseType>
    auto uses() {
        auto transform_fn = [](entity_use * use) {
            return dynamic_cast<UseType*>(use);
        };

        auto filter_fn = [](UseType * use) {
            return use != nullptr;
        };

        return uses()
            | std::ranges::views::transform(transform_fn)
            | std::ranges::views::filter(filter_fn);
    }

    /// Adds use of type
    void add_use(entity_use * use) {
        uses_.insert(use);
    }

    /// Removes use of type
    void remove_use(entity_use * use) {
        auto it = uses_.find(use);
        assert(it != uses_.end() && "use does not exist");
        uses_.erase(it);
    }

    /// Returns true if entity is builtin entity (defined by language or compiler)
    virtual bool is_builtin() const { return false; }

    /// Prints entity short description with address to output stream
    void print_desc(std::ostream & str, bool print_addr) const {
        str << std::hex << std::setw(8) << this << " ";
        print_desc(str);
    }

    /// Prints entity short description to output stream
    virtual void print_desc(std::ostream & str) const = 0;

    /// Returns entity short description
    std::string desc() const {
        std::ostringstream str;
        print_desc(str);
        return str.str();
    }

    /// Dumps entity to output stream with indent
    virtual void dump(std::ostream & str, const dump_options & opts, unsigned int indent) const {
        print_indent_spaces(str, indent);
        print_desc(str);
        str << std::endl;
    }

    /// Dumps entity to string
    std::string dump_to_string(const dump_options & opts = {}, unsigned int indent = 0) const {
        std::ostringstream str;
        dump(str, opts, indent);
        return str.str();
    }

    /// Prints indent spaces to output stream
    static void print_indent_spaces(std::ostream & str, unsigned int indent) {
        for (unsigned int i = 0; i < indent * 4; ++i) {
            str << " ";
        }
    }

private:
    std::multiset<entity_use*> uses_;               ///< Set of uses of entity
};


/// Implementation of entity use
template <std::derived_from<entity> Entity>
class entity_use_impl: virtual public entity_use {
public:
    /// Constructs use of entity. Adds this instance to the list of entity uses.
    explicit entity_use_impl(Entity * used_ent):
    used_entity_{used_ent} {
        do_add_use();
    }

    entity_use_impl(const entity_use_impl &) = delete;
    entity_use_impl(entity_use_impl &&) = delete;
    entity_use_impl & operator=(const entity_use_impl &) = delete;
    entity_use_impl & operator=(entity_use_impl &&) = delete;

    /// Destroys use of entity. Removes this instance from the list of entity uses.
    ~entity_use_impl() override {
        do_remove_use();
    }

    /// Returns pointer to used entity
    Entity * used_entity() { return used_entity_; }

    /// Returns const pointer to used entity
    const Entity * used_entity() const { return used_entity_; }

    /// Sets pointer to used entity. Removes this use from the list of uses of
    /// current entity and adds this use to the list of uses of the new entity
    void set_used_entity(Entity * ent) {
        do_remove_use();
        used_entity_ = ent;
        do_add_use();
    }

private:
    /// Adds this use to the list of uses of current entity if it's not null
    void do_add_use() {
        if (used_entity_) {
            used_entity_->add_use(this);
        }
    }

    /// Removes this use from the list of uses of current used entity if it's not null
    void do_remove_use() {
        if (used_entity_) {
            used_entity_->remove_use(this);
        }
    }

    Entity * used_entity_;          ///< Pointer to used entity
};


}
