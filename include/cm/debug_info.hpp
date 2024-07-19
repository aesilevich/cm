// Copyright (c) 2024, Alexandr Esilevich
// 
// Distributed under the BSD 2-Clause License.
// See accompanying file LICENSE for license information.
//

/// \file debug_info.hpp
/// Contains definition of the debug_info class.

#pragma once

#include "debug_symbol.hpp"
#include "record_type_debug_info.hpp"
#include "type_debug_info.hpp"
#include <map>
#include <variant>


namespace cm {

class entity;


/// Represents debug info for object located in memory
class object_debug_info {
public:
    /// Constructs object debug info with specified address and size
    object_debug_info(uint64_t a, uint64_t sz): addr_{a}, size_{sz} {}

    /// Returns object address
    auto addr() const { return addr_; }

    /// Sets object address
    void set_addr(uint64_t a) { addr_ = a; }

    /// Returns object size
    auto size() const { return size_; }

    /// Sets object size
    void set_size(uint64_t sz) { size_ = sz; }

private:
    uint64_t addr_;         ///< Function address
    uint64_t size_;         ///< Function size
};


/// Debug info for code model
class debug_info {
public:
    /// Constructs empty debug info for specified code model
    debug_info(code_model & cm);

    /// Adds debug info for type
    void add_type(const type_t * type, type_debug_info_up && dbg) {
        types_.emplace(type, std::move(dbg));
    }

    /// Adds type debug info with specified size of type
    void add_type(const type_t * type, size_t sz) {
        add_type(type, std::make_unique<type_debug_info>(sz));
    }

    /// Returns debug info for type or null pointer if debug info not found
    const type_debug_info * type(const type_t * t) const {
        auto res = types_.find(t);
        if (res == types_.end()) {
            return nullptr;
        }

        return res->second.get();
    }

    /// Returns debug info for type or null pointer if debug info not found
    const type_debug_info * type(const const_qual_type & t) const {
        return type(t.type());
    }

    /// Returns casted debug info for type or null pointer if debug info not found
    template <typename T>
    const T * type_as(const const_qual_type & t) const {
        return dynamic_cast<const T*>(type(t));
    }

    /// Returns size of pointer in bytes
    auto ptr_size() const { return ptr_size_; }

    /// Sets size of pointer in bytes
    void set_ptr_size(unsigned int sz) { ptr_size_ = sz; }

    /// Returns type size or 0 if not found in debug info
    uint64_t find_type_size(const const_qual_type & t) const;

    /// Returns type size. Debug info must exist for type
    uint64_t type_size(const const_qual_type & t) const;

    /// Makes default layout for specified record type
    const record_type_debug_info * make_def_rec_layout(record_type * rec,
                                                       bool recursive = true);

    /// Returns object debug info for code model entity, or nullptr if object not found
    object_debug_info * object(entity * ent) const {
        auto it = objects_.find(ent);
        if (it == objects_.end()) {
            return nullptr;
        }

        return it->second.get();
    }

    /// Adds object debug info for entity
    void add_object(entity * ent, std::unique_ptr<object_debug_info> && d) {
        auto dinfo = d.get();

        // addung object debug info into list of objects
        auto res = objects_.emplace(ent, std::move(d));
        assert(res.second && "debug info for function already exist");

        // adding object into address map
        auto addr_res = obj_addrs_.emplace(dinfo->addr(), ent);
        assert(addr_res.second && "function with specified address already exists");
    }

    /// Adds object debug info with specified address and size
    void add_object(entity * ent, uint64_t addr, uint64_t sz) {
        auto dinfo = std::make_unique<object_debug_info>(addr, sz);
        add_object(ent, std::move(dinfo));
    }

    /// Finds entity located at specified address. Returns nullptr if object not found
    entity * entity_at_addr(uint64_t addr) const;

    /// Adds symbol with specified address, name, and size into debug info
    void add_sym(uint64_t addr, const std::string & name, uint64_t sz) {
        symbols_.emplace(addr, std::make_unique<debug_symbol>(name, addr, sz));
    }

    /// Finds symbol located at specified address. Returns nullptr if symbol not found
    debug_symbol * sym_at_addr(uint64_t addr) const;

private:
    code_model & cm_;       ///< Reference to code model

    /// Map of debug info for types
    std::map<const type_t*, type_debug_info_up> types_;

    /// Map of debug objects for entities
    std::map<entity*, std::unique_ptr<object_debug_info>> objects_;

    /// Map of entity addresses
    std::map<uint64_t, entity*> obj_addrs_;

    /// Map of symbols
    std::multimap<uint64_t, std::unique_ptr<debug_symbol>> symbols_;

    /// Size of pointer
    unsigned int ptr_size_ = 8;
};


}
